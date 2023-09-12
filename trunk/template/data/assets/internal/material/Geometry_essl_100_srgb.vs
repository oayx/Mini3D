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
varying vec4 varying_POSITION;
varying vec4 varying_COLOR0;

void main()
{
    varying_POSITION = in_var_POSITION * TransformBuffer.MATRIX_MVP;
    varying_COLOR0 = in_var_COLOR0 * CustomBuffer._Color;
}

