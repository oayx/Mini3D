#version 100

struct type_CustomBuffer
{
    vec4 _MainTex_TexelSize;
    vec4 _ColorThreshold;
    vec4 _BlurOffsets;
    vec4 _BloomColor;
    float _BloomFactor;
};

uniform type_CustomBuffer CustomBuffer;

attribute vec3 in_var_POSITION;
attribute vec2 in_var_TEXCOORD0;
varying vec2 varying_TEXCOORD0;
varying vec4 varying_TEXCOORD1;
varying vec4 varying_TEXCOORD2;
varying vec4 varying_TEXCOORD3;

void main()
{
    vec4 _46 = (CustomBuffer._BlurOffsets * CustomBuffer._MainTex_TexelSize.xyxy).xyxy * vec4(1.0, 1.0, -1.0, -1.0);
    gl_Position = vec4(in_var_POSITION, 1.0);
    varying_TEXCOORD0 = in_var_TEXCOORD0;
    varying_TEXCOORD1 = in_var_TEXCOORD0.xyxy + _46;
    varying_TEXCOORD2 = in_var_TEXCOORD0.xyxy + (_46 * 2.0);
    varying_TEXCOORD3 = in_var_TEXCOORD0.xyxy + (_46 * 3.0);
}

