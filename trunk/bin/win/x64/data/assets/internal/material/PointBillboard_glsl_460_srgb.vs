#version 460

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
layout(location = 1) in vec2 in_var_SIZE;
layout(location = 0) out vec4 var_POSITION;
layout(location = 1) out vec2 var_SIZE;

void main()
{
    var_POSITION = in_var_POSITION * TransformBuffer.MATRIX_M;
    var_SIZE = in_var_SIZE;
}

