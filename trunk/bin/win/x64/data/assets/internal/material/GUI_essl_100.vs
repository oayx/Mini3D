#version 100

struct type_CustomBuffer
{
    mat4 ProjectionMatrix;
};

uniform type_CustomBuffer CustomBuffer;

attribute vec4 in_var_POSITION;
attribute vec4 in_var_COLOR0;
attribute vec2 in_var_TEXCOORD0;
varying vec4 varying_TEXCOORD0;
varying vec2 varying_TEXCOORD1;

void main()
{
    gl_Position = vec4(in_var_POSITION.xy, 0.0, 1.0) * CustomBuffer.ProjectionMatrix;
    varying_TEXCOORD0 = in_var_COLOR0;
    varying_TEXCOORD1 = in_var_TEXCOORD0;
}

