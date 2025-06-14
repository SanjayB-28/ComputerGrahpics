#include "CSCIx229.h"
#include <stdbool.h> 

#define PI 3.14159265       
#define numBigTrees 50     
#define numStumps 30                                                                                               
#define numWeeds 50                                                                                               
#define waitTime 16                                                                                                 
#define groundSize 80.0                                                                                             
#define movementSpeed 0.3                                                                                         
#define numFairies 30                                                                                          
#define numRaindrops 300                                                                                         
#define numSnowflakes 300  
#define numButterflies 20                                                                                        
float snowflakes[numSnowflakes][3];                                                                                          
float snowSpeed = 0.2;                                                                                          
bool isSnowing = false;                                                                                          
float raindrops[numRaindrops][3];                                                                                         
float rainSpeed = 0.5;                                                                                         
bool isRaining = false;                                                                                         
float birdX = 0.0, birdY = 15.0, birdZ = -10.0;
float wingAngle = 0.0;                                                                                                                  
int flapDirection = 1;     
float fairies[numFairies][3];                                                                                         
float fairySpeeds[numFairies][2];                                                                                         
float fairyGlow = 0.5;                                                                                         
float glowDirection = 0.01;  
float butterflies[numButterflies][3]; // Positions
float butterflyGlow[numButterflies];  // Glow intensities
int butterflyGlowDirection[numButterflies]; // Glow direction
int axes = 1;
int th = 0;
int ph = 5;
int mode = 0;
double dim = 3;
double len = 2;
int n = 50000;
int current = 0;
double percent = 0;
double points[50000][3];
double colors[50000][3];
int fov = 120; 
double asp = 1;
double EX = 0, EY = 1.5, EZ = 5;   
double AX = 0, AY = 1.5, AZ = 0;   

char* views[] = {"Orthogonal", "Perspective"};


//#define Cos(x) (cos((x)*3.1415927/180))
//#define Sin(x) (sin((x)*3.1415927/180))


int screenWidth = 1000;
int screenHeight = 720;

int helpMenu = 1;       

int isSwaying = 0;                                                                                              
float swayAngle = 0.0;                                                                                          
float swaySpeed = 1.0;                                                                                          


float bigTrees[numBigTrees][5];                                                                                                             
float stumps[numStumps][5];                                                                                                                 
float weeds[numWeeds][5];                                                                                                                   

float generateHeight(float x, float z) {
    return sin(x * 0.1) * cos(z * 0.1) * 5.0;                                                                                         
}
float getCurrentTerrainHeight(float x, float z) {
    return generateHeight(x, z);
}



int w_state, a_state, s_state,
    d_state, q_state, e_state = 0;                                                                                                          
int mousePressed, mouseStartX, mouseStartY = 0;                                                                                             
float xpos = -52, ypos=0, zpos = 59;                                                                                                        
float xrot=-20, yrot=42;                                                                                                                    
float xrotChange, yrotChange = 0;                                                                                                           

int jumpRising=0;
float jumpSpeed=0;                                                                                                              

unsigned int woodTexture;
unsigned int branchTexture;
unsigned int groundTexture;
unsigned int groundTexture2;
unsigned int mountainTexture;

                                                                                        
void bitmapText(float x, float y, float z, char* words) {
    int len = 0;
                                                                                            
    glRasterPos3f(x,y,z);
    len = (int) strlen(words);
                                                                                            
    for(int i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,words[i]);
    }
}

void initRain() {
    for (int i = 0; i < numRaindrops; i++) {
        raindrops[i][0] = rand() % ((int)groundSize * 2) - groundSize;                                                                                         
        raindrops[i][1] = rand() % 20 + 10;                                                                                         
        raindrops[i][2] = rand() % ((int)groundSize * 2) - groundSize;                                                                                         
    }
}

void initSnow() {
    for (int i = 0; i < numSnowflakes; i++) {
        snowflakes[i][0] = rand() % ((int)groundSize * 2) - groundSize;                                                                                          
        snowflakes[i][1] = rand() % 20 + 10;                                                                                          
        snowflakes[i][2] = rand() % ((int)groundSize * 2) - groundSize;                                                                                          
    }
}

void initButterflies() {
    for (int i = 0; i < numButterflies; i++) {
        butterflies[i][0] = rand() % ((int)groundSize * 2) - groundSize; // Random X position
        butterflies[i][1] = rand() % 10 + 10;                            // Random Y (height)
        butterflies[i][2] = rand() % ((int)groundSize * 2) - groundSize; // Random Z position
        butterflyGlow[i] = 0.5 + ((rand() % 50) / 100.0); // Initial glow
        butterflyGlowDirection[i] = 1;                    // Start glowing brighter
    }
}


