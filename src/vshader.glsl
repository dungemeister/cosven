#version 450
in gl_
void main(void)
{   
    if(gl_VertexID == 0) {gl_Position = vec4(500.0, 400.0, 0.0, 1.0);}
    else
    {gl_Position = vec4(0.0, 0.0, 0.0, 1.0);}
}
