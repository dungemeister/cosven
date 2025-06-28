
#version 450

layout (location = 0) in vec3 position;

out vec3 TexCoord;

uniform mat4 view;
uniform mat4 projection;

void main(void)
{  
    vec4 pos = projection * view * vec4(position, 1.0);
    TexCoord = position;
    gl_Position = pos.xyww;

}
