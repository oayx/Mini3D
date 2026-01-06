#version 100

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

void main()
{
    vec4 _69 = (((((in_var_POSITION * CustomBuffer.BoneMatrixArray[int(in_var_BLENDINDICES.x)]) * in_var_BLENDWEIGHT.x) + ((in_var_POSITION * CustomBuffer.BoneMatrixArray[int(in_var_BLENDINDICES.y)]) * in_var_BLENDWEIGHT.y)) + ((in_var_POSITION * CustomBuffer.BoneMatrixArray[int(in_var_BLENDINDICES.z)]) * in_var_BLENDWEIGHT.z)) + ((in_var_POSITION * CustomBuffer.BoneMatrixArray[int(in_var_BLENDINDICES.w)]) * in_var_BLENDWEIGHT.w)) * CustomBuffer.LIGHT_MATRIX_MVP;
    gl_Position = _69;
    varying_TEXCOORD0 = _69.zw;
}

