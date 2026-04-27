#version 330 core
out vec4 FragColor;

in vec3 vNormal;
in vec3 vPos;
in vec3 vColor;
in float vHeight;

uniform vec3 cameraPos;
uniform vec4 clearColor;

uniform float fogStart;
uniform float fogLength;
uniform float seaLevel;

const vec3 lightPos = vec3(200.0, 200.0, 0.0);

void main() {
    vec3 outColor = vec3(1.0,0.0,0.0);
    if (vHeight < seaLevel) {
        float depth = (seaLevel - vHeight) / seaLevel;
        outColor = vec3(0.0, 0.3 - depth * 0.2, clamp(0.8 - depth * 0.3, 0.0, 1.0));
    } else {
        outColor = vColor;
    }
    float dist = distance(vPos, cameraPos);
    if (dist > fogStart + fogLength) {
        outColor = clearColor.xyz;
    } else if (dist > fogStart) {
        // higher mix value = more clear color
        // dist = 50 -> min(0, 1) -> 0
        // dist = 60 -> min(0.4, 1) -> 0.4
        // dist = 70 -> min(0.8, 1) -> 0.8
        // dist = 75 -> min(1, 1) -> 1
        // dist = 80 -> min(1.2, 1) -> 1
        // dist = 85 -> min(1.4, 1) -> 1
        // dist = 100 -> min(2, 1) -> 1
        outColor = mix(outColor, clearColor.xyz, min((dist - fogStart) / fogLength, 1.0));
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
