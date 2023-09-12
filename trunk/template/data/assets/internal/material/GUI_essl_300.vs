#version 300 es

struct type_CustomBuffer
{
    mat4 ProjectionMatrix;
};

uniform type_CustomBuffer CustomBuffer;

layout(location = 0) in vec4 in_var_POSITION;
layout(location = 1) in vec4 in_var_COLOR0;
layout(location = 2) in vec2 in_var_TEXCOORD0;
out vec4 varying_TEXCOORD0;
out vec2 varying_TEXCOORD1;

void main()
{
    gl_Position = vec4(in_var_POSITION.xy, 0.0, 1.0) * CustomBuffer.ProjectionMatrix;
    varying_TEXCOORD0 = in_var_COLOR0;
    varying_TEXCOORD1 = in_var_TEXCOORD0;
}

