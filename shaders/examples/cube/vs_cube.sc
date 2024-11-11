$input a_position, a_color0
$output v2f_color0

#include "../../utils/bgfx_shader.sh"

void main()
{
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0) );
	v2f_color0 = a_color0;
}