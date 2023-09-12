#version 100

struct type_CustomBuffer
{
    vec4 _MainTex_TexelSize;
    vec4 _RcpFrame;
    vec4 _RcpFrameOpt;
};

uniform type_CustomBuffer CustomBuffer;

attribute vec3 in_var_POSITION;
attribute vec2 in_var_TEXCOORD0;
varying vec2 varying_TEXCOORD0;
varying vec4 varying_TEXCOORD1;

void main()
{
    vec2 _35 = in_var_TEXCOORD0;
    _35.y = 1.0 - in_var_TEXCOORD0.y;
    vec2 _43 = _35 + (vec2(-CustomBuffer._MainTex_TexelSize.x, CustomBuffer._MainTex_TexelSize.y) * 0.5);
    vec2 _47 = _35 + (vec2(CustomBuffer._MainTex_TexelSize.x, -CustomBuffer._MainTex_TexelSize.y) * 0.5);
    gl_Position = vec4(in_var_POSITION, 1.0);
    varying_TEXCOORD0 = _35;
    varying_TEXCOORD1 = vec4(_43.x, _43.y, _47.x, _47.y);
}

