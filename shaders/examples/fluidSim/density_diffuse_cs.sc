#include "../../utils/bgfx_compute.sh"
#include "fluidSim_utils.sh"

BUFFER_RO(prevDensityField, float, 0);
BUFFER_RW(curDensityField, float, 1);

NUM_THREADS(32, 32, 1)
void main() {
    diffuse(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y, uDiff, uDeltaTime);
}

void diffuse(uint x, uint y, float diff, float dt) {
    float diffRate = diff * dt * uBufferWidth * uBufferHeight;

    uint curIndex = calIndex(x, y, uBufferWidth);
    uint prevXIndex = calIndex(x-1, y, uBufferWidth);
    uint prevYIndex = calIndex(x, y-1, uBufferWidth);
    uint nextXIndex = calIndex(x+1, y, uBufferWidth);
    uint nextYIndex = calIndex(x, y+1, uBufferWidth);

    int itr = 20;
    for(int i=0; i<itr; i++) {
        curDensityField[curIndex] = 
            prevDensityField[curIndex] +
            diffRate *
            (curDensityField[prevXIndex] +
             curDensityField[nextXIndex] +
             curDensityField[prevYIndex] +
             curDensityField[nextYIndex]) /
            (1 + 4 * diffRate);
    }
}
