#include "../../utils/bgfx_compute.sh"
#include "velocity_uniforms.sh"

BUFFER_RW(_curVelX, float, 0);
BUFFER_RW(_curVelY, float, 1);
BUFFER_RO(_isFluid, float, 2);
BUFFER_WO(_divergence, float, 3);

//NUM_THREADS(8, 8, 1)
NUM_THREADS(32, 32, 1)
void main() {
  uvec2 pos = gl_GlobalInvocationID.xy;

  // offset the pos so that we can stagger the divergence
  pos.x = (pos.x << 1) + uint(uOffsetX);
  pos.y = (pos.y << 1) + uint(uOffsetY);

  // if out of the boundray
  if(pos.x <= 0 || pos.y <= 0 || pos.x >= uint(uSimResX-1) || pos.y >= uint(uSimResY-1)) return;

  uint index = Index2D(pos.x, pos.y, uSimResX);

  //if(_isFluid[index] == 0 || _isFluid[index] == -1.0) return;
  if(_isFluid[index] == -1.0) return;


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

  // I have three different value in _isFuild, 0, -1, 1
  // this line of code may cause neighboursNum = 0 because of the -1
  // which caused an incorrect premature return
  int rn = _isFluid[RIGHT(index)] < 0.0 ? 0 : int(_isFluid[RIGHT(index)]);
  int ln = _isFluid[LEFT(index)] < 0.0 ? 0  : int(_isFluid[LEFT(index)]);
  int un = _isFluid[UP(index)] < 0.0 ? 0    : int(_isFluid[UP(index)]);
  int dn = _isFluid[DOWN(index)] < 0.0 ? 0  : int(_isFluid[DOWN(index)]);

  //float neighboursNum = _isFluid[RIGHT(index)] + _isFluid[LEFT(index)] + _isFluid[UP(index)] + _isFluid[DOWN(index)];
  float neighboursNum = float(rn + ln + un + dn);
  
  if(neighboursNum == 0) return;

  // if divergence > 0 means positive divergence, we need more velocity out
  // if divergence <0 means negative divergence, we need more velocity in
  // maybe right: divergence = _curVelX[RIGHT(index)] + _curVelY[UP(index)] - _curVelX[index] - _curVelY[index];
  // wrong: divergence = _curVelX[index] + _curVelY[index] - _curVelX[RIGHT(index)] - _curVelY[UP(index)];
  float divergence = _curVelX[RIGHT(index)] - _curVelX[index] + _curVelY[UP(index)] - _curVelY[index];

  //divergence = -_curVelX[index] + _curVelX[RIGHT(index)] - _curVelY[index] + _curVelY[UP(index)];
  
  /* Debug of the correction value */
  // multiple 1.95 is call overrelaxtion
  //float correction = 1.9 * divergence / neighboursNum;
  float correction = 1.2 * divergence / neighboursNum;
  //float correction = 0.4 * divergence / neighboursNum;
  //float correction = divergence / neighboursNum;

  // divergence > 0, too much outflow, less out, more in
  // divergence < 0, too much inflow, less in, more out
  if(_isFluid[LEFT(index)] == 1)  _curVelX[index]        += correction;
  if(_isFluid[DOWN(index)] == 1)  _curVelY[index]        += correction;
  if(_isFluid[RIGHT(index)] == 1) _curVelX[RIGHT(index)] -= correction;
  if(_isFluid[UP(index)] == 1)    _curVelY[UP(index)]    -= correction;

  // after correction, compute the div again
  float newDiv = _curVelX[RIGHT(index)] - _curVelX[index] + _curVelY[UP(index)] - _curVelY[index]; 

  // record the divergence
  //_divergence[index] = correction;
  _divergence[index] = newDiv;
}
