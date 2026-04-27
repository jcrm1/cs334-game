#version 330 core
out vec4 FragColor;

in vec3 vNormal;
in vec3 vModelNormal;
in vec3 vPos;
in float vHeight;
in float vBiome;

uniform vec3 cameraPos;
uniform vec4 clearColor;

uniform bool enableFog;
uniform float fogStart;
uniform float fogLength;
uniform float seaLevel;
uniform float terrainScale;
uniform bool near;

uniform sampler2D grassTex;
uniform sampler2D rockTex;
uniform sampler2D snowTex;

void main() {
    vec3 outColor;

    {
        vec3 normal = normalize(vModelNormal);
        float steepness = 1.0 - abs(dot(normal, vec3(0.0, 1.0, 0.0)));

        // World-space UVs for texture tiling
        vec2 uv = vPos.xz * 2.0;

        // Sample textures
        vec3 grassColor = texture(grassTex, uv).rgb;
        vec3 rockColor  = texture(rockTex, uv).rgb;
        vec3 snowColor  = texture(snowTex, uv).rgb;

        float steep_t = smoothstep(0.004, 0.015, steepness);

        // Each biome blends between grass/snow (flat) and rock (steep)
        vec3 plains_color   = mix(grassColor, rockColor, steep_t);
        vec3 forest_color   = mix(grassColor * 0.7, rockColor, steep_t); // darker grass for forest
        vec3 mountain_color = mix(rockColor, rockColor, steep_t);

        // Triangular basis weights from biome value (same logic as CPU side)
        float w_plains   = clamp(1.0 - vBiome * 2.0, 0.0, 1.0);
        float w_mountain = clamp(vBiome * 2.0 - 1.0, 0.0, 1.0);
        float w_forest   = 1.0 - w_plains - w_mountain;

        outColor = w_plains * plains_color + w_forest * forest_color + w_mountain * mountain_color;

        // Snow replaces grass in mountain biome — sharp cutoff based on biome weight
        float snow_t = smoothstep(0.2, 0.4, w_mountain) * (1.0 - steep_t);
        if (snow_t > 0.5) {
            outColor = snowColor;
        }
    } // end biome coloring

    float dist = distance(vPos, cameraPos);
    if (enableFog) {
        if (dist > fogStart + fogLength) {
            outColor = clearColor.xyz;
        } else if (dist > fogStart) {
            outColor = mix(outColor, clearColor.xyz, min((dist - fogStart) / fogLength, 1.0));
        }
    } else {
        if (!near && dist > fogStart + fogLength - (fogLength / 10.0)) {
        } else if (near && dist <= fogStart + fogLength) {
        } else {
            discard;
        }
    }
    FragColor = vec4(outColor, gl_FragCoord.z);
}
