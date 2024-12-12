uniform vec4 uParams[5];

#define uMousePosX              uParams[0].x
#define uMousePosY              uParams[0].y
#define uInterPosZ              uParams[0].z
#define uMouseVelX              uParams[0].w

#define uMouseVelY              uParams[1].x
#define uInterVelZ              uParams[1].y
#define uRadius                 uParams[1].z
#define uSimResX                uParams[1].w

#define uSimResY                uParams[2].x
#define uSimResZ                uParams[2].y
#define uDeltaTime              uParams[2].z
#define uSpeed                  uParams[2].w

#define uOffsetX                uParams[3].x
#define uOffsetY                uParams[3].y
#define uDivOffsetZ             uParams[3].z

#define bufferSize 1024

#define speedDeltaTime (uDeltaTime * uSpeed)

uint LEFT(uint idx) {
    return idx - 1u;
}

uint RIGHT(uint idx) {
    return idx + 1u;
}

uint UP(uint idx) {
    return idx + uint(uSimResX);
}

uint DOWN(uint idx) {
    return idx - uint(uSimResX);
}

uint Index2D(uint x, uint y, float simResX) {
    return x + uint(simResX) * y;
}

uint Index3D(uint x, uint y, uint z, float simResX, float simResY) {
    return 0u;
}
