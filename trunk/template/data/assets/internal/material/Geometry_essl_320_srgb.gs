#version 320 es
layout(points) in;
layout(max_vertices = 24, points) out;

layout(location = 0) in vec4 in_var_POSITION[1];
layout(location = 1) in vec4 in_var_COLOR0[1];
layout(location = 0) out vec4 out_var_TEXCOORD0;

void main()
{
    gl_Position = in_var_POSITION[0];
    out_var_TEXCOORD0 = in_var_COLOR0[0];
    EmitVertex();
}

