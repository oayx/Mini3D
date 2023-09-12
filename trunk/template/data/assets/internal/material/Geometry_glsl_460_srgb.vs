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

struct type_CustomBuffer
{
    vec4 _Color;
};

uniform type_CustomBuffer CustomBuffer;

layout(location = 0) in vec4 in_var_POSITION;
layout(location = 1) in vec4 in_var_COLOR0;
layout(location = 0) out vec4 out_var_POSITION;
layout(location = 1) out vec4 out_var_COLOR0;

void main()
{
    out_var_POSITION = in_var_POSITION * TransformBuffer.MATRIX_MVP;
    out_var_COLOR0 = in_var_COLOR0 * CustomBuffer._Color;
}

