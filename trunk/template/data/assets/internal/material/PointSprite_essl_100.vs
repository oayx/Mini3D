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

attribute vec4 in_var_POSITION;
attribute float in_var_SIZE;
varying vec4 varying_POSITION;
varying float varying_SIZE;

void main()
{
    varying_POSITION = in_var_POSITION * TransformBuffer.MATRIX_M;
    varying_SIZE = in_var_SIZE;
}

