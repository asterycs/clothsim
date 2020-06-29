uniform highp int objectId;
uniform highp vec3 vertexColor;

in highp vec3 transformedNormal;
in highp vec3 lightDirection;

layout(location = 0) out highp vec4 outColor;
layout(location = 1) out highp int outObjectId;

// Seems like a WebGL bug. Without these declarations
// the targets remain uncleared
layout(location = 2) out highp vec4 outSumColor;
layout(location = 3) out highp vec4 outSumWeight;

flat in highp int vertexID;

void main() {
    mediump vec3 normalizedTransformedNormal = normalize(transformedNormal);
    highp vec3 normalizedLightDirection = normalize(lightDirection);

    mediump vec3 color;
    color = vertexColor * 0.3f;

    lowp float intensity = max(0.0, clamp(dot(normalizedTransformedNormal, normalizedLightDirection), 0.0f, 1.0f));
    color += vertexColor*intensity;

    outObjectId = objectId;
    outColor = vec4(color, 1.f);
}
