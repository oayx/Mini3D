#version 460
layout(points) in;
layout(max_vertices = 24, points) out;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) in vec4 var_POSITION[1];
layout(location = 1) in vec4 var_COLOR0[1];
layout(location = 0) out vec4 var_TEXCOORD0;

void main()
{
    gl_Position = var_POSITION[0];
    var_TEXCOORD0 = var_COLOR0[0];
    EmitVertex();
}

