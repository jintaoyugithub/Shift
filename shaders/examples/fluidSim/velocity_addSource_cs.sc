#include "../../utils/bgfx_compute.sh"
#include "velocity_uniforms.sh"

BUFFER_RO(_prevVelX, float, 0);
BUFFER_RO(_prevVelY, float, 1);
BUFFER_RW(_curVelX, float, 2);
BUFFER_RW(_curVelY, float, 3);
BUFFER_WO(_isFluid, float, 4);

NUM_THREADS(8, 8, 1)
void main() {
  uvec2 pos = gl_GlobalInvocationID.xy;
  uint index = Index2D(pos.x, pos.y, uSimResX);

  // calculate the distance from the current cell to the mouse position
  float dis = distance(pos, vec2(uMousePosX, uSimResY - uMousePosY));

  if(dis < uRadius) {
    //_curVelX[index] += uMouseVelX * speedDeltaTime  + _prevVelX[index];
    //_curVelY[index] += uMouseVelY * speedDeltaTime + _prevVelY[index];
    
    //_curVelX[index] += uMouseVelX * speedDeltaTime + _prevVelX[index] * uDeltaTime;
    //_curVelY[index] += uMouseVelY * speedDeltaTime + _prevVelY[index] * uDeltaTime;
    
    //_curVelX[index] += uMouseVelX * speedDeltaTime + _prevVelX[index] * uDeltaTime;
    //_curVelY[index] += uMouseVelY * speedDeltaTime + _prevVelY[index] * uDeltaTime;

    _curVelX[index] = uMouseVelX * speedDeltaTime;
    _curVelY[index] = uMouseVelY * speedDeltaTime;
    _isFluid[index] = 0.0;
  }
}
