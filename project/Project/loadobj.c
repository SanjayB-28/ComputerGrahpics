// ---------------------------------------------
// loadobj.c - Minimalist Wavefront OBJ loader for OpenGL
// ---------------------------------------------

#include "CSCIx229.h"
#include <ctype.h>

// --- Material structures and storage ---
/* Structure to store material properties from MTL files */
typedef struct {
    char* name;
    float Ka[4], Kd[4], Ks[4], Ns;  // Ambient, diffuse, specular components and shininess
    float d;                         // Transparency
    int map;                         // Texture map ID
} mtl_t;

static int Nmtl = 0;
static mtl_t* mtl = NULL;

// --- File parsing utilities ---
/* Character and line processing for OBJ/MTL file parsing */
static int CRLF(char ch) {
    return ch == '\r' || ch == '\n';
}

static int linelen = 0;
static char* line = NULL;
static char* readline(FILE* f) {
    char ch;
    int k = 0;
    while ((ch = fgetc(f)) != EOF) {
        if (k >= linelen) {
            linelen += 8192;
            line = (char*)realloc(line, linelen);
            if (!line) Fatal("Out of memory in readline\n");
        }
        if (CRLF(ch)) {
            while ((ch = fgetc(f)) != EOF)
                if (!CRLF(ch)) break;
            if (ch != EOF) ungetc(ch, f);
            break;
        } else {
            line[k++] = ch;
        }
    }
    if (k > 0) line[k] = 0;
    return k > 0 ? line : NULL;
}

static char* getword(char** line) {
    while (**line && isspace(**line)) (*line)++;
    if (!**line) return NULL;
    char* word = *line;
    while (**line && !isspace(**line)) (*line)++;
    if (**line) {
        **line = 0;
        (*line)++;
    }
    return word;
}

/* Parse floating point values from OBJ/MTL data */
static void readfloat(char* line, int n, float x[]) {
    for (int i = 0; i < n; i++) {
        char* str = getword(&line);
        if (!str) Fatal("Premature EOL reading %d floats\n", n);
        if (sscanf(str, "%f", x + i) != 1) Fatal("Error reading float %d\n", i);
    }
}

static void readcoord(char* line, int n, float* x[], int* N, int* M) {
    if (*N + n > *M) {
        *M += 8192;
        *x = (float*)realloc(*x, (*M) * sizeof(float));
        if (!*x) Fatal("Cannot allocate memory\n");
    }
    readfloat(line, n, (*x) + *N);
    (*N) += n;
}

static char* readstr(char* line, const char* skip) {
    while (*skip && *line && *skip == *line) {
        skip++;
        line++;
    }
    if (*skip || !isspace(*line)) return NULL;
    return getword(&line);
}

// --- Material loading and management ---
/* Load and process material definitions from MTL files */
static void LoadMaterial(const char* file) {
    int k = -1;
    char* line;
    char* str;
    FILE* f = fopen(file, "r");
    if (!f) {
        fprintf(stderr, "Cannot open material file %s\n", file);
        return;
    }
    while ((line = readline(f))) {
        if ((str = readstr(line, "newmtl"))) {
            int l = strlen(str);
            k = Nmtl++;
            mtl = (mtl_t*)realloc(mtl, Nmtl * sizeof(mtl_t));
            mtl[k].name = (char*)malloc(l + 1);
            if (!mtl[k].name) Fatal("Cannot allocate %d for name\n", l + 1);
            strcpy(mtl[k].name, str);
            mtl[k].Ka[0] = mtl[k].Ka[1] = mtl[k].Ka[2] = 0;   mtl[k].Ka[3] = 1;
            mtl[k].Kd[0] = mtl[k].Kd[1] = mtl[k].Kd[2] = 0;   mtl[k].Kd[3] = 1;
            mtl[k].Ks[0] = mtl[k].Ks[1] = mtl[k].Ks[2] = 0;   mtl[k].Ks[3] = 1;
            mtl[k].Ns = 0;
            mtl[k].d = 0;
            mtl[k].map = 0;
        } else if (k < 0) {
            // No material yet
        } else if (line[0] == 'K' && line[1] == 'a') {
            readfloat(line + 2, 3, mtl[k].Ka);
        } else if (line[0] == 'K' && line[1] == 'd') {
            readfloat(line + 2, 3, mtl[k].Kd);
        } else if (line[0] == 'K' && line[1] == 's') {
            readfloat(line + 2, 3, mtl[k].Ks);
        } else if (line[0] == 'N' && line[1] == 's') {
            readfloat(line + 2, 1, &mtl[k].Ns);
            if (mtl[k].Ns > 128) mtl[k].Ns = 128;
        } else if ((str = readstr(line, "map_Kd"))) {
            mtl[k].map = LoadTexBMP(str);
        }
    }
    fclose(f);
}

