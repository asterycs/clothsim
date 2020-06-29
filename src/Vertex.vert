uniform highp mat4 transformationMatrix;
uniform highp mat4 projectionMatrix;
uniform highp mat3 normalMatrix;
uniform highp vec3 light;

layout(location = 0) in highp vec4 inVertexPosition;
layout(location = 1) in highp vec3 inNormal;

out highp vec3 transformedNormal;
out highp vec3 lightDirection;

void main() {
    highp vec4 transformedPosition4 = transformationMatrix*inVertexPosition;
    highp vec3 transformedPosition = transformedPosition4.xyz/transformedPosition4.w;

    transformedNormal = normalMatrix*inNormal;
    lightDirection = normalize(light - transformedPosition);

    gl_Position = projectionMatrix*transformedPosition4;
}