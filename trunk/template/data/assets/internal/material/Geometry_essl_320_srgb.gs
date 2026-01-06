#version 320 es
layout(points) in;
layout(max_vertices = 24, points) out;

layout(location = 0) in vec4 var_POSITION[1];
layout(location = 1) in vec4 var_COLOR0[1];
layout(location = 0) out vec4 var_TEXCOORD0;

void main()
{
    gl_Position = var_POSITION[0];
    var_TEXCOORD0 = var_COLOR0[0];
    EmitVertex();
}

