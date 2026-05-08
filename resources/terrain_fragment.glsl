#version 330 core
out vec4 FragColor;

in vec3 vNormal;
in vec3 vPos;
in vec3 vColor;
in float vHeight;

uniform vec3 cameraPos;
uniform vec4 clearColor;

uniform bool enableFog;
uniform float fogStart;
uniform float fogLength;
uniform float seaLevel;
uniform bool near;

uniform vec3 lightPos;

const float specularPower = 32;
const float specStrength = 0.2;
const vec3 specularColor = vec3(1,1,1);
const float ambientScalar = 0.9;

void main() {
    vec3 outColor = vec3(1.0,0.0,0.0);
    if (vHeight < seaLevel) {
        float depth = (seaLevel - vHeight) / seaLevel;
        outColor = vec3(0.0, 0.3 - depth * 0.2, clamp(0.8 - depth * 0.3, 0.0, 1.0));
    } else {
        outColor = vColor;
    }
    // apply phong shading based on info at https://mrl.cs.nyu.edu/~perlin/courses/fall2005ugrad/phong.html
    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(lightPos - vPos);
    vec3 cameraDir = normalize(cameraPos - vPos);
    
    vec3 ambient = outColor * ambientScalar;
    float nl = max(0, dot(normal, lightDir));
    vec3 diffuse = outColor * nl;
    vec3 specular = specularColor * pow(max(0, dot(cameraDir, reflect(-1 * lightDir, normal))), specularPower) * specStrength;
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
        if (!near && dist > fogStart + fogLength - (fogLength / 10.0)) {
            // do nothing! this will output to FragColor
        } else if (near && dist <= fogStart + fogLength) {
            // do nothing! this will output to FragColor
        } else {
            discard; // this will not output to FragColor
        }
    }
    FragColor = vec4(outColor, gl_FragCoord.z);
}
