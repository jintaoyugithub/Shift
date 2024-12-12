$input v2f_texCoord0

#include "../../utils/bgfx_compute.sh"
#include "velocity_uniforms.sh"


BUFFER_RO(_divergence, float, 0);
BUFFER_RO(_isFluid, float, 1);

vec4 DebugDispActive(float isFluid) {
  vec4 color;

  if(isFluid == 0) {
    color = vec4(1.0, 1.0, 1.0, 1.0);
  } else if (isFluid == -1) {
    color = vec4(0.0, 1.0, 0.0, 1.0);
  } else {
    color = vec4(0.2, 0.2, 0.2, 1.0);
  }

  return color;
}

vec4 DebugDispDiv(float div, float isFluid) {
  vec4 color = vec4(0.5, 0.1, 0.7, 1.0);

  if(isFluid == 1) {
    // too much inflow
    if(div < 0) {
      color = vec4(0.0, 0.0, -div*100, 1.0);
    // too much outflow
    } else if(div > 0) {
      color = vec4(div*100, 0.0, 0.0, 1.0);
    } else {
      color = vec4(0.0, 0.0, 0.0, 1.0);
    }
  } else if(isFluid == -1) {
      color = vec4(0.0, 1.0, 0.0, 1.0);
  } else {
      color = vec4(0.9, 0.9, 0.9, 1.0);
      //color = vec4(0.0, 0.0, 0.0, 1.0);
  }

  //color = vec4(div, 0.0, 0.0, 1.0);

  return color;
}

void main() {
	int index = int(gl_FragCoord.x) + bufferSize * int(gl_FragCoord.y);

    float isFluid = _isFluid[index];

    vec4 divColor = DebugDispDiv(_divergence[index], isFluid);
    vec4 actColor = DebugDispActive(isFluid);

    gl_FragColor = divColor;
    //gl_FragColor = actColor;
}
