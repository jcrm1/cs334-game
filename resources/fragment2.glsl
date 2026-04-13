#version 330 core
out vec4 FragColor;

in vec3 vNormal;
in vec3 vPos;
in vec3 vColor;

const vec3 lightPos = vec3(200.0, 200.0, 0.0);

void main() {
    // FragColor = vec4(vColor, 1.0);
    vec3 lightDir = normalize(lightPos - vPos);
    vec3 normal = normalize(vNormal);

    // float a = dot(lightDir, normal);
    // if (a < 0.0) {
    //     FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    // } else {
    //     float ambientStrength = 0.5;
    //     vec3 ambient = ambientStrength * vColor;
    //     vec3 diffuse = a * vColor;
    //     vec3 result = ambient + diffuse;
    //     FragColor = vec4(result, 1.0);
    // }

    float a = max(dot(lightDir, normal), 0.0);
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * vColor;
    vec3 diffuse = a * vColor;
    vec3 result = ambient + diffuse;
    FragColor = vec4(result, 1.0);
    
}
