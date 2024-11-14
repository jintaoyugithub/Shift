#include "../../utils/bgfx_compute.sh"

BUFFER_WO(prevDensityField, float, 0);
BUFFER_WO(prevVelocityField, vec2, 1);

#define bufferSize 256

NUM_THREADS(16, 16, 1)
void main() {
    // init the density field and velocity field
    prevDensityField[gl_GlobalInvocationID.x + gl_GlobalInvocationID.y * bufferSize] = 0.1;
    prevVelocityField[gl_GlobalInvocationID.x] = vec2(1.0, 1.0);
}