void drawSnow() {
    glColor3f(1.0, 1.0, 1.0);                                                                                          
    glPointSize(2.0);                                                                                          
    glBegin(GL_POINTS);                                                                                          
    for (int i = 0; i < numSnowflakes; i++) {
        glVertex3f(snowflakes[i][0], snowflakes[i][1], snowflakes[i][2]);                                                                                          
    }
    glEnd();
}

void drawButterfly(float x, float y, float z, float glow) {
    glPushMatrix();
    glTranslatef(x, y, z);

    // Enable blending for glowing effect
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Draw body
    glColor4f(0.3, 0.3, 0.3, glow); // Body with glow effect
    glutSolidSphere(0.2, 10, 10);

    // Draw wings
    glColor4f(0.8, 0.1, 0.9, glow); // Glowing pinkish wings
    glBegin(GL_TRIANGLES);
    glVertex3f(-0.5, 0.0, 0.0);  // Left wing
    glVertex3f(0.0, 0.5, 0.0);
    glVertex3f(0.0, 0.0, 0.5);

    glVertex3f(0.5, 0.0, 0.0);   // Right wing
    glVertex3f(0.0, 0.5, 0.0);
    glVertex3f(0.0, 0.0, -0.5);
    glEnd();

    glDisable(GL_BLEND);
    glPopMatrix();
}

void drawRain() {
    glColor3f(0.5, 0.5, 1.0);                                                                                         
    glBegin(GL_LINES);                                                                                         
    for (int i = 0; i < numRaindrops; i++) {
        glVertex3f(raindrops[i][0], raindrops[i][1], raindrops[i][2]);                                                                                         
        glVertex3f(raindrops[i][0], raindrops[i][1] - 0.5, raindrops[i][2]);                                                                                         
    }
    glEnd();
}

void fractalBranch(float x, float y, float z, float length, float angleX, float angleY, int depth) {
    if (depth <= 0) return; // Base case: no further branching

    // Apply swaying transformation
    glPushMatrix();
    if (isSwaying) {
        glTranslatef(x, y, z);          // Move to the starting point
        glRotatef(swayAngle / 2.0, 0.0, 0.0, 1.0); // Apply reduced sway effect
        glTranslatef(-x, -y, -z);      // Undo the translation
    }

    float endX = x + length * cos(angleX * PI / 180.0) * cos(angleY * PI / 180.0);
    float endY = y + length * sin(angleX * PI / 180.0);
    float endZ = z + length * cos(angleX * PI / 180.0) * sin(angleY * PI / 180.0);

    // Shades of green based on depth
    float greenIntensity = 0.2 + 0.6 * (depth / 10.0); // Range: 0.2 to 0.8
    greenIntensity = greenIntensity > 0.8 ? 0.8 : greenIntensity; // Cap at 0.8 for balance
    glColor3f(0.1, greenIntensity, 0.1); // Green shade with minimal red/blue

    // Draw the branch
    glBegin(GL_LINES);
    glVertex3f(x, y, z);
    glVertex3f(endX, endY, endZ);
    glEnd();

    // Recursive calls for sub-branches
    float newLength = length * 0.7; // Reduce length for child branches
    fractalBranch(endX, endY, endZ, newLength, angleX + 30.0, angleY + 15.0, depth - 1);
    fractalBranch(endX, endY, endZ, newLength, angleX + 30.0, angleY - 15.0, depth - 1);
    fractalBranch(endX, endY, endZ, newLength, angleX - 30.0, angleY + 15.0, depth - 1);
    fractalBranch(endX, endY, endZ, newLength, angleX - 30.0, angleY - 15.0, depth - 1);

    glPopMatrix(); // Undo the swaying transformation
}




void drawFractalPlant(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);

    // Initial branch
    glColor3f(0.0, 1.0, 0.0); // Green base
    fractalBranch(0.0, 0.0, 0.0, 3.0, 90.0, 0.0, 6); // Root branch facing upwards

    glPopMatrix();
}

void magicMushroom(float x, float y, float z, float glow) {
    glPushMatrix();
    glTranslatef(x, y, z);

    // Enable blending for glowing effect
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Draw stem
    glColor4f(0.5, 0.5, 0.7, 1.0); // Light bluish stem
    glPushMatrix();
    glScalef(0.15, 1.0, 0.15); // Adjust proportions for a slender stem
    glutSolidSphere(1.0, 20, 20);
    glPopMatrix();

    // Draw glowing cap with shading
    glPushMatrix();
    glTranslatef(0.0, 1.0, 0.0); // Place above the stem
    glColor4f(0.2, 0.3, 1.0, glow); // Base cap color
    glutSolidSphere(0.8, 30, 30); // Base cap shape

    // Add highlights to mimic the glowing pattern
    glColor4f(0.5, 0.7, 1.0, glow * 1.5);
    glutSolidSphere(0.85, 20, 20); // Glow layer
    glPopMatrix();

    // Add halo effect around the mushroom
    glColor4f(0.2, 0.6, 1.0, glow * 0.3);
    glutSolidSphere(1.2, 20, 20); // Outer halo glow

    glDisable(GL_BLEND);
    glPopMatrix();
}


