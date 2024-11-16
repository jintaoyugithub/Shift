#include "../../utils/bgfx_compute.sh"
#include "fluidSim_utils.sh"

BUFFER_WO(prevDensityField, float, 0);
BUFFER_WO(prevVelocityField, vec2, 1);
BUFFER_WO(curVelocityField, vec2, 2);


NUM_THREADS(32, 32, 1)
void main() {
    // init the density field and velocity field
    uint index = gl_GlobalInvocationID.x + gl_GlobalInvocationID.y * uBufferHeight;
    prevDensityField[index] = 0.0;
    prevVelocityField[index] = vec2(1.0, 1.0);
    curVelocityField[index] = vec2(0.0, 1.0);
}

