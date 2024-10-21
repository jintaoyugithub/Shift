$input v2f_texCoord0

#include "../utils/bgfx_compute.sh"

BUFFER_RO(colorBuffer, vec4, 1);

void main() {
	//gl_FragColor = vec4(0.6, v2f_texCoord0, 1.0);
	gl_FragColor = colorBuffer[gl_FragCoord.x];
}
