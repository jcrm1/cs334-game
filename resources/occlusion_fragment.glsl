#version 330 core
out vec4 FragColor;

in vec3 vPos;
in vec3 vColor;

uniform vec3 cameraPos;
uniform vec4 clearColor;
uniform float fogStart;
uniform float fogLength;

void main() {
  float dist = distance(vPos, cameraPos);
  if (dist > fogStart) {
    float s = min((dist - fogStart) / fogLength, 1.0);
    FragColor = vec4(s, s, s, 1.0);
  } else {
    FragColor = vec4(0.0, 0.0, 0.0, 0.0); // black
  }
}
