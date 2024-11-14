$input v2f_texCoord0

#include "../../utils/bgfx_compute.sh"
#include "fluidSim_utils.sh"

#define bufferSize 1024

BUFFER_RO(curDensityField, float, 0);
BUFFER_RO(curVelocityField, vec2, 1);

void main() {
	//gl_FragColor = curDensityField[int(gl_FragCoord.x)];
	//gl_FragColor = vec4(0.0, v2f_texCoord0, 1.0);

	int index = int(gl_FragCoord.x) + bufferSize * int(gl_FragCoord.y);
	gl_FragColor = vec4(0.0, 0.0, curDensityField[index], 1.0);
	//gl_FragColor = vec4(uMousePosX, 0.0, 0.0, 1.0);
}
