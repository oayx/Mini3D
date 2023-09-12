#version 300 es

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
    vec4 _MainTex_ST;
    float _SpecularPower;
};

uniform type_CustomBuffer CustomBuffer;

layout(location = 0) in vec4 in_var_POSITION;
layout(location = 1) in vec3 in_var_NORMAL;
layout(location = 2) in vec4 in_var_TANGENT;
layout(location = 3) in vec2 in_var_TEXCOORD0;
out vec2 varying_TEXCOORD0;
out mat3 varying_TEXCOORD1;
out vec3 varying_TEXCOORD4;
out vec3 varying_TEXCOORD5;
out float varying_TEXCOORD6;

void main()
{
    vec4 _64 = in_var_POSITION * TransformBuffer.MATRIX_MVP;
    vec3 _71 = normalize(in_var_NORMAL);
    vec3 _73 = normalize(in_var_TANGENT.xyz);
    vec3 _81 = (in_var_POSITION * TransformBuffer.MATRIX_M).xyz;
    vec3 _101;
    do
    {
        if (LightBuffer._Light[0].Type == 0)
        {
            _101 = -LightBuffer._LightDir0;
            break;
        }
        else
        {
            _101 = normalize(LightBuffer._Light[0].Position - _81);
            break;
        }
    } while(false);
    gl_Position = _64;
    varying_TEXCOORD0 = (in_var_TEXCOORD0 * CustomBuffer._MainTex_ST.xy) + CustomBuffer._MainTex_ST.zw;
    varying_TEXCOORD1 = mat3(_73, cross(_71, _73) * in_var_TANGENT.w, _71);
    varying_TEXCOORD4 = normalize(CameraBuffer._WorldSpaceCameraPos - _81);
    varying_TEXCOORD5 = _101;
    varying_TEXCOORD6 = _64.z;
}

