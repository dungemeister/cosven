
#version 450

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D u_texture;
uniform samplerCube u_cube_texture;
uniform float u_green;

void main(void)
{
    //  FragColor = texture(u_texture, TexCoord);
    // FragColor = texture(u_logo, TexCoord);
    // FragColor = vec4(TexCoord.x, TexCoord.y, 0.0, 1.0);
    // FragColor = mix(texture(u_logo, TexCoord), vec4(0.0, abs(sin(u_green)), 0.0, 1.0), 0.2);
    FragColor = mix(texture(u_texture, TexCoord), vec4(abs(sin(6 * u_green)), abs(sin(2 * u_green)), abs(sin(20 * u_green)), 1.0), 0.5);
    // FragColor = mix(texture(u_texture,TexCoord), mix(texture(u_logo, TexCoord), vec4(0.0, abs(sin(u_green)), 0.0, 1.0), 0.1), 1);
}
