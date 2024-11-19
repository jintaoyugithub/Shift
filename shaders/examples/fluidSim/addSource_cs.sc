#include "../../utils/bgfx_compute.sh"
#include "fluidSim_utils.sh"

BUFFER_RO(prevDensityField, float, 0);
BUFFER_RW(curDensityField, float, 1);
BUFFER_RO(prevVelocityField, vec2, 2);
BUFFER_RW(curVelocityField, vec2, 3);

float _radius = 5.0;

NUM_THREADS(32, 32, 1)
void main() {
    uint index = calIndex(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y, uBufferWidth);
    // The origin of gl_FragCoord is bottom left, however the origin of glfwGetCursorPos() is top left
    float dis = distance(vec2(float(gl_GlobalInvocationID.x), float(gl_GlobalInvocationID.y)), vec2(uMousePosX, uBufferHeight - uMousePosY));

    //if (dis < _radius && bool(uIsMousePressed)) {
    if (dis < _radius) {
        // add density
        addSource(index, uDeltaTime, bool(uState));
        //test
    } else {
        curDensityField[index] = prevDensityField[index];
        //curVelocityField[index] = prevVelocityField[index];
    }
}


void addSource(uint index, float dt, bool state) {
    //vec2 velocityDir = vec2(uMouseXAcce, uMouseYAcce);
    vec2 velocityDir = vec2(uMouseXAcce, -uMouseYAcce); // why need -y here

    // bug: can not use normalize here
    //curVelocityField[index] += normalize(velocityDir) + prevVelocityField[index] * dt;
    curVelocityField[index] += velocityDir + prevVelocityField[index] * dt;
    curDensityField[index] += 0.2 + prevDensityField[index] * dt;
}


