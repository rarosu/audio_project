#version 400

layout(location=0) in vec4 in_PositionM;
layout(location=1) in vec4 in_NormalM;
layout(location=2) in vec2 in_TexCoord;
out vec2 ex_TexCoord;
out vec4 ex_LightInterpolation;

uniform mat4 g_Projection;
uniform mat4 g_ViewWorld;
uniform mat4 g_NormalViewWorld;

uniform vec4 g_LightDirection;
uniform vec4 g_LightIntensity;

void main(void) {
    gl_Position = g_Projection * g_ViewWorld * in_PositionM;
    ex_TexCoord = in_TexCoord;

    // Gourard per-vertex shading
    vec4 normalV = normalize(g_NormalViewWorld * in_NormalM);
    float incidence = -dot(normalV, g_LightDirection);
    incidence = clamp(incidence, 0, 1);

    ex_LightInterpolation = g_LightIntensity * incidence;
}
