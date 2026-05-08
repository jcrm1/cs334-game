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
uniform bool enableGodrays;

// godrays from https://developer.nvidia.com/gpugems/gpugems3/part-ii-light-and-shadows/chapter-13-volumetric-light-scattering-post-process

const int numSamples = 100;
const float density = 1.0;
const float weight = 0.0025;
const float decay = 1.0;
const float exposure = 1;

float mapf(float val, float in_min, float in_max, float out_min, float out_max) {
  return ((((val - in_min) / (in_max - in_min)) * (out_max - out_min)) + out_min);
}

vec2 map2f(vec2 val, vec2 in_min, vec2 in_max, vec2 out_min, vec2 out_max) {
  return ((((val - in_min) / (in_max - in_min)) * (out_max - out_min)) + out_min);
}

// incoming sun curve is like y=-x+1
// outgoing sun curve should be like a triangular pdf with a=0,c=0.5,b=1
vec2 sunCurve(vec2 val) {
  float x, y;
  if (val.x < 0.5) {
    x = mapf(val.x, 0, 0.5, 0, 1);
  } else {
    x = mapf(val.x, 0.5, 1, 1, 0);
  }
  if (val.y < 0.5) {
    y = mapf(val.y, 0, 0.5, 0, 1);
  } else {
    y = mapf(val.y, 0.5, 1, 1, 0);
  }
  return clamp(vec2(x, y), 0, 1);
}

void main() {
  vec2 texCoord = gl_FragCoord.xy / vec2(screenWidth, screenHeight);
  vec3 sunlight = vec3(0,0,0);
  // FragColor = vec4(clamp(screenLightPos.x, 0, 1), 0, 0, 1);
  if (screenLightPos.x >= 0 && screenLightPos.x <= 1 && texture(maskTex, texCoord).x == 1) {
    // Render the sun
    float d = 1 - clamp((distance(screenLightPos, texCoord) * 10), 0, 1);
    sunlight = vec3(d, d, d);
  }
  // float dist = distance(vPos, cameraPos);
  vec3 sunPlusColor = sunlight + texture(prevTex, texCoord).xyz;
  if (enableGodrays) {
    if (texture(maskTex, texCoord).x != 1) {
      // terrain
      if (screenLightPos.x <= 1 && screenLightPos.x >= 0) {
        // sun is on-screen
        vec2 deltaTexCoord = (texCoord - screenLightPos) * (density * (1.0 / numSamples));
        float illuminationDecay = 1.0;
        vec2 rayTexCoord = texCoord;
        float mx = texture(maskTex, texCoord).r;
        float mxc = floor(mx);
        vec3 orig_mask = vec3(mxc, mxc, mxc);
        vec3 color = orig_mask;
        for (int i = 0; i < numSamples; i += 1) {
          rayTexCoord -= deltaTexCoord;
          float mxr = texture(maskTex, rayTexCoord).r;
          float mxrc = floor(mxr);
          vec3 sample = vec3(mxrc, mxrc, mxrc);
          sample *= illuminationDecay * weight;
          color += sample;
          illuminationDecay *= decay;
        }
        FragColor = vec4(mix(sunPlusColor + (color * exposure), clearColor.rgb, mx), 1.0);
      } else {
        // sun is off-screen
        // float fakeLight = weight * numSamples;
        float fakeLight = 0;
        FragColor = vec4(sunPlusColor + (vec3(fakeLight, fakeLight, fakeLight) * exposure), 1.0);
      }
    } else {
      // not terrain
      // FragColor = vec4(mix(sunPlusColor, vec3(1,1,1), 0.2), 1.0);
      FragColor = vec4(sunPlusColor, 1.0);
    }
  } else {
    // not terrain
    FragColor = vec4(sunPlusColor, 1.0);
  }
  // FragColor = vec4(texture(maskTex, texCoord).z, texture(maskTex, texCoord).z, texture(maskTex, texCoord).z, 1.0);
  // 0 = close
  // 1 = far
}
