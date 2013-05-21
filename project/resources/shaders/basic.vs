#version 400

layout(location=0) in vec4 in_PositionM;
layout(location=1) in vec4 in_NormalM;
layout(location=2) in vec2 in_TexCoord;
out vec2 ex_TexCoord;
out vec3 ex_NormalV;
out vec4 ex_PositionV;

uniform mat4 g_Projection;
uniform mat4 g_ViewWorld;
uniform mat3 g_NormalViewWorld;

void main(void) {
    gl_Position = g_Projection * g_ViewWorld * in_PositionM;
    ex_TexCoord = in_TexCoord;
    ex_NormalV = normalize(g_NormalViewWorld * in_NormalM.xyz);
    ex_PositionV = g_ViewWorld * in_PositionM;
}
