#version 450

out gl_PerVertex
{
    vec4 gl_Position;
};

struct type_CustomBuffer
{
    vec4 _MainTex_TexelSize;
    vec4 _EdgeColor;
};

uniform type_CustomBuffer CustomBuffer;

layout(location = 0) in vec3 in_var_POSITION;
layout(location = 1) in vec2 in_var_TEXCOORD0;
layout(location = 0) out vec2 out_var_TEXCOORD0[9];

void main()
{
    gl_Position = vec4(in_var_POSITION, 1.0);
    out_var_TEXCOORD0 = vec2[](in_var_TEXCOORD0 + (CustomBuffer._MainTex_TexelSize.xy * vec2(-1.0, 1.0)), in_var_TEXCOORD0 + (CustomBuffer._MainTex_TexelSize.xy * vec2(0.0, 1.0)), in_var_TEXCOORD0 + CustomBuffer._MainTex_TexelSize.xy, in_var_TEXCOORD0 + (CustomBuffer._MainTex_TexelSize.xy * vec2(-1.0, 0.0)), in_var_TEXCOORD0, in_var_TEXCOORD0 + (CustomBuffer._MainTex_TexelSize.xy * vec2(1.0, 0.0)), in_var_TEXCOORD0 + (CustomBuffer._MainTex_TexelSize.xy * vec2(-1.0)), in_var_TEXCOORD0 + (CustomBuffer._MainTex_TexelSize.xy * vec2(0.0, -1.0)), in_var_TEXCOORD0 + (CustomBuffer._MainTex_TexelSize.xy * vec2(1.0, -1.0)));
}