void initForest(void) {
    float x, z, dist;

                                                                                            
    for(int i=0; i<numBigTrees; i++) {                                                                                                      
        do {
            x = rand()%((int)groundSize*2)-groundSize;                                                                                          
            z = rand()%((int)groundSize*2)-groundSize;                                                                                          
            dist = sqrt(x*x + z*z);                                                                                                             
        } while( dist < 30.0);                                                                                                                  
        bigTrees[i][0] = x;                                                                                                             
        bigTrees[i][1] = z;                                                                                                             
        bigTrees[i][2] = (rand()%50)/10.0+0.2;                                                                                          
        bigTrees[i][3] = bigTrees[i][2]*20;                                                                                             
        bigTrees[i][4] =(dist*dist)/1200.0;                                                                                             
    }

                                                                                            
    for(int i=0; i<numStumps; i++) {                                                                                                        
        do {
            x = rand()%((int)groundSize*2)-groundSize;                                                                                          
            z = rand()%((int)groundSize*2)-groundSize;                                                                                          
            dist = sqrt(x*x + z*z);                                                                                                             
        } while( dist < 10.0);                                                                                                                  
        stumps[i][0] = x;                                                                                                               
        stumps[i][1] = z;                                                                                                               
        stumps[i][2] = (rand()%80)/20.0+1.0;                                                                                            
        stumps[i][3] = (rand()%80)/20.0+0.5;                                                                                            
        stumps[i][4] = (dist*dist)/1200.0;                                                                                              
    }

                                                                                            
    for (int i = 0; i < numWeeds; i++) {
        weeds[i][0] = rand() % ((int)groundSize * 2) - groundSize; // X position
        weeds[i][1] = generateHeight(weeds[i][0], weeds[i][2]);   // Terrain height
        weeds[i][2] = rand() % ((int)groundSize * 2) - groundSize; // Z position
        weeds[i][3] = (rand() % 50) / 100.0 + 0.5; // Random glow intensity
    }
}
void drawFairy(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);

                                                                                            
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0, 0.8, 0.1, fairyGlow * 0.5);                                                                                         
    glutSolidSphere(0.8, 10, 10);                                                                                         

                                                                                            
    glColor4f(1.0, 0.8, 0.1, fairyGlow);                                                                                         
    glutSolidSphere(0.5, 10, 10);                                                                                         

                                                                                            
    glColor4f(1.0, 1.0, 1.0, 0.8);                                                                                         
    glBegin(GL_TRIANGLES);
    glVertex3f(-0.5, 0.2, 0.0);
    glVertex3f(0.5, 0.2, 0.0);
    glVertex3f(0.0, 1.0, 0.0);
    glEnd();

    glPopMatrix();
    glDisable(GL_BLEND);
}

void drawBird(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);                                                                                         
    glScalef(0.8, 0.8, 0.8);                                                                                         

                                                                                            
    glColor3f(1.0, 1.0, 1.0);                                                                                         
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0, 0.0, 0.0);                                                                                          
    glVertex3f(-2.0, -0.4, 0.0);                                                                                         
    glVertex3f(-2.0, 0.4, 0.0);                                                                                          
    glEnd();

                                                                                            
    glColor3f(0.9, 0.9, 0.9);                                                                                         
    glBegin(GL_TRIANGLES);
    glVertex3f(-2.0, -0.2, 0.0);                                                                                         
    glVertex3f(-2.8, 0.0, 0.0);                                                                                          
    glVertex3f(-2.0, 0.2, 0.0);                                                                                          
    glEnd();

                                                                                            
    glColor3f(1.0, 1.0, 1.0);                                                                                         
    glPushMatrix();
    glRotatef(wingAngle, 0.0, 0.0, 1.0);                                                                                         
    glBegin(GL_POLYGON);
    glVertex3f(0.0, 0.0, 0.0);                                                                                          
    glVertex3f(1.8, 1.2, 0.0);                                                                                          
    glVertex3f(0.6, 0.3, 0.0);                                                                                          
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glRotatef(-wingAngle, 0.0, 0.0, 1.0);                                                                                         
    glBegin(GL_POLYGON);
    glVertex3f(0.0, 0.0, 0.0);                                                                                          
    glVertex3f(1.8, -1.2, 0.0);                                                                                         
    glVertex3f(0.6, -0.3, 0.0);                                                                                         
    glEnd();
    glPopMatrix();

                                                                                            
    glColor3f(1.0, 0.8, 0.0);                                                                                         
    glBegin(GL_TRIANGLES);
    glVertex3f(0.2, 0.0, 0.0);                                                                                         
    glVertex3f(0.5, 0.1, 0.0);                                                                                         
    glVertex3f(0.5, -0.1, 0.0);                                                                                         
    glEnd();

                                                                                            
    glColor3f(0.0, 0.0, 0.0);                                                                                         
    glPushMatrix();
    glTranslatef(0.1, 0.1, 0.0);                                                                                         
    glutSolidSphere(0.05, 10, 10);                                                                                         
    glPopMatrix();

    glPopMatrix();
}


                                                                                        
void updateBirdFlap() {
    wingAngle += flapDirection * 2.0;                                                                                         
    if (wingAngle > 30.0 || wingAngle < -30.0) {                                                                                         
        flapDirection = -flapDirection;
    }
}

                                                                                        
void animateBird() {
    birdX += 0.1;                                                                                         
    if (birdX > 30.0) {
        birdX = -30.0;                                                                                         
    }
    updateBirdFlap();                                                                                         
}

                                                                                        
void drawBirds() {
    drawBird(birdX, birdY, birdZ);                                                                                         
}

