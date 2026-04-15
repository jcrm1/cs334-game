#version 330 core
out vec4 FragColor;

in vec3 vPos;
in vec3 vColor;

uniform vec3 cameraPos;
uniform vec4 clearColor;

void main() {
  FragColor = vec4(0.0, 0.0, 0.0, 0.0); // black
}
