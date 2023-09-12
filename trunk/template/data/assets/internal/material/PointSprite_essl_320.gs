#version 320 es
layout(points) in;
layout(max_vertices = 4, triangle_strip) out;

struct type_CameraBuffer
{
    mat4 MATRIX_V;
    mat4 MATRIX_P;
    mat4 MATRIX_VP;
    mat4 MATRIX_I_VP;
    vec3 _WorldSpaceCameraPos;
    vec3 _WorldSpaceCameraDir;
    vec4 _ProjectionParams;
    vec4 _ScreenParams;
    vec4 _ZBufferParams;
};

uniform type_CameraBuffer CameraBuffer;

layout(location = 0) in vec4 in_var_POSITION[1];
layout(location = 1) in float in_var_SIZE[1];
layout(location = 0) out vec2 out_var_TEXCOORD0;

void main()
{
    float _40 = 0.5 * in_var_SIZE[0];
    float _41 = in_var_SIZE[0] * (-0.5);
    gl_Position = (in_var_POSITION[0] + vec4(_41, _41, 0.0, 0.0)) * CameraBuffer.MATRIX_VP;
    out_var_TEXCOORD0 = vec2(0.0, 1.0);
    EmitVertex();
    gl_Position = (in_var_POSITION[0] + vec4(_41, _40, 0.0, 0.0)) * CameraBuffer.MATRIX_VP;
    out_var_TEXCOORD0 = vec2(0.0);
    EmitVertex();
    gl_Position = (in_var_POSITION[0] + vec4(_40, _41, 0.0, 0.0)) * CameraBuffer.MATRIX_VP;
    out_var_TEXCOORD0 = vec2(1.0);
    EmitVertex();
    gl_Position = (in_var_POSITION[0] + vec4(_40, _40, 0.0, 0.0)) * CameraBuffer.MATRIX_VP;
    out_var_TEXCOORD0 = vec2(1.0, 0.0);
    EmitVertex();
}

