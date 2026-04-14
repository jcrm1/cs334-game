#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 vColor;
out vec3 vPos;
// out vec3 vNormal;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalMatrix;

void main()
{
   gl_Position = projection * view * model * vec4(aPos, 1.0);
   vPos = vec3(model * vec4(aPos, 1.0));
   // vNormal = vec3(normalMatrix * vec4(aNormal, 0.0));
   vColor = vec3(aPos.y / 8.0, aPos.y / 8.0, aPos.y / 8.0);
   // vColor = vec3(1.0, 0.0, 0.0);
}
