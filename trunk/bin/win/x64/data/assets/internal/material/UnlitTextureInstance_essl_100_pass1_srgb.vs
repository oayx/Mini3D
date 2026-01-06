#version 100

struct type_CustomBuffer
{
    mat4 LIGHT_MATRIX_MVP;
    vec4 _MainTex_ST;
    vec4 _Color;
};

uniform type_CustomBuffer CustomBuffer;

attribute vec4 in_var_POSITION;
attribute vec4 in_var_COLOR0;
attribute vec2 in_var_TEXCOORD0;
attribute mat4 in_var_POSITION1;
varying vec2 varying_TEXCOORD0;

void main()
{
    vec4 _31 = (in_var_POSITION * in_var_POSITION1) * CustomBuffer.LIGHT_MATRIX_MVP;
    gl_Position = _31;
    varying_TEXCOORD0 = _31.zw;
}

