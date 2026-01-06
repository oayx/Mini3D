#version 100

struct type_TransformBuffer
{
    mat4 MATRIX_MVP;
    mat4 MATRIX_M;
    mat4 MATRIX_MV;
    mat4 MATRIX_I_M;
    mat4 MATRIX_T_M;
    mat4 MATRIX_T_MV;
    mat4 MATRIX_IT_M;
    mat4 MATRIX_IT_MV;
};

uniform type_TransformBuffer TransformBuffer;

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
varying vec4 varying_TEXCOORD0;
varying vec2 varying_TEXCOORD1;
varying float varying_TEXCOORD2;

void main()
{
    vec4 _42 = (in_var_POSITION * in_var_POSITION1) * TransformBuffer.MATRIX_MVP;
    gl_Position = _42;
    varying_TEXCOORD0 = in_var_COLOR0 * CustomBuffer._Color;
    varying_TEXCOORD1 = (in_var_TEXCOORD0 * CustomBuffer._MainTex_ST.xy) + CustomBuffer._MainTex_ST.zw;
    varying_TEXCOORD2 = _42.z;
}

