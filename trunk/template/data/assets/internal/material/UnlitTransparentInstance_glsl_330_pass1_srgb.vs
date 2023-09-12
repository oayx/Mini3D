#version 330
#extension GL_ARB_separate_shader_objects : require

out gl_PerVertex
{
    vec4 gl_Position;
};

struct type_CustomBuffer
{
    mat4 LIGHT_MATRIX_MVP;
    vec4 _MainTex_ST;
    vec4 _Color;
};

uniform type_CustomBuffer CustomBuffer;

layout(location = 0) in vec4 in_var_POSITION;
layout(location = 1) in vec4 in_var_COLOR0;
layout(location = 2) in vec2 in_var_TEXCOORD0;
layout(location = 3) in mat4 in_var_POSITION1;
layout(location = 0) out vec2 out_var_TEXCOORD0;

void main()
{
    vec4 _31 = (in_var_POSITION * in_var_POSITION1) * CustomBuffer.LIGHT_MATRIX_MVP;
    gl_Position = _31;
    out_var_TEXCOORD0 = _31.zw;
}

