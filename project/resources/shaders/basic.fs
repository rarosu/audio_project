#version 400

in vec2 ex_TexCoord;
in vec3 ex_NormalV;
in vec4 ex_PositionV;
out vec4 out_Color;

uniform sampler2D Texture;

uniform vec4 g_LightPositionV;
uniform vec3 g_LightIntensity;
uniform vec3 g_AmbientIntensity;

void main(void) {
    out_Color = texture(Texture, ex_TexCoord).rgba;
    
    float incidence = dot(normalize(g_LightPositionV - ex_PositionV).xyz, ex_NormalV);
    incidence = clamp(incidence, 0, 1);
    
    out_Color *= vec4(g_LightIntensity * incidence + g_AmbientIntensity, 1.0f);
}
