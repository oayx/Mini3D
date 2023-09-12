#version 330
#extension GL_ARB_separate_shader_objects : require

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) in vec3 in_var_POSITION;
layout(location = 1) in vec2 in_var_TEXCOORD0;
layout(location = 0) out vec2 out_var_TEXCOORD0;

void main()
{
    vec2 _26 = in_var_TEXCOORD0;
    _26.y = 1.0 - in_var_TEXCOORD0.y;
    gl_Position = vec4(in_var_POSITION, 1.0);
    out_var_TEXCOORD0 = _26;
}

