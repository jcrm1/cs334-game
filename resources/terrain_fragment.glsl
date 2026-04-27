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

void main() {
    vec3 outColor;

    {
        vec3 normal = normalize(vModelNormal);
        float steepness = 1.0 - abs(dot(normal, vec3(0.0, 1.0, 0.0)));

        // Per-biome colors: flat vs steep
        vec3 plains_flat  = vec3(0.45, 0.65, 0.25); // grass green
        vec3 plains_steep = vec3(0.55, 0.45, 0.30); // dirt brown

        vec3 forest_flat  = vec3(0.20, 0.45, 0.15); // dark green
        vec3 forest_steep = vec3(0.40, 0.35, 0.25); // muddy brown

        vec3 mountain_flat  = vec3(0.90, 0.92, 0.95); // snow white
        vec3 mountain_steep = vec3(0.50, 0.48, 0.45); // rock grey

        float steep_t = smoothstep(0.03, 0.08, steepness);

        vec3 plains_color   = mix(plains_flat, plains_steep, steep_t);
        vec3 forest_color   = mix(forest_flat, forest_steep, steep_t);
        vec3 mountain_color = mix(mountain_flat, mountain_steep, steep_t);

        // Triangular basis weights from biome value (same logic as CPU side)
        float w_plains   = clamp(1.0 - vBiome * 2.0, 0.0, 1.0);
        float w_mountain = clamp(vBiome * 2.0 - 1.0, 0.0, 1.0);
        float w_forest   = 1.0 - w_plains - w_mountain;

        outColor = w_plains * plains_color + w_forest * forest_color + w_mountain * mountain_color;
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
