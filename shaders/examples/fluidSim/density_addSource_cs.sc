#include "../../utils/bgfx_compute.sh"
#include "fluidSim_utils.sh"

BUFFER_RO(prevDensityField, float, 0);
BUFFER_RW(curDensityField, float, 1);

float _radius = 1.0;

NUM_THREADS(32, 32, 1)
void main() {
    uint index = calIndex(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y, uBufferWidth);
    // The origin of gl_FragCoord is bottom left, however the origin of glfwGetCursorPos() is top left
    float dis = distance(vec2(float(gl_GlobalInvocationID.x), float(gl_GlobalInvocationID.y)), vec2(uMousePosX, uBufferHeight - uMousePosY));

    if (dis < _radius) {
        // add density
        addSource(index, uDeltaTime);
    } else {
        curDensityField[index] = prevDensityField[index];
    }
}

void addSource(uint index, float dt) {
    curDensityField[index] += 0.005 + prevDensityField[index] * dt;
}


