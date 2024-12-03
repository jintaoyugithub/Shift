$input v2f_posMS, v2f_texCoord0

#include "../../utils/bgfx_shader.sh"

#define CubeSize 512

/*
 * 改用cube的position来作为采样的index
 */

void main()
{ 
  vec3 pos = v2f_posMS;

  float border = 0.01;

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
    gl_FragColor= vec4(0.0, 0.0, 0.0, 1.0);
  } else {
    gl_FragColor= vec4(0.0, 1.0, 0.0, 1.0);
  }
}
