#version 460

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

layout(location = 0) in vec4 in_var_POSITION;
layout(location = 1) in vec3 in_var_NORMAL;
layout(location = 2) in vec4 in_var_TANGENT;
layout(location = 3) in vec2 in_var_TEXCOORD0;
layout(location = 0) out vec2 var_TEXCOORD0;
layout(location = 1) out mat3 var_TEXCOORD1;
layout(location = 4) out vec3 var_TEXCOORD4;
layout(location = 5) out vec3 var_TEXCOORD5;
layout(location = 6) out vec3 var_TEXCOORD6;
layout(location = 7) out float var_TEXCOORD7;

void main()
{
    vec4 _54 = in_var_POSITION * TransformBuffer.MATRIX_MVP;
    vec3 _61 = normalize(in_var_NORMAL);
    vec3 _63 = normalize(in_var_TANGENT.xyz);
    vec3 _71 = (in_var_POSITION * TransformBuffer.MATRIX_M).xyz;
    gl_Position = _54;
    var_TEXCOORD0 = (in_var_TEXCOORD0 * CustomBuffer._MainTex_ST.xy) + CustomBuffer._MainTex_ST.zw;
    var_TEXCOORD1 = mat3(_63, cross(_61, _63) * in_var_TANGENT.w, _61);
    var_TEXCOORD4 = _71;
    var_TEXCOORD5 = normalize(CameraBuffer._WorldSpaceCameraPos - _71);
    var_TEXCOORD6 = normalize(_61 * mat3(TransformBuffer.MATRIX_IT_M[0].xyz, TransformBuffer.MATRIX_IT_M[1].xyz, TransformBuffer.MATRIX_IT_M[2].xyz));
    var_TEXCOORD7 = _54.z;
}

