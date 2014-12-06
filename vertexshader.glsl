#version 130

varying vec2 pos;

void main()
{
pos=gl_Vertex.xy;
gl_Position=vec4(2.0*pos-vec2(1.0,1.0),0.0,1.0);
}
