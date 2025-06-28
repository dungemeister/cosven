
#version 450

out vec4 FragColor;

in vec3 TexCoord;

uniform samplerCube skybox;

void main(void)
{
     FragColor = texture(skybox, TexCoord);

}
