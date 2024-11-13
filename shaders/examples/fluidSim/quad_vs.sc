$input a_position, a_texcoord0
$output v2f_texCoord0

#include "../../utils/bgfx_shader.sh"

void main() {
	gl_Position = vec4(a_position, 1.0);
	v2f_texCoord0 = a_texcoord0;
}
