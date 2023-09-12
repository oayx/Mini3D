#version 300 es
#extension GL_EXT_geometry_shader : require
layout(points) in;
layout(max_vertices = 24, points) out;

in vec4 in_var_POSITION[1];
in vec4 in_var_COLOR0[1];
out vec4 out_var_TEXCOORD0;

void main()
{
    gl_Position = in_var_POSITION[0];
    out_var_TEXCOORD0 = in_var_COLOR0[0];
    EmitVertex();
}

