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
    float _SpecularPower;
};

uniform type_CustomBuffer CustomBuffer;

attribute vec4 in_var_POSITION;
attribute vec3 in_var_NORMAL;
attribute vec4 in_var_TANGENT;
attribute vec2 in_var_TEXCOORD0;
varying vec2 varying_TEXCOORD0;
varying vec3 varying_TEXCOORD1;
varying vec3 varying_TEXCOORD2;
varying vec3 varying_TEXCOORD3;
varying float varying_TEXCOORD4;

void main()
{
    vec4 _58 = in_var_POSITION * TransformBuffer.MATRIX_MVP;
    vec3 _68 = (in_var_POSITION * TransformBuffer.MATRIX_M).xyz;
    vec3 _93;
    for (int SPIRV_Cross_Dummy210 = 0; SPIRV_Cross_Dummy210 < 1; SPIRV_Cross_Dummy210++)
    {
        if (LightBuffer._Light[0].Type == 0)
        {
            _93 = -LightBuffer._LightDir0;
            break;
        }
        else
        {
            _93 = normalize(LightBuffer._Light[0].Position - _68);
            break;
        }
    }
    gl_Position = _58;
    varying_TEXCOORD0 = (in_var_TEXCOORD0 * CustomBuffer._MainTex_ST.xy) + CustomBuffer._MainTex_ST.zw;
    varying_TEXCOORD1 = _68;
    varying_TEXCOORD2 = normalize(in_var_NORMAL * mat3(TransformBuffer.MATRIX_M[0].xyz, TransformBuffer.MATRIX_M[1].xyz, TransformBuffer.MATRIX_M[2].xyz));
    varying_TEXCOORD3 = _93;
    varying_TEXCOORD4 = _58.z;
}

