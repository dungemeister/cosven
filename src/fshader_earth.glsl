
#version 450

out vec4 FragColor;

// in vec2 TexCoord;

uniform vec3 u_color;
// uniform sampler2D u_texture;

void main(void)
{

    FragColor = vec4(u_color, 1.0);
    // FragColor = texture(u_texture, TexCoord);
}
