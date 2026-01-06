#version 100

struct type_CustomBuffer
{
    mat4 LIGHT_MATRIX_MVP;
};

uniform type_CustomBuffer CustomBuffer;

attribute vec4 in_var_POSITION;
varying vec2 varying_TEXCOORD0;

void main()
{
    vec4 _24 = in_var_POSITION * CustomBuffer.LIGHT_MATRIX_MVP;
    gl_Position = _24;
    varying_TEXCOORD0 = _24.zw;
}

