#include "../../utils/bgfx_compute.sh"

BUFFER_RO(prevDensityField, float, 0);
BUFFER_WO(curDensityField, float, 1);

#define bufferSize 512

uniform vec4 mousePos;
float _radius = 5.0;

NUM_THREADS(512, 1, 1)
void main() {
    uint index = gl_GlobalInvocationID.x;
    if(bool(mousePos.z)) {
        uint x = index % bufferSize;
        uint y = index / bufferSize;

        float dis = distance(vec2(float(x), float(y)), vec2(mousePos.x, bufferSize - mousePos.y));

        if (dis < _radius) {
            curDensityField[index] = 0.5;
        }
    } else {
        curDensityField[index] = prevDensityField[index];
    }
}