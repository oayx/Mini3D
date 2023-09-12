#version 100

attribute vec3 in_var_POSITION;
attribute vec2 in_var_TEXCOORD0;
varying vec2 varying_TEXCOORD0;

void main()
{
    vec2 _26 = in_var_TEXCOORD0;
    _26.y = 1.0 - in_var_TEXCOORD0.y;
    gl_Position = vec4(in_var_POSITION, 1.0);
    varying_TEXCOORD0 = _26;
}

