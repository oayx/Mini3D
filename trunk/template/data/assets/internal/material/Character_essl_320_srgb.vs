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

struct type_CustomBuffer
{
    mat4 LIGHT_MATRIX_MVP;
    vec4 _MainTex_ST;
    mat4 BoneMatrixArray[200];
};

uniform type_CustomBuffer CustomBuffer;

layout(location = 0) in vec4 in_var_POSITION;
layout(location = 1) in vec3 in_var_NORMAL;
layout(location = 2) in vec4 in_var_TANGENT;
layout(location = 3) in vec2 in_var_TEXCOORD0;
layout(location = 4) in vec4 in_var_BLENDINDICES;
layout(location = 5) in vec4 in_var_BLENDWEIGHT;
layout(location = 0) out vec2 out_var_TEXCOORD0;
layout(location = 1) out float out_var_TEXCOORD1;

void main()
{
    vec4 _45 = in_var_BLENDINDICES;
    vec4 _44 = in_var_BLENDWEIGHT;
    uint _50 = uint(0);
    uint _60 = uint(1);
    uint _71 = uint(2);
    uint _82 = uint(3);
    vec4 _99 = vec4((((((in_var_POSITION * CustomBuffer.BoneMatrixArray[uint(_45[_50])]) * _44[_50]) + ((in_var_POSITION * CustomBuffer.BoneMatrixArray[uint(_45[_60])]) * _44[_60])) + ((in_var_POSITION * CustomBuffer.BoneMatrixArray[uint(_45[_71])]) * _44[_71])) + ((in_var_POSITION * CustomBuffer.BoneMatrixArray[uint(_45[_82])]) * _44[_82])).xyz, 1.0) * TransformBuffer.MATRIX_MVP;
    gl_Position = _99;
    out_var_TEXCOORD0 = (in_var_TEXCOORD0 * CustomBuffer._MainTex_ST.xy) + CustomBuffer._MainTex_ST.zw;
    out_var_TEXCOORD1 = _99.z;
}

