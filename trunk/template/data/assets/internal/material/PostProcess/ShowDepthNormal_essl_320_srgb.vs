#version 320 es

layout(location = 0) in vec3 in_var_POSITION;
layout(location = 1) in vec2 in_var_TEXCOORD0;
layout(location = 0) out vec2 var_TEXCOORD0;

void main()
{
    gl_Position = vec4(in_var_POSITION, 1.0);
    var_TEXCOORD0 = in_var_TEXCOORD0;
}

