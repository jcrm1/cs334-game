#version 330 core
out vec4 FragColor;

in vec3 vPos;
in vec3 vColor;
in float vHeight;

uniform vec3 cameraPos;
uniform vec3 lightPos;

uniform vec4 clearColor;
uniform bool enableFog;
uniform float fogStart;
uniform float fogLength;
uniform float seaLevel;
uniform bool near;

const float specularPower = 80;
const float specStrength = 0.3;
const vec3 specularColor = vec3(1,1,1);
const float ambientScalar = 0.7;

void main() {
    vec3 vNormal = vec3(0,1,0);
    vec3 outColor = vColor;
    float mySpecularPower = specularPower;
    float mySpecStrength = specStrength;
    float fractY = fract(vPos.y);
    // if (fractY >= 0.25 && fractY <= 0.75) {
    vNormal = normalize(vNormal + vec3(0.025 * cos(((8 * vPos.x) + (12 * vPos.z))),sin(vPos.y),0.05 * sin(4 * ((4 * vPos.x) + (6 * vPos.z)))));
    // mySpecularPower += 4 * sin(length(normalize(vec2(5 * vPos.x, 3 * vPos.z))));
    // mySpecStrength += 0.1 * sin(length(normalize(vec2(4 * vPos.x - 4, 2 * vPos.z -1))));
    // }
    // apply phong shading based on info at https://mrl.cs.nyu.edu/~perlin/courses/fall2005ugrad/phong.html
    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(lightPos - vPos);
    vec3 cameraDir = normalize(cameraPos - vPos);
    
    vec3 ambient = outColor * ambientScalar;
    float nl = max(0, dot(normal, lightDir));
    vec3 diffuse = outColor * nl;
    vec3 specular = specularColor * pow(max(0, dot(cameraDir, reflect(-1 * lightDir, normal))), mySpecularPower) * mySpecStrength;
    outColor = ambient + diffuse + specular;

    float dist = distance(vPos, cameraPos);
    if (enableFog) {
        // fog mode
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
    } else {
        // LOD mode
        // gives a bit of extra margin with (fogLength / 10.0) to remove the seam in the world
        // do nothing
    }
    FragColor = vec4(outColor, 1);
    
}
