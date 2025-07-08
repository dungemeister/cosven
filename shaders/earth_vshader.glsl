
#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in mat4 instanceTransform;
uniform mat4 mvp;
uniform mat4 offsetModel;
out vec2 TexCoord;
void main() {
    gl_Position = mvp * instanceTransform *(offsetModel * vec4(aPos, 1.0));
    TexCoord = aTexCoord;
}
