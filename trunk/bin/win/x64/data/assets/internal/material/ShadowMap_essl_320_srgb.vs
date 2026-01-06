#version 320 es

struct type_CustomBuffer
{
    mat4 LIGHT_MATRIX_MVP;
};

uniform type_CustomBuffer CustomBuffer;

layout(location = 0) in vec4 in_var_POSITION;
layout(location = 0) out vec2 var_TEXCOORD0;

void main()
{
    vec4 _24 = in_var_POSITION * CustomBuffer.LIGHT_MATRIX_MVP;
    gl_Position = _24;
    var_TEXCOORD0 = _24.zw;
}

