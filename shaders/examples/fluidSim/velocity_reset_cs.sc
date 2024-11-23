#include "../../utils/bgfx_compute.sh"
#include "velocity_uniforms.sh"

BUFFER_WO(_prevVelX, float, 0);
BUFFER_WO(_prevVelY, float, 1);
BUFFER_WO(_curVelX, float, 2);
BUFFER_WO(_curVelY, float, 3);
BUFFER_WO(_isFluid, float, 4);

NUM_THREADS(8, 8, 1)
void main() {
    // init the density field and velocity field
    vec2 pos = gl_GlobalInvocationID.xy;
    uint index = Index2D(uint(pos.x), uint(pos.y), 512.0);

    _prevVelX[index] = 0.0;
    _prevVelY[index] = 0.0;
    _curVelX[index] = 0.0;
    _curVelY[index] = 0.0;

    // determine if the current grid is inside the sim area
    if(pos.x > 100 && pos.y > 100 && pos.x < uSimResX - 100 && pos.y < uSimResY - 100) {
        _isFluid[index] = 1.0;
        return;
    }

    _isFluid[index] = 0.0;
}
