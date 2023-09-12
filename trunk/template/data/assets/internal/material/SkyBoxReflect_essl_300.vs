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

struct type_CustomBuffer
{
    vec4 _MainTex_ST;
};

uniform type_CustomBuffer CustomBuffer;

layout(location = 0) in vec4 in_var_POSITION;
layout(location = 1) in vec3 in_var_NORMAL;
layout(location = 2) in vec2 in_var_TEXCOORD0;
out vec2 varying_TEXCOORD0;
out vec3 varying_TEXCOORD1;
out vec3 varying_TEXCOORD2;

void main()
{
    gl_Position = in_var_POSITION * TransformBuffer.MATRIX_MVP;
    varying_TEXCOORD0 = (in_var_TEXCOORD0 * CustomBuffer._MainTex_ST.xy) + CustomBuffer._MainTex_ST.zw;
    varying_TEXCOORD1 = (in_var_POSITION * TransformBuffer.MATRIX_M).xyz;
    varying_TEXCOORD2 = normalize(in_var_NORMAL * mat3(TransformBuffer.MATRIX_IT_M[0].xyz, TransformBuffer.MATRIX_IT_M[1].xyz, TransformBuffer.MATRIX_IT_M[2].xyz));
}

