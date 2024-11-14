#include "../../utils/bgfx_compute.sh"

BUFFER_RO(prevDensityField, float, 0);
BUFFER_WO(curDensityField, float, 1);

uniform vec4 mousePos;
float _radius = 100000.0;

NUM_THREADS(16, 1, 1)
void main() {
    //float dis = distance(vec2(1.0, 1.0), vec2(1.0, 1.0));
    uint index = gl_GlobalInvocationID.x;
    uint x = index % 256;
    uint y = index / 256;

    float dis = distance(vec2(float(x), float(y)), mousePos.xy);

    if (dis < _radius) {
        curDensityField[index] = 0.001 * mousePos.x;
    }
}