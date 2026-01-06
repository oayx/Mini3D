#version 100

struct type_CustomBuffer
{
    mat4 MATRIX_SKYBOX;
};

uniform type_CustomBuffer CustomBuffer;

attribute vec4 in_var_POSITION;
varying vec3 varying_TEXCOORD0;

void main()
{
    gl_Position = (vec4(in_var_POSITION.xyz, 1.0) * CustomBuffer.MATRIX_SKYBOX).xyww;
    varying_TEXCOORD0 = in_var_POSITION.xyz;
}

