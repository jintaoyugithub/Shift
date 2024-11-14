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

