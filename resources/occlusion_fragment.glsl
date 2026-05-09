#version 330 core
out vec4 FragColor;

in vec3 vPos;
in vec3 vColor;

uniform vec3 cameraPos;
uniform vec4 clearColor;
uniform float fogStart;
uniform float fogLength;
uniform bool enableFog;

void main() {
  float dist = distance(vPos, cameraPos);
  if (enableFog && dist > fogStart) {
    float s = clamp((dist - fogStart) / fogLength, 0.0, 1.0);
    FragColor = vec4(s, 0, gl_FragCoord.z, 1.0); // fog level, 0, depth, 1
  } else {
    FragColor = vec4(0.0, 0.0, gl_FragCoord.z, 0.0); // black // 0, 0, depth, 0
  }
}