void initFairies(void) {
    for (int i = 0; i < numFairies; i++) {
        fairies[i][0] = rand() % ((int)groundSize * 2) - groundSize;                                                                                         
        fairies[i][1] = rand() % 10 + 5;                                                                                         
        fairies[i][2] = rand() % ((int)groundSize * 2) - groundSize;                                                                                         
        fairySpeeds[i][0] = (rand() % 10 - 5) * 0.01;                                                                                         
        fairySpeeds[i][1] = (rand() % 10 - 5) * 0.01;                                                                                         
    }
}

void magicalTreeRecursive(float startX, float startY, float startZ, float length, float angleX, float angleZ, int depth) {
    if (depth <= 0) return;                                                                                         

                                                                                            
    float endX = startX + length * cos(angleX * PI / 180.0) * cos(angleZ * PI / 180.0);
    float endY = startY + length * sin(angleX * PI / 180.0);
    float endZ = startZ + length * cos(angleX * PI / 180.0) * sin(angleZ * PI / 180.0);

                                                                                            
    glBegin(GL_LINES);
    glColor3f(1.0 - depth * 0.1, 0.2 + depth * 0.1, 1.0 - depth * 0.05);                                                                                         
    glVertex3f(startX, startY, startZ);
    glVertex3f(endX, endY, endZ);
    glEnd();

                                                                                            
    float newLength = length * 0.7;                                                                                         
    float branchAngle = 30.0;                                                                                              

    magicalTreeRecursive(endX, endY, endZ, newLength, angleX + branchAngle, angleZ, depth - 1);                                                                                         
    magicalTreeRecursive(endX, endY, endZ, newLength, angleX - branchAngle, angleZ, depth - 1);                                                                                         
    magicalTreeRecursive(endX, endY, endZ, newLength, angleX, angleZ + branchAngle, depth - 1);                                                                                         
    magicalTreeRecursive(endX, endY, endZ, newLength, angleX, angleZ - branchAngle, depth - 1);                                                                                         
}

void magicalTree(float radius, float height, float dist) {
    glPushMatrix();
    glTranslatef(0.0, 0.0, 0.0);                                                                                         
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                                                                                            
    glBegin(GL_LINES);
    glColor3f(0.6, 0.3, 0.8);                                                                                         
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, height, 0.0);
    glEnd();

                                                                                            
    magicalTreeRecursive(0.0, height, 0.0, height * 0.6, 90.0, 0.0, 6);                                                                                         
    glDisable(GL_BLEND);
    glPopMatrix();
}

