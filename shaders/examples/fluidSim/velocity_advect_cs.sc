#include "../../utils/bgfx_compute.sh"
#include "velocity_uniforms.sh"

BUFFER_RO(_prevVelX, float, 0);
BUFFER_RO(_prevVelY, float, 1);
BUFFER_WO(_curVelX, float, 2);
BUFFER_WO(_curVelY, float, 3);
BUFFER_RO(_isFluid, float, 4);

/* before we advect the velocity, we need to interpolate the gap
 * This is how velocity move
 * Horizontal arrow only represent horizontal velocity, no vertical velocity
 * Same as the vertical arrow
 *
 *        -------
 *       |   |   |
 *    <- |   v <-|
 *       |       |
 *        ------- 
 *           |
 *           v
 *
 * So if we want to, for example, calculate the horizontal velocity for horizontal line s 
 * which only store the vertical velocity at that line
 * We need to interpolate the for horizontal velocity around it, like
 *
 *       -------- ------- 
 *      |        |       |
 *      |->4 *   |->3    |
 *      |        |       |
 *       ----s--- ------- 
 *      |        |       |
 *      |-> 1    |->2    |
 *      |        |       |
 *       -------- ------- 
 *
 *  * means the current index
 *
 *  // if you dont know why they are computed like this
 *  // check how velocity move just above 
 *  vel1 = _curVelX[D(index)];
 *  vel2 = _curVelX[D(R(index))];
 *  vel3 = _curVelX[R(index)];
 *  vel4 = _curVelX[index];
 *
 *  s = (vel1 + vel2 + vel3 + vel4) / 4
*/

float InterpolateCurVelX(uvec2 pos, uint index) {
  
  //bool hasDown = bool(_isFluid[DOWN(index)]);
  //bool hasRight = bool(_isFluid[RIGHT(index)]);

  // might have a bug here, because the origin is different!!!
  bool hasDown = pos.y > 0;
  bool hasRight = float(pos.x) < uSimResX-1;

  float velX_tl = _prevVelX[index];
  float velX_tr = hasRight ? _prevVelX[RIGHT(index)] : 0.0;
  float velX_bl = hasDown ? _prevVelX[DOWN(index)] : 0.0;
  float velX_br = (hasRight && hasDown) ? _prevVelX[DOWN(RIGHT(index))] : 0.0;

  return (velX_tl + velX_tr + velX_bl + velX_br) / 4;
}

float InterpolateCurVelY(uvec2 pos, uint index) {
  // hasLeft = pos.x > 0;
  //bool hasLeft = bool(_isFluid[LEFT(index)]);
  //bool hasUp = bool(_isFluid[UP(index)]);
  bool hasLeft = pos.x > 0;
  bool hasUp = float(pos.y) < uSimResY-1;

  float velY_br = _prevVelY[index];
  float velY_bl = hasLeft ? _prevVelY[LEFT(index)] : 0.0;
  float velY_tl = (hasLeft && hasUp) ? _prevVelY[UP(LEFT(index))] : 0.0;
  // wrong: float velY_tr = _prevVelY[UP(index)];
  float velY_tr = hasUp ? _prevVelY[UP(index)] : 0.0;

  return  (velY_br + velY_bl + velY_tl + velY_tr) / 4;
}

float InterpolatePrevVelX(vec2 pos) {
  uvec2 upos = uvec2(pos.x, pos.y);
  vec2 lambda = pos - upos;
  uint index = Index2D(upos.x, upos.y, uSimResX);

  vec2 oneMinsLambda = vec2(1.0,1.0) - lambda;
  // why we dont use isFluid?
  // Becaus the pos here is curPos - deltaTime * curVel, 
  // it may exceed the sim area
  bool hasLeft = pos.x > 0;
  bool hasRight = pos.x < uSimResX-1;
  bool hasDown = pos.y > 0;
  bool hasUp = pos.y < uSimResY-1;

  float velX_tr = hasUp ? _prevVelX[UP(index)] : 0.0;                      
  float velX_tl = (hasRight && hasUp) ? _prevVelX[RIGHT(UP(index))] : 0.0; 
  float velX_bl = hasRight? _prevVelX[(RIGHT(index))] : 0.0;               
  float velX_br = _prevVelX[index];                                        

  float top = oneMinsLambda.x * velX_tl + lambda.x * velX_tr;
  float bottom = oneMinsLambda.x * velX_bl + oneMinsLambda.x * velX_br;

  float velX = oneMinsLambda.y * bottom + lambda.y * top;
  return velX;
}


float InterpolatePrevVelY(vec2 pos) {
  uvec2 upos = uvec2(pos.x, pos.y);
  vec2 lambda = pos - upos;
  uint index = Index2D(upos.x, upos.y, uSimResX);

  vec2 oneMinsLambda = vec2(1.0,1.0) - lambda;
  bool hasLeft = pos.x > 0;
  bool hasRight = pos.x < uSimResX-1;
  bool hasDown = pos.y > 0;
  bool hasUp = pos.y < uSimResY-1;

  float velY_tr = hasUp ? _prevVelY[UP(index)] : 0.0;
  float velY_tl = (hasRight && hasUp) ? _prevVelY[RIGHT(UP(index))] : 0.0;
  float velY_bl = hasRight? _prevVelY[(RIGHT(index))] : 0.0;
  float velY_br = _prevVelY[index];

  float top = oneMinsLambda.x * velY_tr + lambda.x * velY_tl;
  float bottom = oneMinsLambda.x * velY_br + oneMinsLambda.x * velY_bl;

  float velY = oneMinsLambda.y * bottom + lambda.y * top;
  return velY;
}


