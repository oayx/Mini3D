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

struct type_TransformBuffer_1
{
    vec4 _MainTex_ST;
    vec4 _Time;
    vec4 _TintColor;
    float _USpeed;
    float _VSpeed;
};

uniform type_TransformBuffer_1 TransformBuffer_1;

attribute vec4 in_var_POSITION;
attribute vec4 in_var_COLOR;
attribute vec2 in_var_TEXCOORD0;
varying vec4 varying_COLOR;
varying vec2 varying_TEXCOORD0;
varying float varying_TEXCOORD1;

void main()
{
    vec4 _40 = in_var_POSITION * TransformBuffer.MATRIX_MVP;
    vec2 _48 = in_var_TEXCOORD0;
    _48.x = in_var_TEXCOORD0.x + (TransformBuffer_1._Time.y * TransformBuffer_1._USpeed);
    vec2 _54 = _48;
    _54.y = in_var_TEXCOORD0.y + (TransformBuffer_1._Time.y * TransformBuffer_1._VSpeed);
    gl_Position = _40;
    varying_COLOR = in_var_COLOR;
    varying_TEXCOORD0 = (_54 * TransformBuffer_1._MainTex_ST.xy) + TransformBuffer_1._MainTex_ST.zw;
    varying_TEXCOORD1 = _40.z;
}