void bigTree(float radius, float height, float dist) {
    int segments = 10;      
    float cx = 0, cz = 0; 
    float phi, x1, z1;

    glPushMatrix(); // Apply the swaying only once for the entire tree
    if (isSwaying) {
        glTranslatef(0.0, 0.0, 0.0);
        glRotatef(swayAngle / 2.0, 0.0, 0.0, 1.0); // Apply reduced sway effect
    }

    for (int j = 0; j < segments; j++) {
        float shade = (cos(((float)j / segments) * 2 * PI)) / 5.0;
        glColor3f(
            (0.10 + 0.52 * shade) / dist,
            (0.04 + 0.24 * shade) / dist,
            (0.01 + 0.11 * shade) / dist
        );
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        glBegin(GL_POLYGON);
        phi = 2 * PI * j / segments;
        x1 = radius * cos(phi) + cx;
        z1 = radius * sin(phi) + cz;
        glTexCoord2f((float)j / segments, 0);
        glVertex3f(x1, 0, z1);
        glTexCoord2f((float)j / segments, 1);
        glVertex3f(x1, height, z1);
        phi = 2 * PI * (j + 1) / segments;
        x1 = radius * cos(phi) + cx;
        z1 = radius * sin(phi) + cz;
        glTexCoord2f((float)(j + 1) / segments, 1);
        glVertex3f(x1, height, z1);
        glTexCoord2f((float)(j + 1) / segments, 0);
        glVertex3f(x1, 0, z1);
        glEnd();
        glDisable(GL_TEXTURE_2D);
    }

    // Tree top logic remains unchanged
    radius *= 6;                                        
    float center1[3] = {cx, height + (height / 6.0), cz};   
    float center2[3] = {cx, height, cz};                
    phi = 0;
    x1 = radius * cos(phi) + cx;
    z1 = radius * sin(phi) + cz;
    float first[3] = {x1, height, z1};                  

    float color1[3] = {0.2 / dist, 1.0 / dist, 0.9 / dist};     
    float color2[3] = {0.52 / dist, 0.24 / dist, 0.11 / dist};  

    for (int i = 0; i < segments; i++) {      
        phi = 2 * PI * (i + 1) / segments;
        x1 = radius * cos(phi) + cx;
        z1 = radius * sin(phi) + cz;
        float next[] = {x1, height, z1};
        glBegin(GL_POLYGON);            
            glColor3fv(color1);
            glVertex3fv(center1);
            glColor3fv(color2);
            glVertex3fv(first);
            glVertex3fv(next);
        glEnd();
        glBegin(GL_POLYGON);                
            glColor3fv(color1);
            glVertex3fv(center2);
            glColor3fv(color2);
            glVertex3fv(next);
            glVertex3fv(first);
        glEnd();
        first[0] = next[0];
        first[1] = next[1];
        first[2] = next[2];
    }
    glPopMatrix(); // End swaying effect
}


void pythagorasTree(float topLeftx, float topLefty, float topRightx, float topRighty, float n) {
    if (isSwaying) {
        glPushMatrix();
        glTranslatef(0.0, 0.0, 0.0);                                                                                          
        glRotatef(swayAngle, 0.0, 0.0, 1.0);                                                                                         
    }
                                                                                          
    float Q1x = topLeftx, Q1y = topLefty;
    float Q2x = topRightx, Q2y = topRighty;
    float Q3x = Q2x - (Q1y - Q2y), Q3y = Q2y + (Q1x - Q2x);
    float Q4x = Q1x - (Q1y - Q2y), Q4y = Q1y + (Q1x - Q2x);
                                                                                   
    glColor3f(0.2f * n, 0.5f * n, 1.0f - n);
                                                                                      
    glBegin(GL_POLYGON);
    glVertex3f(Q1x, Q1y, 0.5);
    glVertex3f(Q2x, Q2y, 0.5);
    glVertex3f(Q3x, Q3y, 0.5);
    glVertex3f(Q4x, Q4y, 0.5);
    glEnd();

                                                                                            
    glBegin(GL_POLYGON);
    glVertex3f(Q4x, Q4y, -0.5);
    glVertex3f(Q3x, Q3y, -0.5);
    glVertex3f(Q2x, Q2y, -0.5);
    glVertex3f(Q1x, Q1y, -0.5);
    glEnd();
                                                                                    
    if (n < 1.0f) {
        float ACx = (Q3x - Q1x) / 2;
        float ACy = (Q3y - Q1y) / 2;
        float newx = Q4x + ACx;
        float newy = Q4y + ACy;
        pythagorasTree(Q4x, Q4y, newx, newy, n + 0.1f);
        pythagorasTree(newx, newy, Q3x, Q3y, n + 0.1f);
    }

    if (isSwaying) {
        glPopMatrix();                                                                                         
    }
}
                                                                                        
