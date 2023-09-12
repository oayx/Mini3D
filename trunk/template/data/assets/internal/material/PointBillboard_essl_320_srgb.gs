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
layout(location = 1) in vec2 in_var_SIZE[1];
layout(location = 0) out vec2 out_var_TEXCOORD0;

void main()
{
    vec3 _48 = normalize(CameraBuffer._WorldSpaceCameraPos - in_var_POSITION[0].xyz);
    _48.y = 0.0;
    vec3 _55 = cross(vec3(0.0, 1.0, 0.0), normalize(_48)) * (0.5 * in_var_SIZE[0].x);
    vec3 _56 = in_var_POSITION[0].xyz + _55;
    vec3 _57 = vec3(0.0, 1.0, 0.0) * (0.5 * in_var_SIZE[0].y);
    vec3 _68 = in_var_POSITION[0].xyz - _55;
    gl_Position = vec4(_56 - _57, 1.0) * CameraBuffer.MATRIX_VP;
    out_var_TEXCOORD0 = vec2(0.0, 1.0);
    EmitVertex();
    gl_Position = vec4(_56 + _57, 1.0) * CameraBuffer.MATRIX_VP;
    out_var_TEXCOORD0 = vec2(0.0);
    EmitVertex();
    gl_Position = vec4(_68 - _57, 1.0) * CameraBuffer.MATRIX_VP;
    out_var_TEXCOORD0 = vec2(1.0);
    EmitVertex();
    gl_Position = vec4(_68 + _57, 1.0) * CameraBuffer.MATRIX_VP;
    out_var_TEXCOORD0 = vec2(1.0, 0.0);
    EmitVertex();
}

