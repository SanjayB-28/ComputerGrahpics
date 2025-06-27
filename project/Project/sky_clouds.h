#ifndef CLOUDS_H
#define CLOUDS_H

#define MAX_CLOUDS 200          


typedef struct {
    float x, y, z;               
    float size;                  
    float alpha;                 
    float speed;                 
    float variation;             
} SkyCloud;

typedef struct {
    SkyCloud clouds[MAX_CLOUDS]; 
    int cloudCount;              
    float baseHeight;            
    float windSpeed;             
    float smoothBrightness;
    float smoothAlpha;
} SkyCloudSystem;

SkyCloudSystem* skyCloudSystemCreate(float baseHeight);

void skyCloudSystemUpdate(SkyCloudSystem* cs, float deltaTime, float dayTime);

void skyCloudSystemRender(SkyCloudSystem* cs, float dayTime);

void skyCloudSystemDestroy(SkyCloudSystem* cs);

#endif