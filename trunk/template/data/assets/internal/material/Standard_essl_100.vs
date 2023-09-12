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

struct type_CameraBuffer
{
    mat4 MATRIX_V;
    mat4 MATRIX_P;
    mat4 MATRIX_VP;
    mat4 MATRIX_I_VP;
    vec3 _WorldSpaceCameraPos;
    vec3 _WorldSpaceCameraDir;
    vec4 _ProjectionParams;
    vec4 _ScreenParams;
    vec4 _ZBufferParams;
};

uniform type_CameraBuffer CameraBuffer;

struct type_CustomBuffer
{
    vec4 _MainTex_ST;
    float _SpecularPower;
    float _Reflections;
};

uniform type_CustomBuffer CustomBuffer;

attribute vec4 in_var_POSITION;
attribute vec3 in_var_NORMAL;
attribute vec4 in_var_TANGENT;
attribute vec2 in_var_TEXCOORD0;
varying vec2 varying_TEXCOORD0;
varying mat3 varying_TEXCOORD1;
varying vec3 varying_TEXCOORD4;
varying vec3 varying_TEXCOORD5;
varying vec3 varying_TEXCOORD6;
varying float varying_TEXCOORD7;

void main()
{
    vec4 _54 = in_var_POSITION * TransformBuffer.MATRIX_MVP;
    vec3 _61 = normalize(in_var_NORMAL);
    vec3 _63 = normalize(in_var_TANGENT.xyz);
    vec3 _71 = (in_var_POSITION * TransformBuffer.MATRIX_M).xyz;
    gl_Position = _54;
    varying_TEXCOORD0 = (in_var_TEXCOORD0 * CustomBuffer._MainTex_ST.xy) + CustomBuffer._MainTex_ST.zw;
    varying_TEXCOORD1 = mat3(_63, cross(_61, _63) * in_var_TANGENT.w, _61);
    varying_TEXCOORD4 = _71;
    varying_TEXCOORD5 = normalize(CameraBuffer._WorldSpaceCameraPos - _71);
    varying_TEXCOORD6 = normalize(_61 * mat3(TransformBuffer.MATRIX_IT_M[0].xyz, TransformBuffer.MATRIX_IT_M[1].xyz, TransformBuffer.MATRIX_IT_M[2].xyz));
    varying_TEXCOORD7 = _54.z;
}

