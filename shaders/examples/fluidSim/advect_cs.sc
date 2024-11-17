#include "../../utils/bgfx_compute.sh"
#include "fluidSim_utils.sh"

BUFFER_RO(prevDensityField, float, 0);
BUFFER_RW(curDensityField, float, 1);
BUFFER_RO(prevVelocityField, vec2, 2);
BUFFER_RW(curVelocityField, vec2, 3);

NUM_THREADS(32, 32, 1)
void main() {
    uint index = calIndex(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y, uBufferWidth);
    advect(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y, index, uDeltaTime);
}

void advect(uint x, uint y, uint index, float dt) {
    // previous position when passed time dt
    float xElapsed = float(x) - dt * uBufferWidth * curVelocityField[index].x;
    float yElapsed = float(y) - dt * uBufferHeight * curVelocityField[index].y;

    int i0 = int(xElapsed);
    int j0 = int(yElapsed);
    int i1 = i0 + 1;
    int j1 = j0 + 1;
    
    i0 = clamp(i0, 0, int(uBufferWidth) - 1);
    j0 = clamp(j0, 0, int(uBufferHeight) - 1);
    i1 = clamp(i1, 0, int(uBufferWidth) - 1);
    j1 = clamp(j1, 0, int(uBufferHeight) - 1);

    float s1 = xElapsed - i0;
    float s0 = 1 - s1;
    float t1 = yElapsed - j0;
    float t0 = 1 - t1;

    curDensityField[index] = 
                //uLifeTime * (
                (
                s0 * (t0 * prevDensityField[calIndex(uint(i0), uint(j0), uBufferWidth)] +
                      t1 * prevDensityField[calIndex(uint(i0), uint(j1), uBufferWidth)]) +
                s1 * (t0 * prevDensityField[calIndex(uint(i1), uint(j0), uBufferWidth)] +
                      t1 * prevDensityField[calIndex(uint(i1), uint(j1), uBufferWidth)]));
}