void stump(float radius, float height, float dist) {
    int segments = 20;                                                                                                  
    float cx=0, cz=0;                                                                                             
    float phi, x1, z1;         
                                                                   
    for (int j = 0; j<segments; j++) {                                                                                          
        float shade = (cos(((float)j/segments)*2*PI))/5.0;
        glColor3f(
            (0.10+0.52*shade)/dist,
            (0.04+0.24*shade)/dist,
            (0.01+0.11*shade)/dist);
        glBegin(GL_POLYGON);
                                                                                                
        phi = 2 * PI * j / segments;
        x1 = radius * cos(phi) + cx;
        z1 = radius * sin(phi) + cz;
        glVertex3f(x1, 0, z1);
        glVertex3f(x1, height, z1);

                                                                                                
        phi = 2 * PI * (j+1) / segments;
        x1 = radius * cos(phi) + cx;
        z1 = radius * sin(phi) + cz;
        glVertex3f(x1, height, z1);
        glVertex3f(x1, 0, z1);
        glEnd();
    }                                                                                      
                                                                                            
    phi = 0;
    x1 = radius * cos(phi) + cx;
    z1 = radius * sin(phi) + cz;
    float first[3] = {x1, height, z1};
    float color1[3] = {0.2/dist,1.0/dist,0.9/dist};                                                                                                 
    float color2[3] = {0.52/dist, 0.24/dist, 0.11/dist};                                                                                            
    for (int i = 0; i<segments; i++) {                                                                                                      
        phi = 2 * PI * (i+1) / segments;
        x1 = radius * cos(phi) + cx;
        z1 = radius * sin(phi) + cz;
        float next[] = {x1, height, z1};                                                                                                                                                                                                  
        glBegin(GL_POLYGON);
            glColor3fv(color1);
            glVertex3f(cx, height, cz);
            glColor3fv(color2);
            glVertex3fv(first);
            glVertex3fv(next);
        glEnd();                                                                                               
        first[0] = next[0];
        first[1] = next[1];
        first[2] = next[2];
    }
}

                                                                                        
void weed(float radius, float height, float dist) {
    int segments = 4;                                                                                               
    float cx=0, cz=0;                                                                                               
    float size = 0.03;                                                                                              
    float phi, x1, z1;                                                                                          
    glColor3f(0.1/dist, 0.03/dist, 0.1/dist);
    for (int j = 0; j<segments; j++) {                                                                                          
        glBegin(GL_POLYGON);
                                                                                                
        phi = 2 * PI * j / segments;
        x1 = size * cos(phi) + cx;
        z1 = size * sin(phi) + cz;
        glVertex3f(x1, 0, z1);
        glVertex3f(x1, height, z1);

                                                                                                
        phi = 2 * PI * (j+1) / segments;
        x1 = size * cos(phi) + cx;
        z1 = size * sin(phi) + cz;
        glVertex3f(x1, height, z1);
        glVertex3f(x1, 0, z1);

        glEnd();
    }                                                                                        
    glColor3f(0.62/dist, 1.4/dist, 0.82/dist);
    glPushMatrix();
    glTranslatef(0,height+radius,0);
    glutSolidSphere(radius, 6, 6);
    glPopMatrix();
}

                                                                                        
void ground(void) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, groundTexture2);

    float step = groundSize / 20.0;                                                                                         
    glBegin(GL_QUADS);
    for (float x = -groundSize; x < groundSize; x += step) {
        for (float z = -groundSize; z < groundSize; z += step) {
            float h1 = generateHeight(x, z);
            float h2 = generateHeight(x + step, z);
            float h3 = generateHeight(x + step, z + step);
            float h4 = generateHeight(x, z + step);

            glTexCoord2f(0.0, 0.0); glVertex3f(x, h1, z);
            glTexCoord2f(1.0, 0.0); glVertex3f(x + step, h2, z);
            glTexCoord2f(1.0, 1.0); glVertex3f(x + step, h3, z + step);
            glTexCoord2f(0.0, 1.0); glVertex3f(x, h4, z + step);
        }
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
                                                                                            
    glColor3f(0, 0, 0);                                                                                         
    glLineWidth(1);                                                                                             

    for (int i = 0; i < groundSize * 2 + 1; i++) {
        glBegin(GL_LINES);
        glVertex3f(-groundSize + i, 0, groundSize);
        glVertex3f(-groundSize + i, 0, -groundSize);
        glEnd();
    }
    for (int j = 0; j < groundSize * 2 + 1; j++) {
        glBegin(GL_LINES);
        glVertex3f(-groundSize, 0, groundSize - j);
        glVertex3f(groundSize, 0, groundSize - j);
        glEnd();
    }
}
                                                                                        
void menuMode(int flag) {
    if(flag == 1) {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, screenWidth, 0, screenHeight);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glDisable(GL_DEPTH_TEST);
    }
    else {
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glEnable(GL_DEPTH_TEST);
    }
}

void projection() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (mode == 0) {
        glOrtho(-asp * dim, +asp * dim, -dim, +dim, -10 * dim, +10 * dim);
    } else if (mode == 1 || mode == 2) {
        gluPerspective(fov, asp, 0.1, 100.0 * dim);
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glRotatef(xrot+xrotChange, 1,0,0);                                                                                          
    glRotatef(yrot+yrotChange, 0,1,0);                                                                                          
    glTranslatef(-xpos,-ypos,-zpos);                                                                                            
    if (mode == 0) { // Orthogonal
    glRotatef(ph, 1, 0, 0);
    glRotatef(th, 0, 1, 0);
} else if (mode == 1) { // Perspective
    double Ex = -2 * dim * Sin(th) * Cos(ph);
    double Ey = +2 * dim * Sin(ph);
    double Ez = +2 * dim * Cos(th) * Cos(ph);
    gluLookAt(Ex, Ey, Ez, 0, 0, 0, 0, Cos(ph), 0);
}

                                                                                      
    ground();                                                                                                                   
    drawBirds();                                                                                                                                                                                   
    for(int i=0; i<numBigTrees; i++) {
        glPushMatrix();
        glTranslatef(bigTrees[i][0], 0, bigTrees[i][1]);
        bigTree(bigTrees[i][2], bigTrees[i][3], bigTrees[i][4]);
        glPopMatrix();
    }                
                                                                             
for (int i = 0; i < numStumps; i++) {
    glPushMatrix();
    glTranslatef(stumps[i][0], 0, stumps[i][1]);
    magicalTree(stumps[i][2], stumps[i][3], stumps[i][4]);
    glPopMatrix();
}                                                                                            
    for (int i = 0; i < numWeeds; i++) {
    drawFractalPlant(weeds[i][0], weeds[i][1], weeds[i][2]);
}


    for (int i = 0; i < numFairies; i++) {
    drawFairy(fairies[i][0], fairies[i][1], fairies[i][2]);
}                                                                                    
    pythagorasTree(2,0,-2,0,0.5);
    if (isRaining) {
    drawRain();                                                                                         
}
if (isSnowing) {
    drawSnow();                                                                                          
}
    if(helpMenu) {
        menuMode(1);                                                                                                                
        glColor3f(0.8,0.8,0.8);
        menuMode(0);                                                                                                                
    }

    glutSwapBuffers();
}                                                                                         
                                                                                        
