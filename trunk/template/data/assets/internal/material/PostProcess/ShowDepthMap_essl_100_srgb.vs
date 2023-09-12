#version 100

attribute vec3 in_var_POSITION;
attribute vec2 in_var_TEXCOORD0;
varying vec2 varying_TEXCOORD0;

void main()
{
    gl_Position = vec4(in_var_POSITION, 1.0);
    varying_TEXCOORD0 = in_var_TEXCOORD0;
}

