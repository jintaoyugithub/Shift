$input v2f_texCoord0

#include "../../utils/bgfx_compute.sh"
#include "velocity_uniforms.sh"

#define bufferSize 512

BUFFER_RO(_isFluid, float, 0);
BUFFER_RO(_curVelX, float, 1);
BUFFER_RO(_curVelY, float, 2);

void main() {
	int index = int(gl_FragCoord.x) + bufferSize * int(gl_FragCoord.y);

    if(_isFluid[index] == 0) {
      // draw the boundary
      gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
    } else {
      // draw the velocity
      //vec4 colorX = _curVelX[index] > 0 ? 
      //                       vec4(_curVelX[index], 0.0, 0.0, 1.0) : 
      //                       vec4(0.0, _curVelX[index]*0.5+0.5, _curVelX[index]*0.5+0.5, 1.0);
      //                       //vec4(0.0, _curVelX[index]*0.5+0.5, 0.0, 1.0);

      //vec4 colorY = _curVelY[index] > 0 ? 
      //                       vec4(0.0, _curVelY[index], 0.0, 1.0) :
      //                       vec4(_curVelY[index]*0.5+0.5, 0.0, _curVelY[index]*0.5+0.5, 1.0);
                             //vec4(0.0, 0.0, _curVelY[index]*0.5+0.5, 1.0);

      //gl_FragColor = mix(colorX, colorY, 0.5);
      //gl_FragColor = colorX + colorY;

      // Debug
      //float colorX = _curVelX[index] == 0 ? 0 : _curVelX[index] * 0.5 + 0.5;
      //float colorY = _curVelY[index] == 0 ? 0 : _curVelY[index] * 0.5 + 0.5;

      float colorX = _curVelX[index] == 0 ? 0 : _curVelX[index];
      float colorY = _curVelY[index] == 0 ? 0 : _curVelY[index];
      gl_FragColor = vec4(0.0, colorX, colorY, 1.0);
      
      //gl_FragColor = vec4(1.0,0.0,0.0,1.0);
    }
}
