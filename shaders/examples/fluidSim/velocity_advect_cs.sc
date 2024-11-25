#include "../../utils/bgfx_compute.sh"
#include "velocity_uniforms.sh"

BUFFER_RO(_prevVelX, float, 0);
BUFFER_RO(_prevVelY, float, 1);
BUFFER_RW(_curVelX, float, 2);
BUFFER_RW(_curVelY, float, 3);
BUFFER_RO(_isFluid, float, 4);

/* before we advect the velocity, we need to interpolate the gap
 * This is how velocity move
 *
          -------
         |   |   |
      <- |   v <-|
         |       |
          ------- 
             |
             v
  
    -u1.up.velY- -u2.up.velY-
   |            |            |
   |   u1       s1    u2     |
   |            |            |
    ---u1.velY-- --u2.velY---
 *
 *
*/

void AdvectVelX(uvec2 pos, uint index) {

}

void AdvectVelY(uvec2 pos, uint index) {

}

NUM_THREADS(8, 8, 1)
void main() {
  // interpolate the velocity of the gaps first
  uvec2 pos = gl_GlobalInvocationID.xy;
  uint index = Index2D(pos.x, pos.y, uSimResX);

  if(pos.x > 0 && pos.y > 0 && pos.x < uint(uSimResX-1) && pos.y < uint(uSimResY -1)) {
    if(_isFluid[index] == 1) {
      float neighboursNum = _isFluid[RIGHT(index)] + _isFluid[LEFT(index)] + _isFluid[UP(index)] + _isFluid[DOWN(index)];
      vec2 tempVel = vec2(0.0,0.0);

      if(_isFluid[LEFT(index)] == 1)  tempVel += vec2(_curVelX[LEFT(index)], _curVelY[LEFT(index)]);
      if(_isFluid[RIGHT(index)] == 1) tempVel += vec2(_curVelX[RIGHT(index)], _curVelY[RIGHT(index)]);
      if(_isFluid[UP(index)] == 1)    tempVel += vec2(_curVelX[UP(index)], _curVelY[UP(index)]);
      if(_isFluid[DOWN(index)] == 1)  tempVel += vec2(_curVelX[DOWN(index)], _curVelY[DOWN(index)]);

      tempVel = tempVel / neighboursNum;

      _curVelX[index] = tempVel.x;
      _curVelY[index] = tempVel.y;
    }
  }
}
