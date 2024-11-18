#include "../../utils/bgfx_compute.sh"
#include "fluidSim_utils.sh"

BUFFER_RO(prevDensityField, float, 0);
BUFFER_RW(curDensityField, float, 1);
BUFFER_RW(prevVelocityField, vec2, 2);
BUFFER_RW(curVelocityField, vec2, 3);

NUM_THREADS(32, 32, 1)
void main() {
    project(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y);
}

void project(uint x, uint y) {
    float h = 1 / uBufferWidth;

    uint curIndex = calIndex(x, y, uBufferWidth);
    uint prevXIndex = calIndex(x-1, y, uBufferWidth);
    uint prevYIndex = calIndex(x, y-1, uBufferWidth);
    uint nextXIndex = calIndex(x+1, y, uBufferWidth);
    uint nextYIndex = calIndex(x, y+1, uBufferWidth);

    prevVelocityField[curIndex] = vec2(0.0, 
        -0.5 * h * (
        curVelocityField[nextXIndex].x - curVelocityField[prevXIndex].x +
        curVelocityField[nextYIndex].y - curVelocityField[prevYIndex].y));

    int itr = 20;

    for(int i=0; i<itr; i++) {
        prevVelocityField[curIndex] = vec2(
          (prevVelocityField[curIndex].y +
          prevVelocityField[prevXIndex].x +
          prevVelocityField[prevYIndex].x +
          prevVelocityField[nextXIndex].x +
          prevVelocityField[nextYIndex].x) / 
          4,
          prevVelocityField[curIndex].y
        );
    }

    curVelocityField[curIndex] -= 0.5 * vec2(
        (prevVelocityField[nextXIndex].x - prevVelocityField[prevXIndex].x) / h,
        (prevVelocityField[nextYIndex].x - prevVelocityField[prevYIndex].x) / h
    );
}
