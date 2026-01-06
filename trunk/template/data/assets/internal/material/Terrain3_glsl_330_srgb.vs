#version 330
#extension GL_ARB_separate_shader_objects : require

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
    vec4 _MainTex0_ST;
    vec4 _MainTex1_ST;
    vec4 _MainTex2_ST;
    float _SpecularPower;
};

uniform type_CustomBuffer CustomBuffer;

layout(location = 0) in vec4 in_var_POSITION;
layout(location = 1) in vec3 in_var_NORMAL;
layout(location = 2) in vec4 in_var_TANGENT;
layout(location = 3) in vec2 in_var_TEXCOORD0;
layout(location = 0) out vec2 var_TEXCOORD0;
layout(location = 1) out vec2 var_TEXCOORD1;
layout(location = 2) out vec2 var_TEXCOORD2;
layout(location = 3) out vec2 var_TEXCOORD3;
layout(location = 4) out mat3 var_TEXCOORD4;
layout(location = 7) out vec3 var_TEXCOORD7;
layout(location = 8) out vec3 var_TEXCOORD8;
layout(location = 9) out float var_TEXCOORD9;

void main()
{
    vec4 _68 = in_var_POSITION * TransformBuffer.MATRIX_MVP;
    vec3 _87 = normalize(in_var_NORMAL);
    vec3 _89 = normalize(in_var_TANGENT.xyz);
    vec3 _97 = (in_var_POSITION * TransformBuffer.MATRIX_M).xyz;
    vec3 _113;
    do
    {
        if (LightBuffer._Light[0].Type == 0)
        {
            _113 = -LightBuffer._LightDir0;
            break;
        }
        else
        {
            _113 = normalize(LightBuffer._Light[0].Position - _97);
            break;
        }
    } while(false);
    gl_Position = _68;
    var_TEXCOORD0 = (in_var_TEXCOORD0 * CustomBuffer._MainTex0_ST.xy) + CustomBuffer._MainTex0_ST.zw;
    var_TEXCOORD1 = (in_var_TEXCOORD0 * CustomBuffer._MainTex1_ST.xy) + CustomBuffer._MainTex1_ST.zw;
    var_TEXCOORD2 = (in_var_TEXCOORD0 * CustomBuffer._MainTex2_ST.xy) + CustomBuffer._MainTex2_ST.zw;
    var_TEXCOORD3 = in_var_TEXCOORD0;
    var_TEXCOORD4 = mat3(_89, cross(_87, _89) * in_var_TANGENT.w, _87);
    var_TEXCOORD7 = _113;
    var_TEXCOORD8 = normalize(CameraBuffer._WorldSpaceCameraPos - _97);
    var_TEXCOORD9 = _68.z;
}

