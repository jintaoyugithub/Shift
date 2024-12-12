$input v2f_texCoord0

#include "../../utils/bgfx_compute.sh"
#include "velocity_uniforms.sh"


BUFFER_RO(_isFluid, float, 0);
BUFFER_RO(_curVelX, float, 1);
BUFFER_RO(_curVelY, float, 2);

void main() {
	int index = int(gl_FragCoord.x) + bufferSize * int(gl_FragCoord.y);

    float velX = _curVelX[index] * 0.5 + 0.5;
    float velY = _curVelY[index] * 0.5 + 0.5;

    if(_isFluid[index] == 1) {
      gl_FragColor = vec4(velX, velY, 0.0, 1.0);
    } else {
      gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
