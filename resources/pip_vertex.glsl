#version 330 core
layout (location = 0) in vec2 aPos;

out vec2 texCoord;
out vec2 screenLightPos;

void main()
{
  gl_Position = vec4(aPos, 0.0, 1.0);
  texCoord = aPos;
}
