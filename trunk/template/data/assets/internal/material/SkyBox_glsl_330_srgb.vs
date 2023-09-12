#version 330
#extension GL_ARB_separate_shader_objects : require

out gl_PerVertex
{
    vec4 gl_Position;
};

struct type_CustomBuffer
{
    mat4 MATRIX_SKYBOX;
};

uniform type_CustomBuffer CustomBuffer;

layout(location = 0) in vec4 in_var_POSITION;
layout(location = 0) out vec3 out_var_TEXCOORD0;

void main()
{
    gl_Position = (vec4(in_var_POSITION.xyz, 1.0) * CustomBuffer.MATRIX_SKYBOX).xyww;
    out_var_TEXCOORD0 = in_var_POSITION.xyz;
}

