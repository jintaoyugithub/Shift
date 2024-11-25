$input v2f_texCoord0

#include "../../utils/bgfx_compute.sh"
#include "velocity_uniforms.sh"

#define bufferSize 512

BUFFER_RO(_isFluid, float, 0);
BUFFER_RO(_curVelX, float, 1);
BUFFER_RO(_curVelY, float, 2);

vec3 hsv2rgb(vec3 hsv) {
    float H = hsv.x * 6.0; // 色相范围扩展到 [0, 6]
    float S = hsv.y;
    float V = hsv.z;

    float C = V * S; // Chroma
    float X = C * (1.0 - abs(mod(H, 2.0) - 1.0));
    float m = V - C;

    vec3 rgb;
    if (H < 1.0) {
        rgb = vec3(C, X, 0.0);
    } else if (H < 2.0) {
        rgb = vec3(X, C, 0.0);
    } else if (H < 3.0) {
        rgb = vec3(0.0, C, X);
    } else if (H < 4.0) {
        rgb = vec3(0.0, X, C);
    } else if (H < 5.0) {
        rgb = vec3(X, 0.0, C);
    } else {
        rgb = vec3(C, 0.0, X);
    }

    return (rgb + vec3(m, m, m)); // 添加亮度偏移
}

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

      //float colorX = _curVelX[index] <= 0 ? 0 : _curVelX[index];
      //float colorY = _curVelY[index] <= 0 ? 0 : _curVelY[index];

      //float colorX = _curVelX[index] <= 0 ? -_curVelX[index] : _curVelX[index];
      //float colorY = _curVelY[index] <= 0 ? -_curVelY[index] : _curVelY[index];
      //gl_FragColor = vec4(0.0, colorX, colorY, 1.0);

      if (abs(_curVelX[index]) < 1e-5 && abs(_curVelY[index]) < 1e-5) {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0); 
      } else {
        float angle = atan2(float(_curVelY[index]), float(_curVelX[index]));
        float hue = angle / (2.0 * 3.14159) + 0.5;
        vec3 color = hsv2rgb(vec3(hue, 1.0, 1.0));
        gl_FragColor = vec4(color, 1.0);
}
      
      //gl_FragColor = vec4(1.0,0.0,0.0,1.0);
    }
}
