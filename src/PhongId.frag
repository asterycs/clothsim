uniform highp float depthScale;

in highp vec3 transformedNormal;
in highp vec3 lightDirection;
in highp vec3 cameraDirection;
in highp vec3 vertexColor;

layout(location = 0) out highp vec4 outColor;
layout(location = 1) out highp int outObjectId;

void main() {
    mediump vec3 normalizedTransformedNormal = normalize(transformedNormal);
    highp vec3 normalizedLightDirection = normalize(lightDirection);

    mediump vec4 color;
    color.rgb = vertexColor * 0.3;

    lowp float intensity = max(0.0, abs(dot(normalizedTransformedNormal, normalizedLightDirection)));
    color.rgb += vertexColor*intensity;

    /*if(intensity > 0.001) {
        highp vec3 reflection = reflect(-normalizedLightDirection, normalizedTransformedNormal);
        mediump float specularity = pow(max(0.0, abs(dot(normalize(cameraDirection), reflection))), 80.0);
        color.rgb += vec3(1.0)*specularity;
    }*/

    color.a = 1.0;
    outObjectId = -1;
    //float closestEdge = min(uvFrag.x, min(uvFrag.y, 1.f - uvFrag.x - uvFrag.y));

    outColor = color;
}
