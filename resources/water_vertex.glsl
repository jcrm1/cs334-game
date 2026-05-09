#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 vColor;
out vec3 vPos;
out float vHeight;
// out vec3 vNormal;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalMatrix;
// uniform float fogStart;
// uniform float fogLength;
// uniform vec3 cameraPos;

void main()
{
   gl_Position = projection * view * model * vec4(aPos, 1.0);
   vPos = vec3(model * vec4(aPos, 1.0));
   vHeight = aPos.y;
   vColor = vec3(0.0, 0.3, 0.5);
   // vNormal = vec3(normalMatrix * vec4(aNormal, 0.0));
   // vColor = vec3(1.0, 0.0, 0.0);
}
