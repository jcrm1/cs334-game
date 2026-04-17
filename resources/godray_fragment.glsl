#version 330 core
out vec4 FragColor;

in vec2 screenLightPos;
in vec3 vPos;

uniform uint screenWidth;
uniform uint screenHeight;
uniform vec4 clearColor;
uniform sampler2D prevTex;
uniform sampler2D maskTex;
uniform float fogStart;
uniform float fogLength;
// const float fogStart = 50.0;
// const float fogLength = 25.0;
uniform vec3 cameraPos;

// godrays from https://developer.nvidia.com/gpugems/gpugems3/part-ii-light-and-shadows/chapter-13-volumetric-light-scattering-post-process

const int numSamples = 100;
const float density = 1.0;
const float weight = 0.01;
const float decay = 1.0;
const float exposure = 1;

void main() {
  vec2 texCoord = gl_FragCoord.xy / vec2(screenWidth, screenHeight);
  vec3 sunlight = vec3(0,0,0);
  if (screenLightPos.x >= 0 && screenLightPos.x <= 1 && texture(maskTex, texCoord).x == 1) {
    // Render the sun
    float d = 1 - clamp((distance(screenLightPos, texCoord) * 10), 0, 1);
    sunlight = vec3(d, d, d);
  }
  float dist = distance(vPos, cameraPos);
  vec3 sunPlusColor = sunlight + texture(prevTex, texCoord).xyz;
  if (texture(maskTex, texCoord).x != 1 && screenLightPos.x >= 0) {
    // terrain

    // THE PROBLEM:
    // The vPos I get from vertex shader is not the coordinate
    // of the terrain vertex like I think it is. Rather, it is
    // the coordinate of a vertex of the SCREEN SPACE TRIANGLE. AUGHHHHH
    // edit: maybe fixed? leaving note for future reference
    vec2 deltaTexCoord = (texCoord - screenLightPos) * (density * (1.0 / numSamples));
    float illuminationDecay = 1.0;
    vec2 rayTexCoord = texCoord;
    vec3 orig_mask = texture(maskTex, texCoord).xyz;
    vec3 color = orig_mask;
    for (int i = 0; i < numSamples; i += 1) {
      rayTexCoord -= deltaTexCoord;
      vec3 sample = texture(maskTex, rayTexCoord).xyz;
      sample *= illuminationDecay * weight;
      color += sample;
      illuminationDecay *= decay;
    }
    FragColor = vec4(mix(mix(sunPlusColor, (color * exposure), 0.2), clearColor.xyz, orig_mask.x), 1.0);
  } else {
    // not terrain
    FragColor = vec4(sunPlusColor, 1.0);
  }
}
