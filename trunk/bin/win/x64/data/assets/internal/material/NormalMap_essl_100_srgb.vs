#version 100

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

attribute vec4 in_var_POSITION;
attribute vec3 in_var_NORMAL;
attribute vec4 in_var_TANGENT;
attribute vec2 in_var_TEXCOORD0;
varying vec2 varying_TEXCOORD0;
varying vec3 varying_TEXCOORD1;
varying float varying_TEXCOORD2;

void main()
{
    vec4 _53 = in_var_POSITION * TransformBuffer.MATRIX_MVP;
    vec3 _60 = normalize(in_var_NORMAL);
    vec3 _62 = normalize(in_var_TANGENT.xyz);
    gl_Position = _53;
    varying_TEXCOORD0 = (in_var_TEXCOORD0 * CustomBuffer._MainTex_ST.xy) + CustomBuffer._MainTex_ST.zw;
    varying_TEXCOORD1 = normalize(normalize((-LightBuffer._LightDir0) * mat3(TransformBuffer.MATRIX_I_M[0].xyz, TransformBuffer.MATRIX_I_M[1].xyz, TransformBuffer.MATRIX_I_M[2].xyz)) * mat3(_62, cross(_60, _62) * in_var_TANGENT.w, _60));
    varying_TEXCOORD2 = _53.z;
}

