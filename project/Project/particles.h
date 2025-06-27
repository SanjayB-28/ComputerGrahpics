#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void particleSystemInit(float terrainScale, float terrainHeight);
void particleSystemUpdate(float dt);
void particleSystemRender();
void particleSystemCleanup();
void particleSystemSetEnabled(int enabled);
void particleSystemUploadHeightmap(float* elevationData);

#ifdef __cplusplus
}
#endif 