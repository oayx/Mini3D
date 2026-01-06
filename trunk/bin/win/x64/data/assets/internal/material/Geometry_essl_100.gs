#version 100
#extension GL_EXT_geometry_shader : require
layout(points) in;
layout(max_vertices = 24, points) out;

in vec4 var_POSITION[1];
in vec4 var_COLOR0[1];
out vec4 var_TEXCOORD0;

void main()
{
    gl_Position = var_POSITION[0];
    var_TEXCOORD0 = var_COLOR0[0];
    EmitVertex();
}

