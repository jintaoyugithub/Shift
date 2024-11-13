$input v2f_texCoord0

#include "../../utils/bgfx_compute.sh"

uniform vec2 bufferSize;

BUFFER_RO(curDensityField, vec4, 0);
BUFFER_RO(curVelocityField, vec4, 1);

void main() {
	//gl_FragColor = curDensityField[int(gl_FragCoord.x)];
	gl_FragColor = vec4(0.0, v2f_texCoord0, 1.0);
}
