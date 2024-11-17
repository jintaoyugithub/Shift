$input v2f_texCoord0

#include "../../utils/bgfx_compute.sh"
#include "fluidSim_utils.sh"

#define bufferSize 512

BUFFER_RO(curDensityField, float, 0);
BUFFER_RO(curVelocityField, vec2, 1);

void main() {
	//int index = int(gl_FragCoord.x) + int(uBufferWidth) * int(gl_FragCoord.y);
	int index = int(gl_FragCoord.x) + bufferSize * int(gl_FragCoord.y);
	gl_FragColor = vec4(0.5 * curDensityField[index], 0.2 * curDensityField[index], curDensityField[index], 1.0);
    //gl_FragColor = vec4(0.0, curVelocityField[index], 1.0);
}
