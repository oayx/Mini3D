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
    mat4 LIGHT_MATRIX_MVP;
    vec4 _MainTex_ST;
    mat4 BoneMatrixArray[60];
};

uniform type_CustomBuffer CustomBuffer;

attribute vec4 in_var_POSITION;
attribute vec3 in_var_NORMAL;
attribute vec4 in_var_TANGENT;
attribute vec2 in_var_TEXCOORD0;
attribute vec4 in_var_BLENDINDICES;
attribute vec4 in_var_BLENDWEIGHT;
varying vec2 varying_TEXCOORD0;
varying float varying_TEXCOORD1;

void main()
{
    vec4 _82 = vec4((((((in_var_POSITION * CustomBuffer.BoneMatrixArray[int(in_var_BLENDINDICES.x)]) * in_var_BLENDWEIGHT.x) + ((in_var_POSITION * CustomBuffer.BoneMatrixArray[int(in_var_BLENDINDICES.y)]) * in_var_BLENDWEIGHT.y)) + ((in_var_POSITION * CustomBuffer.BoneMatrixArray[int(in_var_BLENDINDICES.z)]) * in_var_BLENDWEIGHT.z)) + ((in_var_POSITION * CustomBuffer.BoneMatrixArray[int(in_var_BLENDINDICES.w)]) * in_var_BLENDWEIGHT.w)).xyz, 1.0) * TransformBuffer.MATRIX_MVP;
    gl_Position = _82;
    varying_TEXCOORD0 = (in_var_TEXCOORD0 * CustomBuffer._MainTex_ST.xy) + CustomBuffer._MainTex_ST.zw;
    varying_TEXCOORD1 = _82.z;
}

