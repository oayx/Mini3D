#version 100

struct type_CustomBuffer
{
    vec4 _MainTex_TexelSize;
    vec4 _EdgeColor;
};

uniform type_CustomBuffer CustomBuffer;

attribute vec3 in_var_POSITION;
attribute vec2 in_var_TEXCOORD0;
varying vec2 varying_TEXCOORD0[9];

void main()
{
    gl_Position = vec4(in_var_POSITION, 1.0);
    varying_TEXCOORD0 = vec2[](in_var_TEXCOORD0 + (CustomBuffer._MainTex_TexelSize.xy * vec2(-1.0, 1.0)), in_var_TEXCOORD0 + (CustomBuffer._MainTex_TexelSize.xy * vec2(0.0, 1.0)), in_var_TEXCOORD0 + CustomBuffer._MainTex_TexelSize.xy, in_var_TEXCOORD0 + (CustomBuffer._MainTex_TexelSize.xy * vec2(-1.0, 0.0)), in_var_TEXCOORD0, in_var_TEXCOORD0 + (CustomBuffer._MainTex_TexelSize.xy * vec2(1.0, 0.0)), in_var_TEXCOORD0 + (CustomBuffer._MainTex_TexelSize.xy * vec2(-1.0)), in_var_TEXCOORD0 + (CustomBuffer._MainTex_TexelSize.xy * vec2(0.0, -1.0)), in_var_TEXCOORD0 + (CustomBuffer._MainTex_TexelSize.xy * vec2(1.0, -1.0)));
}

