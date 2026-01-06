#version 410

out gl_PerVertex
{
    vec4 gl_Position;
};

struct type_CustomBuffer
{
    mat4 ProjectionMatrix;
};

uniform type_CustomBuffer CustomBuffer;

layout(location = 0) in vec4 in_var_POSITION;
layout(location = 1) in vec4 in_var_COLOR0;
layout(location = 2) in vec2 in_var_TEXCOORD0;
layout(location = 0) out vec4 var_TEXCOORD0;
layout(location = 1) out vec2 var_TEXCOORD1;

void main()
{
    gl_Position = vec4(in_var_POSITION.xy, 0.0, 1.0) * CustomBuffer.ProjectionMatrix;
    var_TEXCOORD0 = in_var_COLOR0;
    var_TEXCOORD1 = in_var_TEXCOORD0;
}

