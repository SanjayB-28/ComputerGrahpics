#ifndef SKY_H
#define SKY_H

typedef struct {
    float position[3];           
    float size;                  
    float brightness;            
    float color[4];              
} SkyObject;

typedef struct {
    SkyObject sun;               
    SkyObject moon;              
} SkySystem;

void skySystemInit(SkySystem* sky);

void skySystemUpdate(SkySystem* sky, float dayTime);

void skySystemRenderSunAndMoon(SkySystem* sky, float dayTime);

void skySystemUpdateLighting(SkySystem* sky);

void skySystemDestroy(SkySystem* sky);

#endif