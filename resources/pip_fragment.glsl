#version 330 core
out vec4 FragColor;

uniform uint screenWidth;
uniform uint screenHeight;
uniform sampler2D tex;

const float pipSize = 2.0 / 3.0;

vec2 map2f(vec2 val, vec2 in_min, vec2 in_max, vec2 out_min, vec2 out_max) {
  return ((((val - in_min) / (in_max - in_min)) * (out_max - out_min)) + out_min);
}

void main() {
  vec2 texCoord = gl_FragCoord.xy / vec2(screenWidth, screenHeight);
  vec2 newTexCoord = map2f(texCoord, vec2(pipSize, pipSize), vec2(1,1), vec2(0,0), vec2(1,1));
  FragColor = texture(tex, newTexCoord);
}
