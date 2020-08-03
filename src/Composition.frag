uniform sampler2D Opaque;
uniform sampler2D TransparencyAccumulation;
uniform sampler2D TransparencyRevealage;
uniform highp ivec2 viewportSize;

layout(location = 0) out highp vec4 outColor;

void main(void)
{
    highp vec2 texCoord = gl_FragCoord.xy/vec2(viewportSize);
    highp vec4 accumulation = texture(TransparencyAccumulation, texCoord);
    highp float revealage = texture(TransparencyRevealage,  texCoord).r;
    highp vec3 averageTransparentColor = accumulation.rgb / clamp(accumulation.a, 1e-4, 5e4);
    
    // Black background (vec3(0.f))
    averageTransparentColor = averageTransparentColor * (1.0f - revealage) + vec3(0.f) * revealage;

    highp vec4 opaqueColor = texture(Opaque, texCoord);
    
    outColor.rgb = averageTransparentColor * (1.f - opaqueColor.a) + opaqueColor.rgb;
    outColor.a = 1.f;
}