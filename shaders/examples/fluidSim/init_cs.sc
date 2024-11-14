#include "../../utils/bgfx_compute.sh"

BUFFER_WO(prevDensityField, float, 0);
BUFFER_WO(prevVelocityField, vec2, 1);

float random(float seed) {
    return fract(sin(seed) * 43758.5453);
}

vec2 randomVec2(float seed) {
    float x = random(seed) * 2.0 - 1.0;  
    float y = random(seed + 1.0);        
    return vec2(x, y);
}

NUM_THREADS(128, 1, 1)
void main() {
    // init the density field and velocity field
    prevDensityField[gl_GlobalInvocationID.x] = 0.1;
    prevVelocityField[gl_GlobalInvocationID.x] = randomVec2(10);
}

