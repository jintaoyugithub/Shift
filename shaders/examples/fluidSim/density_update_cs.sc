#include "../../utils/bgfx_compute.sh"
#include "fluidSim_utils.sh"

BUFFER_RO(prevDensityField, float, 0);
BUFFER_WO(curDensityField, float, 1);

float _radius = 5.0;

/**
* @ Summary
* @ Parameter
* @ Parameter
*/
void addSource(uint index, float dt);

void diffuse(uint index, float diff, float dt);

void advect(uint index, float dt);

NUM_THREADS(16, 16, 1)
void main() {
    uint index = gl_GlobalInvocationID.x + uBufferHeight * gl_GlobalInvocationID.y;
    // The origin of gl_FragCoord is bottom left, however the origin of glfwGetCursorPos() is top left
    float dis = distance(vec2(float(gl_GlobalInvocationID.x), float(gl_GlobalInvocationID.y)), vec2(uMousePosX, uBufferHeight - uMousePosY));

    if (dis < _radius) {
        addSource(index, uDeltaTime);
    } else {
        curDensityField[index] = prevDensityField[index];
    }
}

void addSource(uint index, float dt) {
    curDensityField[index] = prevDensityField[index] * 100 * dt;
}