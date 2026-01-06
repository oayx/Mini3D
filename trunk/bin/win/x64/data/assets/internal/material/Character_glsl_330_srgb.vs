#version 330
#extension GL_ARB_separate_shader_objects : require

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
layout(location = 0) out vec2 var_TEXCOORD0;
layout(location = 1) out float var_TEXCOORD1;

void main()
{
    vec4 _82 = vec4((((((in_var_POSITION * CustomBuffer.BoneMatrixArray[int(in_var_BLENDINDICES.x)]) * in_var_BLENDWEIGHT.x) + ((in_var_POSITION * CustomBuffer.BoneMatrixArray[int(in_var_BLENDINDICES.y)]) * in_var_BLENDWEIGHT.y)) + ((in_var_POSITION * CustomBuffer.BoneMatrixArray[int(in_var_BLENDINDICES.z)]) * in_var_BLENDWEIGHT.z)) + ((in_var_POSITION * CustomBuffer.BoneMatrixArray[int(in_var_BLENDINDICES.w)]) * in_var_BLENDWEIGHT.w)).xyz, 1.0) * TransformBuffer.MATRIX_MVP;
    gl_Position = _82;
    var_TEXCOORD0 = (in_var_TEXCOORD0 * CustomBuffer._MainTex_ST.xy) + CustomBuffer._MainTex_ST.zw;
    var_TEXCOORD1 = _82.z;
}

