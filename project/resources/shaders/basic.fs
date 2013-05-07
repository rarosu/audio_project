#version 400

in vec2 ex_TexCoord;
in vec4 ex_LightInterpolation;
out vec4 out_Color;

uniform sampler2D Texture;

void main(void) {
    out_Color = texture(Texture, ex_TexCoord).rgba;
    out_Color *= clamp(ex_LightInterpolation + 0.10, 0, 1);
}
