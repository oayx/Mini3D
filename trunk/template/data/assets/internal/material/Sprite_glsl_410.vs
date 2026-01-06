#version 410

out gl_PerVertex
{
    vec4 gl_Position;
};

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
layout(location = 1) in vec4 in_var_COLOR0;
layout(location = 2) in vec2 in_var_TEXCOORD0;
layout(location = 0) out vec4 var_TEXCOORD0;
layout(location = 1) out vec2 var_TEXCOORD1;
layout(location = 2) out float var_TEXCOORD2;

void main()
{
    vec4 _36 = in_var_POSITION * TransformBuffer.MATRIX_MVP;
    gl_Position = _36;
    var_TEXCOORD0 = in_var_COLOR0;
    var_TEXCOORD1 = (in_var_TEXCOORD0 * CustomBuffer._MainTex_ST.xy) + CustomBuffer._MainTex_ST.zw;
    var_TEXCOORD2 = _36.z;
}