// To calculate x and y, it's different
// for x, it look into the down and right
float InterpolatePrevVelXNew(vec2 pos) {
  uvec2 upos = uvec2(pos.x, pos.y);
  vec2 lambda = pos - upos;
  uint index = Index2D(upos.x, upos.y, uSimResX);

  vec2 oneMinsLambda = vec2(1.0,1.0) - lambda;
  // why we dont use isFluid?
  // Becaus the pos here is curPos - deltaTime * curVel, 
  // it may exceed the sim area
  bool hasLeft = pos.x > 0;
  bool hasRight = pos.x < uSimResX-1;
  bool hasDown = pos.y > 0;
  bool hasUp = pos.y < uSimResY-1;

  float velX_tr = hasRight ? _prevVelX[RIGHT(index)] : 0.0;
  float velX_tl = _prevVelX[index];
  float velX_bl = hasDown ? _prevVelX[DOWN(index)] : 0.0;
  float velX_br = (hasDown && hasRight) ? _prevVelX[DOWN(RIGHT(index))] : 0.0;

  float top = oneMinsLambda.x * velX_tl + lambda.x * velX_tr;
  float bottom = oneMinsLambda.x * velX_bl + lambda.x * velX_br;

  float velX = oneMinsLambda.y * bottom + lambda.y * top;
  //float velX = oneMinsLambda.y * top + lambda.y * bottom;
  
  return velX;
}


// for y, it looks into the up and left
float InterpolatePrevVelYNew(vec2 pos) {
  uvec2 upos = uvec2(pos.x, pos.y);
  vec2 lambda = pos - upos;
  uint index = Index2D(upos.x, upos.y, uSimResX);

  vec2 oneMinsLambda = vec2(1.0,1.0) - lambda;
  bool hasLeft = pos.x > 0;
  bool hasRight = pos.x < uSimResX-1;
  bool hasDown = pos.y > 0;
  bool hasUp = pos.y < uSimResY-1;

  float velY_tr = hasUp ? _prevVelY[UP(index)] : 0.0;
  float velY_tl = (hasLeft && hasUp) ? _prevVelY[UP(LEFT(index))] : 0.0;
  float velY_bl = hasLeft ? _prevVelY[LEFT(index)] : 0.0;
  float velY_br = _prevVelY[index];

  //float top = oneMinsLambda.x * velY_tr + lambda.x * velY_tl;
  //float bottom = oneMinsLambda.x * velY_br + lambda.x * velY_bl;

  float top = oneMinsLambda.x * velY_tl + lambda.x * velY_tr;
  float bottom = oneMinsLambda.x * velY_bl + lambda.x * velY_br;

  float velY = oneMinsLambda.y * bottom + lambda.y * top;
  return velY;
}

void AdvectVelX(uvec2 pos, uint index) {
  if(pos.x > 0 && pos.y > 0 && pos.x < uint(uSimResX-1) && pos.y < uint(uSimResY-1) //){
    && _isFluid[index] == 1 && _isFluid[LEFT(index)] == 1) {
    //&& _isFluid[index] == 1) {
      // calculate the current velocity
      float curVelY = InterpolateCurVelY(pos, index); 
      vec2 curVel = vec2(_prevVelX[index], curVelY-0.5f);

      //vec2 curVel = vec2(_prevVelX[index], curVelY);

      // calculate the previous position during deltatime
      vec2 prevPos = vec2(pos) - speedDeltaTime * curVel;

      //prevPos.y -= 1.0f;
      
      // calculate previous velocity X
      float prevVelX = InterpolatePrevVelXNew(prevPos);
      // and set to the current velcity X
      _curVelX[index] = prevVelX;
  } else {
      _curVelX[index] = _prevVelX[index];
  }
}

void AdvectVelY(uvec2 pos, uint index) {
  if(pos.x > 0 && pos.y > 0 && pos.x < uint(uSimResX-1) && pos.y < uint(uSimResY-1) //){
    && _isFluid[index] == 1 && _isFluid[DOWN(index)] == 1) {
    //&& _isFluid[index] == 1) {
      // same as how to advect velocity X
      float curVelX = InterpolateCurVelX(pos, index);  // maight be wrong
      
      vec2 curVel = vec2(curVelX-0.5f, _prevVelY[index]); 

      /* For debug */
      //vec2 curVel = vec2(curVelX, _prevVelY[index]); 
      //vec2 curVel = vec2(0.0f, _prevVelY[index]); normal
      //vec2 curVel = vec2(-0.5f, _prevVelY[index]); looks like (0.0, ~)
      //vec2 curVel = vec2(-1.0f, _prevVelY[index]); // looks like (-1.0f, ~)
      //vec2 curVel = vec2(0.1f, _prevVelY[index]);  // looks like (1.0f, ~)

      vec2 prevPos = vec2(pos) - speedDeltaTime * curVel;
      //prevPos.x -= 1.0f;

      float prevVelY = InterpolatePrevVelYNew(prevPos);

      _curVelY[index] = prevVelY;

      // for debug
      //uint idx = Index2D(uint(prevPos.x), uint(prevPos.y), uSimResX);
      //_curVelY[index] = _prevVelY[idx];

      // for debug
      //_curVelY[index] = 1.0f;
    } else {
      _curVelY[index] = _prevVelY[index];
    }
}

//NUM_THREADS(8, 8, 1)
NUM_THREADS(32, 32, 1)
void main() {
  // interpolate the velocity of the gaps first
  uvec2 pos = gl_GlobalInvocationID.xy;
  if(pos.x <=0 || pos.y <= 0 || pos.x >= uint(uSimResX-1) || pos.y >= uint(uSimResY-1)) return;

  uint index = Index2D(pos.x, pos.y, uSimResX);
  AdvectVelY(pos, index); // has problem
  AdvectVelX(pos, index); 
}