void keyboard(unsigned char key, int x, int y) {
    if(key == 'a') a_state = 1;                                                                                             
    if(key == 'd') d_state = 1;                                                                                             
    if(key == 'w') w_state = 1;                                                                                             
    if(key == 's') s_state = 1;                                                                                             
    if(key == 'e') e_state = 1;                                                                                             
    if(key == 'q') q_state = 1;                                                                                             
    if(key == 32) {                                                                                                         
        if(ypos == 0.0) {                                                                                                   
            jumpRising = 1;                                                                                                 
            jumpSpeed = 1;                                                                                                  
        }
    }
    if(key == 'h') {                                                                                                        
        if(helpMenu == 1) helpMenu = 0;
        else helpMenu = 1;
    }
    if (key == 'r') {
    isRaining = !isRaining;                                                                                         
}
if (key == 'm') {
    mode = (mode + 1) % 2;
    }
if (key == 'n') {
    isSnowing = !isSnowing;                                                                                          
}
    if((int)key == 27) exit(0);                                                                                             
}

                                                                                        
void keyboardUp(unsigned char key, int x, int y) {
    if(key == 'a') a_state = 0;                                                                                             
    if(key == 'd') d_state = 0;                                                                                             
    if(key == 'w') w_state = 0;                                                                                             
    if(key == 's') s_state = 0;                                                                                             
    if(key == 'e') e_state = 0;                                                                                             
    if(key == 'q') q_state = 0;                                                                                             
}

void Fatal(const char* format , ...)
{
   va_list args;
   va_start(args,format);
   vfprintf(stderr,format,args);
   va_end(args);
   exit(1);
}

void ErrCheck(const char* where)
{
   int err = glGetError();
   if (err) fprintf(stderr,"ERROR: %s [%s]\n",gluErrorString(err),where);
}
void mouse(int butt, int state, int x,  int y) {
    if (state == GLUT_DOWN  &&  butt == GLUT_LEFT_BUTTON) {                                                                                         
        if(mousePressed == 0) {                                                                                             
            mouseStartX = x;                                                                                                
            mouseStartY = y;                                                                                                
            isSwaying = !isSwaying;                                                                                         
        }
        mousePressed = 1;                                                                                                   
    }
    else {                                                                                                                  
        mousePressed = 0;                                                                                                   
        xrot += xrotChange;                                                                                                 
        yrot += yrotChange;                                                                                                 
        xrotChange = yrotChange = 0;                                                                                        
    }
    
}
                                                                                      
void motion(int x, int y) {
    if(mousePressed) {                                                                                                                      
        xrotChange = (float)(y - mouseStartY)/3.0;                                                                                                                                                                                         
        if(xrot+xrotChange > 90.0) {                                                                                            
            xrotChange = 90.0 - xrot;
        }
        if(xrot+xrotChange < -90.0) {                                                                                           
            xrotChange = -90 - xrot;
        }
        yrotChange = (float)(x - mouseStartX)/3.0;                                                                                          
    }
}

