#version 330 core
out vec4 FragColor;

// in vec3 vNormal;
in vec3 vPos;
in vec3 vColor;

uniform vec3 cameraPos;
uniform vec4 clearColor;

const float fogStart = 25;
const float fogLength = 50;

// const vec3 lightPos = vec3(200.0, 200.0, 0.0);

void main() {
    vec3 outColor = vec3(1.0,0.0,0.0);
    if (vColor.y < 0.35) {
        outColor = vec3(0.0, vColor.y, clamp(vColor.y * 4, 0.0, 1.0));
    } else {
        outColor = vColor;
    }
    float dist = length(vPos - cameraPos);
    if (dist > fogStart + fogLength) {
        discard;
    } else if (dist > fogStart) {
        outColor = mix(outColor, clearColor.xyz, min((mod(dist - fogStart, fogLength)) / fogStart, 1.0));
    }
    FragColor = vec4(outColor, 1.0);

    // vec3 lightDir = normalize(lightPos - vPos);
    // vec3 normal = normalize(vNormal);

    // // float a = dot(lightDir, normal);
    // // if (a < 0.0) {
    // //     FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    // // } else {
    // //     float ambientStrength = 0.5;
    // //     vec3 ambient = ambientStrength * vColor;
    // //     vec3 diffuse = a * vColor;
    // //     vec3 result = ambient + diffuse;
    // //     FragColor = vec4(result, 1.0);
    // // }

    // float a = max(dot(lightDir, normal), 0.0);
    // float ambientStrength = 0.2;
    // vec3 ambient = ambientStrength * vColor;
    // vec3 diffuse = a * vColor;
    // vec3 result = ambient + diffuse;
    // FragColor = vec4(result, 1.0);
    
}