/* Apply a material to the current OpenGL state */
static void SetMaterial(const char* name) {
    for (int k = 0; k < Nmtl; k++) {
        if (!strcmp(mtl[k].name, name)) {
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mtl[k].Ka);
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mtl[k].Kd);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mtl[k].Ks);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &mtl[k].Ns);
            if (mtl[k].map) {
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, mtl[k].map);
            } else {
                glDisable(GL_TEXTURE_2D);
            }
            return;
        }
    }
    fprintf(stderr, "Unknown material %s\n", name);
}

// --- Main OBJ loading function ---
/* Loads a Wavefront OBJ file into an OpenGL display list
   Handles geometry, normals, texture coordinates and materials */
int LoadOBJ(const char* file) {
    int Nv, Nn, Nt;        // Current count of vertices, normals, textures
    int Mv, Mn, Mt;        // Allocated memory for vertices, normals, textures
    float* V;              // Vertex coordinates
    float* N;              // Normal vectors
    float* T;              // Texture coordinates
    char* line;
    char* str;
    FILE* f = fopen(file, "r");
    if (!f) Fatal("Cannot open file %s\n", file);
    mtl = NULL;
    Nmtl = 0;
    int list = glGenLists(1);
    glNewList(list, GL_COMPILE);
    glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT);
    V = N = T = NULL;
    Nv = Nn = Nt = 0;
    Mv = Mn = Mt = 0;
    while ((line = readline(f))) {
        if (line[0] == 'v' && line[1] == ' ')
            readcoord(line + 2, 3, &V, &Nv, &Mv);
        else if (line[0] == 'v' && line[1] == 'n')
            readcoord(line + 2, 3, &N, &Nn, &Mn);
        else if (line[0] == 'v' && line[1] == 't')
            readcoord(line + 2, 2, &T, &Nt, &Mt);
        else if (line[0] == 'f') {
            line++;
            glBegin(GL_POLYGON);
            while ((str = getword(&line))) {
                int Kv, Kt, Kn;
                if (sscanf(str, "%d/%d/%d", &Kv, &Kt, &Kn) == 3) {
                    if (Kv < 0 || Kv > Nv / 3) Fatal("Vertex %d out of range 1-%d\n", Kv, Nv / 3);
                    if (Kn < 0 || Kn > Nn / 3) Fatal("Normal %d out of range 1-%d\n", Kn, Nn / 3);
                    if (Kt < 0 || Kt > Nt / 2) Fatal("Texture %d out of range 1-%d\n", Kt, Nt / 2);
                } else if (sscanf(str, "%d//%d", &Kv, &Kn) == 2) {
                    if (Kv < 0 || Kv > Nv / 3) Fatal("Vertex %d out of range 1-%d\n", Kv, Nv / 3);
                    if (Kn < 0 || Kn > Nn / 3) Fatal("Normal %d out of range 1-%d\n", Kn, Nn / 3);
                    Kt = 0;
                } else if (sscanf(str, "%d", &Kv) == 1) {
                    if (Kv < 0 || Kv > Nv / 3) Fatal("Vertex %d out of range 1-%d\n", Kv, Nv / 3);
                    Kn = 0;
                    Kt = 0;
                } else {
                    Fatal("Invalid facet %s\n", str);
                }
                if (Kt) glTexCoord2fv(T + 2 * (Kt - 1));
                if (Kn) glNormal3fv(N + 3 * (Kn - 1));
                if (Kv) glVertex3fv(V + 3 * (Kv - 1));
            }
            glEnd();
        } else if ((str = readstr(line, "usemtl"))) {
            SetMaterial(str);
        } else if ((str = readstr(line, "mtllib"))) {
            LoadMaterial(str);
        }
    }
    fclose(f);
    glPopAttrib();
    glEndList();
    
    /* Cleanup allocated resources */
    for (int k = 0; k < Nmtl; k++)
        free(mtl[k].name);
    free(mtl);
    free(V);
    free(T);
    free(N);
    return list;
}
