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
    mat4 MATRIX_MVP_LIGHT;
    vec4 _MainTex_ST;
    float _SpecularPower;
    vec4 _ShadowMap_TexelSize;
    float _Strength;
    float _Bias;
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
varying vec4 varying_TEXCOORD6;
varying float varying_TEXCOORD7;

void main()
{
    vec4 _65 = in_var_POSITION * TransformBuffer.MATRIX_MVP;
    vec3 _72 = normalize(in_var_NORMAL);
    vec3 _74 = normalize(in_var_TANGENT.xyz);
    vec3 _82 = (in_var_POSITION * TransformBuffer.MATRIX_M).xyz;
    vec3 _102;
    for (int SPIRV_Cross_Dummy307 = 0; SPIRV_Cross_Dummy307 < 1; SPIRV_Cross_Dummy307++)
    {
        if (LightBuffer._Light[0].Type == 0)
        {
            _102 = -LightBuffer._LightDir0;
            break;
        }
        else
        {
            _102 = normalize(LightBuffer._Light[0].Position - _82);
            break;
        }
    }
    gl_Position = _65;
    varying_TEXCOORD0 = (in_var_TEXCOORD0 * CustomBuffer._MainTex_ST.xy) + CustomBuffer._MainTex_ST.zw;
    varying_TEXCOORD1 = mat3(_74, cross(_72, _74) * in_var_TANGENT.w, _72);
    varying_TEXCOORD4 = normalize(CameraBuffer._WorldSpaceCameraPos - _82);
    varying_TEXCOORD5 = _102;
    varying_TEXCOORD6 = in_var_POSITION * CustomBuffer.MATRIX_MVP_LIGHT;
    varying_TEXCOORD7 = _65.z;
}

