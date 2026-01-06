#version 300 es

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

layout(location = 0) in vec4 in_var_POSITION;
layout(location = 1) in vec4 in_var_COLOR0;
out vec4 varying_TEXCOORD0;

void main()
{
    gl_Position = in_var_POSITION * TransformBuffer.MATRIX_MVP;
    varying_TEXCOORD0 = in_var_COLOR0;
}

