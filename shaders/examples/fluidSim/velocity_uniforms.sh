uniform vec4 uParams[3];

#define uMousePosX              uParams[0].x
#define uMousePosY              uParams[0].y
#define uMouseVelX              uParams[0].z
#define uMouseVelY              uParams[0].w

#define uRadius                 uParams[1].x
#define uSimResX                uParams[1].y
#define uSimResY                uParams[1].z
#define uSimResZ                uParams[1].w

#define uPersist                uParams[2].x
#define uDeltaTime              uParams[2].y
#define uSpeed                  uParams[2].z

#define LEFT(idx) ((idx) - 1)
#define RIGHT(idx) ((idx) + 1)
#define UP(idx) ((idx) + uSimResX)
#define DOWN(idx) ((idx) - uSimResX)

uint Index2D(uint x, uint y, float simResX) {
    return x + uint(simResX) * y;
}

uint Index3D(uint x, uint y, uint z, float simResX, float simResY) {
    return 0;
}

