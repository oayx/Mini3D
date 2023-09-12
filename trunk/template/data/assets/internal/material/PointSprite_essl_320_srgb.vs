#version 320 es

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
layout(location = 1) in float in_var_SIZE;
layout(location = 0) out vec4 out_var_POSITION;
layout(location = 1) out float out_var_SIZE;

void main()
{
    out_var_POSITION = in_var_POSITION * TransformBuffer.MATRIX_M;
    out_var_SIZE = in_var_SIZE;
}

