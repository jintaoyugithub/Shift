uniform vec4 uParams[3];

#define uMousePosX              uParams[0].x
#define uMousePosY              uParams[0].y
#define uMouseIsPressed         uParams[0].z

#define uBufferWidth            floatBitsToUint(uParams[0].w)
#define uBufferHeight           floatBitsToUint(uParams[1].x)
#define uBufferResolution       floatBitsToUint(uParams[1].y)

#define uDeltaTime              uParams[1].z
#define uDiff                   uParams[1].w
#define uVisc                   uParams[2].x

uint calIndex(uint x, uint y, uint width);

uint calIndex(uint x, uint y, uint width) {
    return x + width * y;
}

/**
* @ Summary
* @ Parameter
* @ Parameter
*/
void addSource(uint index, float dt);

/**
* @ Summary
* @ Parameter
* @ Parameter
*/
void diffuse(uint x, uint y, float diff, float dt);

/**
* @ Summary
* @ Parameter
* @ Parameter
*/
void advect(uint x, uint y, uint index, float dt);
