#version 330 core
out vec4 FragColor;

in vec2 screenLightPos;

uniform uint screenWidth;
uniform uint screenHeight;
uniform vec4 clearColor;
uniform sampler2D prevTex;
uniform sampler2D maskTex;

// godrays from https://developer.nvidia.com/gpugems/gpugems3/part-ii-light-and-shadows/chapter-13-volumetric-light-scattering-post-process

const int numSamples = 100;
const float density = 1.0;
const float weight = 0.01;
const float decay = 1.0;
const float exposure = 1;

void main() {
  vec2 texCoord = gl_FragCoord.xy / vec2(screenWidth, screenHeight);
  vec3 sunlight = vec3(0,0,0);
  if (screenLightPos.x >= 0 && screenLightPos.x <= 1) {
    // Render the sun
    float d = 1 - clamp((distance(screenLightPos, texCoord) * 10), 0, 1);
    sunlight = vec3(d, d, d);
  }
  if (screenLightPos.x >= 0 && texture(maskTex, texCoord).x == 0) {
    vec2 deltaTexCoord = (texCoord - screenLightPos) * (density * (1.0 / numSamples));
    float illuminationDecay = 1.0;
    vec2 rayTexCoord = texCoord;
    vec3 color = texture(maskTex, texCoord).xyz;
    for (int i = 0; i < numSamples; i += 1) {
      rayTexCoord -= deltaTexCoord;
      vec3 sample = texture(maskTex, rayTexCoord).xyz;
      sample *= illuminationDecay * weight;
      color += sample;
      illuminationDecay *= decay;
    }
    FragColor = vec4(mix(sunlight + texture(prevTex, texCoord).xyz, (color * exposure), 0.2), 1.0);
  } else {
    FragColor = vec4(sunlight + texture(prevTex, texCoord).xyz, 1.0);
  }
}
