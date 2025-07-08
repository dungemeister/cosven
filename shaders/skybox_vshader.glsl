
#version 450 core

layout(location = 0) in vec3 aPos;
uniform mat4 mvp;
out vec3 TexCoords;
void main() {
    TexCoords = aPos;
    gl_Position = mvp * vec4(aPos, 1.0);
}
