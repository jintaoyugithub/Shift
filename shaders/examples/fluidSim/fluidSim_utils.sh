
uniform vec4 uParams[3];

#define uMousePosX              uParams[0].x
#define uMousePosY              uParams[0].y
#define uMouseXAcce             uParams[0].z
#define uMouseYAcce             uParams[0].w

#define uState                  uParams[1].x
//#define uBufferWidth            floatBitsToUint(uParams[1].y)
#define uBufferWidth            uParams[1].y
#define uBufferHeight           uParams[1].z
#define uBufferResolution       uParams[1].w

#define uDeltaTime              uParams[2].x
#define uDiff                   uParams[2].y
#define uVisc                   uParams[2].z
#define uIsMousePressed         uParams[2].w

uint calIndex(uint x, uint y, float width);

uint calIndex(uint x, uint y, float width) {
    return x + int(width) * y;
}


void addSource(uint index, float dt, bool state);

void diffuse(uint x, uint y, float dt, bool state);

void advect(uint x, uint y, uint index, float dt, bool state);

void project(uint x, uint y);

