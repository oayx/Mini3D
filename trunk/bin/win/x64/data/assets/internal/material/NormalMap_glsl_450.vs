#version 450

out gl_PerVertex
{
    vec4 gl_Position;
};

struct Light
{
    vec3 Dir;
    int Type;
    vec3 Color;
    vec3 Attenuation;
    float Range;
    vec3 Position;
    float SpotAngle;
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

struct type_LightBuffer
{
    int _LightCount;
    vec3 _LightDir0;
    vec3 _LightColor0;
    Light _Light[8];
};

uniform type_LightBuffer LightBuffer;

struct type_CustomBuffer
{
    vec4 _MainTex_ST;
};

uniform type_CustomBuffer CustomBuffer;

layout(location = 0) in vec4 in_var_POSITION;
layout(location = 1) in vec3 in_var_NORMAL;
layout(location = 2) in vec4 in_var_TANGENT;
layout(location = 3) in vec2 in_var_TEXCOORD0;
layout(location = 0) out vec2 var_TEXCOORD0;
layout(location = 1) out vec3 var_TEXCOORD1;
layout(location = 2) out float var_TEXCOORD2;

void main()
{
    vec4 _53 = in_var_POSITION * TransformBuffer.MATRIX_MVP;
    vec3 _60 = normalize(in_var_NORMAL);
    vec3 _62 = normalize(in_var_TANGENT.xyz);
    gl_Position = _53;
    var_TEXCOORD0 = (in_var_TEXCOORD0 * CustomBuffer._MainTex_ST.xy) + CustomBuffer._MainTex_ST.zw;
    var_TEXCOORD1 = normalize(normalize((-LightBuffer._LightDir0) * mat3(TransformBuffer.MATRIX_I_M[0].xyz, TransformBuffer.MATRIX_I_M[1].xyz, TransformBuffer.MATRIX_I_M[2].xyz)) * mat3(_62, cross(_60, _62) * in_var_TANGENT.w, _60));
    var_TEXCOORD2 = _53.z;
}

