#version 330
#extension GL_ARB_separate_shader_objects : require

out gl_PerVertex
{
    vec4 gl_Position;
};

struct type_CustomBuffer
{
    mat4 LIGHT_MATRIX_MVP;
};

uniform type_CustomBuffer CustomBuffer;

layout(location = 0) in vec4 in_var_POSITION;
layout(location = 0) out vec2 out_var_TEXCOORD0;

void main()
{
    vec4 _24 = in_var_POSITION * CustomBuffer.LIGHT_MATRIX_MVP;
    gl_Position = _24;
    out_var_TEXCOORD0 = _24.zw;
}