void timer(int value) {
    float newyrot = yrot + yrotChange;                                                                                            
    if (a_state) {                                                                                         
        float yrotrad = (newyrot / 180 * PI);
        xpos -= (float)(cos(yrotrad)) * movementSpeed;
        zpos -= (float)(sin(yrotrad)) * movementSpeed;
    }
    if (d_state) {                                                                                         
        float yrotrad = (newyrot / 180 * PI);
        xpos += (float)cos(yrotrad) * movementSpeed;
        zpos += (float)sin(yrotrad) * movementSpeed;
    }
    if (w_state) {                                                                                         
        float yrotrad = (newyrot / 180 * PI);
        xpos += (float)(sin(yrotrad)) * movementSpeed;
        zpos -= (float)(cos(yrotrad)) * movementSpeed;
    }
    if (s_state) {                                                                                         
        float yrotrad = (newyrot / 180 * PI);
        xpos -= (float)(sin(yrotrad)) * movementSpeed;
        zpos += (float)(cos(yrotrad)) * movementSpeed;
    }
    if (isSwaying) {
    swayAngle += swaySpeed;
    if (swayAngle > 10.0 || swayAngle < -10.0) {                                                                                         
        swaySpeed = -swaySpeed;                                                                                         
    }
}
for (int i = 0; i < numFairies; i++) {
    fairies[i][0] += fairySpeeds[i][0];
    fairies[i][2] += fairySpeeds[i][1];                                                                                          
    if (fairies[i][0] < -groundSize || fairies[i][0] > groundSize) {
        fairySpeeds[i][0] = -fairySpeeds[i][0];
    }
    if (fairies[i][2] < -groundSize || fairies[i][2] > groundSize) {
        fairySpeeds[i][1] = -fairySpeeds[i][1];
    }

                                                                                            
    fairies[i][1] = 7.0 + sin(glutGet(GLUT_ELAPSED_TIME) * 0.002 + i) * 2.0;
}                                                                                      
fairyGlow += glowDirection;
if (fairyGlow > 0.8 || fairyGlow < 0.2) {
    glowDirection = -glowDirection;                                                                                         
}
animateBird();                                                                                         
if (isRaining) {
    for (int i = 0; i < numRaindrops; i++) {
        raindrops[i][1] -= rainSpeed;                                                                                         
        if (raindrops[i][1] < 0) {                                                                                         
            raindrops[i][1] = rand() % 20 + 10;                                                                                         
            raindrops[i][0] = rand() % ((int)groundSize * 2) - groundSize;                                                                                         
            raindrops[i][2] = rand() % ((int)groundSize * 2) - groundSize;                                                                                         
        }
    }
}
if (isSnowing) {
    for (int i = 0; i < numSnowflakes; i++) {
        snowflakes[i][1] -= snowSpeed;                                                                                          
        if (snowflakes[i][1] < 0) {                                                                                          
            snowflakes[i][1] = rand() % 20 + 10;                                                                                          
            snowflakes[i][0] = rand() % ((int)groundSize * 2) - groundSize;                                                                                          
            snowflakes[i][2] = rand() % ((int)groundSize * 2) - groundSize;                                                                                          
        }
    }
}                                                                                       
    float terrainHeight = getCurrentTerrainHeight(xpos, zpos);
    if (ypos < terrainHeight + 1.0) {                                                                                         
        ypos = terrainHeight + 1.0;
    }                                                                                       
    if (jumpRising) {
        ypos += jumpSpeed;                                                                                         
        jumpSpeed *= 0.9;                                                                                          
        if (jumpSpeed < 0.1) {
            jumpRising = 0;                                                                                         
            jumpSpeed *= -1;                                                                                         
        }
    } else {
        if (ypos > terrainHeight + 1.0) {                                                                                         
            ypos += jumpSpeed;
            jumpSpeed /= 0.9;
            if (ypos < terrainHeight + 1.0) {
                ypos = terrainHeight + 1.0;                                                                                         
            }
        }
    }

    glutPostRedisplay();                                                                                                       
    glutTimerFunc(waitTime, timer, 1);                                                                                         
}


                                                                                        
void reshape(int w, int h) {
    float left = -0.1, right = 0.1, bottom = -0.1, top = 0.1, znear = 0.1, zfar = 150;
    float ratio = (float)h / (float)w;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
        glFrustum(left, right, bottom * ratio,
            top * ratio, znear, zfar);
    else
        glFrustum(left / ratio, right / ratio,
            bottom, top, znear, zfar);

    glMatrixMode(GL_MODELVIEW);
}
                                                                                      
int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow("Mystical Forest - Varshitha Sathish");
    glEnable(GL_DEPTH_TEST);
    glClearColor(0,0,0,0);
    glutIgnoreKeyRepeat(1);                                                                                                                                                                                                                
    srand(3);
    initForest();                                                                                                       
    initFairies();
    initRain(); 
    initSnow();                                                                                          
    glutDisplayFunc(display);                                                                                                   
    glutKeyboardFunc(keyboard);                                                                                                 
    glutKeyboardUpFunc(keyboardUp);                                                                                             
    glutMouseFunc(mouse);               
    glutMotionFunc(motion);             
    glutTimerFunc(waitTime, timer, 1);  
    glutReshapeFunc(reshape);           
    woodTexture = LoadTexBMP("wood.bmp");
    branchTexture = LoadTexBMP("magical.bmp");
    groundTexture = LoadTexBMP("magical2.bmp");
    groundTexture2 = LoadTexBMP("magical3.bmp");
    mountainTexture = LoadTexBMP("mountain.bmp");
    glutMainLoop();
    return 0;
}




