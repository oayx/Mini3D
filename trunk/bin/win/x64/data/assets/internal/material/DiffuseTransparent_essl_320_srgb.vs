#version 320 es

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

layout(location = 0) in vec4 in_var_POSITION;
layout(location = 1) in vec3 in_var_NORMAL;
layout(location = 2) in vec4 in_var_TANGENT;
layout(location = 3) in vec2 in_var_TEXCOORD0;
layout(location = 0) out vec2 var_TEXCOORD0;
layout(location = 1) out vec3 var_TEXCOORD1;
layout(location = 2) out vec3 var_TEXCOORD2;
layout(location = 3) out vec3 var_TEXCOORD3;
layout(location = 4) out float var_TEXCOORD4;

void main()
{
    vec4 _58 = in_var_POSITION * TransformBuffer.MATRIX_MVP;
    vec3 _68 = (in_var_POSITION * TransformBuffer.MATRIX_M).xyz;
    vec3 _93;
    do
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
    } while(false);
    gl_Position = _58;
    var_TEXCOORD0 = (in_var_TEXCOORD0 * CustomBuffer._MainTex_ST.xy) + CustomBuffer._MainTex_ST.zw;
    var_TEXCOORD1 = _68;
    var_TEXCOORD2 = normalize(in_var_NORMAL * mat3(TransformBuffer.MATRIX_M[0].xyz, TransformBuffer.MATRIX_M[1].xyz, TransformBuffer.MATRIX_M[2].xyz));
    var_TEXCOORD3 = _93;
    var_TEXCOORD4 = _58.z;
}

