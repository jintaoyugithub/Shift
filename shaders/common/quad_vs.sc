$input posMS, texCoord0
$output v2f_texCoord0

#include "../utils/bgfx_shader.sh"

void main() {
	gl_Position = vec4(posMS, 0.0, 1.0);
	v2f_texCoord0 = texCoord0;
}
