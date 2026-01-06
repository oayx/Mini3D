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
    vec4 _Color;
};

uniform type_CustomBuffer CustomBuffer;

attribute vec4 in_var_POSITION;
attribute vec4 in_var_COLOR0;
varying vec4 varying_TEXCOORD0;
varying float varying_TEXCOORD1;

void main()
{
    vec4 _30 = in_var_POSITION * TransformBuffer.MATRIX_MVP;
    gl_Position = _30;
    varying_TEXCOORD0 = in_var_COLOR0 * CustomBuffer._Color;
    varying_TEXCOORD1 = _30.z;
}

