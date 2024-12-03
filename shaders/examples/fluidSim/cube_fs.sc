$input v2f_posMS, v2f_posWS, v2f_texCoord0

#include "../../utils/bgfx_compute.sh"
#include "velocity_uniforms.sh"

BUFFER_RO(_isFluid, float, 0);

/*
 * 改用cube的position来作为采样的index
 */

vec4 DebugDraw(vec3 pos, float border) {
  vec4 color;

  // check top and down
  bool td = (pos.y > 0.9 || pos.y < -0.9) && 
             pos.x > (-1.0 + border) && pos.x < (1.0 - border) && 
             pos.z > (-1.0 + border) && pos.z < (1.0 - border);
  bool lf = (pos.x > 0.9 || pos.x < -0.9) &&
             pos.y > (-1.0 + border) && pos.y < (1.0 - border) && 
             pos.z > (-1.0 + border) && pos.z < (1.0 - border);
  bool fb = (pos.z > 0.9 || pos.z < -0.9) &&
             pos.x > (-1.0 + border) && pos.x < (1.0 - border) && 
             pos.y > (-1.0 + border) && pos.y < (1.0 - border);


  if(td || lf || fb)
  {
    discard;
  } 
  else 
  {
    color = vec4(0.0, 1.0, 0.0, 1.0);
  }

  return color;
}

void main()
{ 
  /* Debug Draw */
  //gl_FragColor = DebugDraw(v2f_posMS, 0.01);

  float dis = distance(v2f_posWS, vec3(uMousePosX, uMousePosY, uInterPosZ));

  if (dis < uRadius) {
    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
  } else {
    gl_FragColor = vec4(0.0, 0.0, 0.0, 0.5);
  }

}
