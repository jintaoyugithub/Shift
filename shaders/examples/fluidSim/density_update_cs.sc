#include "../../utils/bgfx_compute.sh"
#include "../../utils/fluidSim_utils.sh"

BUFFER_RO(prevDensityField, float, 0);
BUFFER_WO(curDensityField, float, 1);

uniform vec4 mousePos;
float _radius = 10.0;

// Declaration
void addSource();

// Implementation
void addSource() {
}

NUM_THREADS(16, 16, 1)
void main() {
    uint index = gl_GlobalInvocationID.x + bufferHeight * gl_GlobalInvocationID.y;

    // The origin of gl_FragCoord is bottom left, however the origin of glfwGetCursorPos() is top left
    float dis = distance(vec2(float(gl_GlobalInvocationID.x), float(gl_GlobalInvocationID.y)), vec2(mousePos.x, bufferHeight - mousePos.y));

    if (dis < _radius) {
        curDensityField[index] = 0.5;
        //addSource();
    } else {
        curDensityField[index] = prevDensityField[index];
    }
}