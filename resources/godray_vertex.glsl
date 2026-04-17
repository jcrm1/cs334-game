#version 330 core
layout (location = 0) in vec2 aPos;

uniform vec3 lightPos = vec3(200,100,0);
uniform mat4 view;
uniform mat4 projection;
uniform uint screenWidth;
uniform uint screenHeight;
// uniform vec3 lightPos;

out vec3 vPos;
out vec2 screenLightPos;

void main()
{
  gl_Position = vec4(aPos, 0.0, 1.0);
  vec4 clipPos = projection * view * vec4(lightPos, 1.0);
  if (clipPos.w <= 0.0) {
    screenLightPos = vec2(-1,-1);
  } else {
    vec3 ndc = clipPos.xyz / clipPos.w; // range typically [-1,1]
    vec2 ndc01 = ndc.xy * 0.5 + 0.5;
    screenLightPos = ndc01;
    // screenLightPos = ndc01 * vec2(screenWidth, screenHeight);
  }
  vPos = gl_Position.xyz;
}
