#include "../../utils/bgfx_compute.sh"
#include "velocity_uniforms.sh"

BUFFER_RW(_curVelX, float, 0);
BUFFER_RW(_curVelY, float, 1);
BUFFER_RO(_isFluid, float, 2);
BUFFER_WO(_divergence, float, 3);

NUM_THREADS(8, 8, 1)
void main() {
  uvec2 pos = gl_GlobalInvocationID.xy;

  // offset the pos so that we can stagger the divergence
  pos.x = (pos.x << 1) + uint(uOffsetX);
  pos.y = (pos.y << 1) + uint(uOffsetY);

  // if out of the boundray
  if(pos.x <= 0 || pos.y <= 0 || pos.x >= uint(uSimResX-1) || pos.y >= uint(uSimResY-1)) return;

  uint index = Index2D(pos.x, pos.y, uSimResX);

  if(_isFluid[index] == 0 || _isFluid[index] == -1.0) return;

  float divergence = 0;
  // we assume that velocity x enter from right side of the current cell and exit from its left
  // velocity y enter from up and left from down, like
  // 这个方向的选择是不是要看原点的位置和velx，vely的正方向？
  /* 
   *     -------
   *    |   |   |
   * <- |   v <-|
   *    |       |
   *     ------- 
   *        |
   *        v
   *
   * or
   *
   *        ^
   *        |
   *     -------
   *    |       |
   *    |->     |->
   *    |   ^   |
   *    |   |   |
   *     ------- 
   *
   */
  float neighboursNum = _isFluid[RIGHT(index)] + _isFluid[LEFT(index)] + _isFluid[UP(index)] + _isFluid[DOWN(index)];
  if(neighboursNum == 0) return;

  // if divergence > 0 means positive divergence, we need more velocity out
  // if divergence <0 means negative divergence, we need more velocity in
  // maybe right: divergence = _curVelX[RIGHT(index)] + _curVelY[UP(index)] - _curVelX[index] - _curVelY[index];
  // wrong: divergence = _curVelX[index] + _curVelY[index] - _curVelX[RIGHT(index)] - _curVelY[UP(index)];
  divergence = _curVelX[RIGHT(index)] - _curVelX[index] + _curVelY[UP(index)] - _curVelY[index];
  //divergence = -_curVelX[index] + _curVelX[RIGHT(index)] - _curVelY[index] + _curVelY[UP(index)];
  
  /* Debug of the correction value */
  // multiple 1.95 is call overrelaxtion
  float correction = 1.5 * divergence / neighboursNum;
  //float correction = 0.1 * divergence / neighboursNum;
  //float correction = divergence / neighboursNum;

  // divergence > 0, out not enough, so more out
  // divergence < 0, out too much, so less out
  if(_isFluid[LEFT(index)] == 1) _curVelX[index] += correction;
  if(_isFluid[DOWN(index)] == 1) _curVelY[index] += correction;

  // divergence > 0, enter too much, so less enter
  // divergence < 0, enter not enough, so more enter
  if(_isFluid[RIGHT(index)] == 1) _curVelX[RIGHT(index)] -= correction;
  if(_isFluid[UP(index)] == 1) _curVelY[UP(index)] -= correction;

  // after correction, compute the div again
  float newDiv = _curVelX[RIGHT(index)] - _curVelX[index] + _curVelY[UP(index)] - _curVelY[index]; 

  // record the divergence
  _divergence[index] = newDiv;
}
