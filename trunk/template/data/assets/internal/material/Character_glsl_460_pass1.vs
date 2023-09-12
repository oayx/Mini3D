#version 460

out gl_PerVertex
{
    vec4 gl_Position;
};

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

void main()
{
    vec4 _38 = in_var_BLENDINDICES;
    vec4 _37 = in_var_BLENDWEIGHT;
    uint _42 = uint(0);
    uint _52 = uint(1);
    uint _63 = uint(2);
    uint _74 = uint(3);
    vec4 _87 = (((((in_var_POSITION * CustomBuffer.BoneMatrixArray[uint(_38[_42])]) * _37[_42]) + ((in_var_POSITION * CustomBuffer.BoneMatrixArray[uint(_38[_52])]) * _37[_52])) + ((in_var_POSITION * CustomBuffer.BoneMatrixArray[uint(_38[_63])]) * _37[_63])) + ((in_var_POSITION * CustomBuffer.BoneMatrixArray[uint(_38[_74])]) * _37[_74])) * CustomBuffer.LIGHT_MATRIX_MVP;
    gl_Position = _87;
    out_var_TEXCOORD0 = _87.zw;
}

