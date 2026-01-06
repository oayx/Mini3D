#version 300 es
#extension GL_EXT_geometry_shader : require
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

in vec4 var_POSITION[1];
in vec2 var_SIZE[1];
out vec2 var_TEXCOORD0;

void main()
{
    vec3 _48 = normalize(CameraBuffer._WorldSpaceCameraPos - var_POSITION[0].xyz);
    _48.y = 0.0;
    vec3 _55 = cross(vec3(0.0, 1.0, 0.0), normalize(_48)) * (0.5 * var_SIZE[0].x);
    vec3 _56 = var_POSITION[0].xyz + _55;
    vec3 _57 = vec3(0.0, 1.0, 0.0) * (0.5 * var_SIZE[0].y);
    vec3 _68 = var_POSITION[0].xyz - _55;
    gl_Position = vec4(_56 - _57, 1.0) * CameraBuffer.MATRIX_VP;
    var_TEXCOORD0 = vec2(0.0, 1.0);
    EmitVertex();
    gl_Position = vec4(_56 + _57, 1.0) * CameraBuffer.MATRIX_VP;
    var_TEXCOORD0 = vec2(0.0);
    EmitVertex();
    gl_Position = vec4(_68 - _57, 1.0) * CameraBuffer.MATRIX_VP;
    var_TEXCOORD0 = vec2(1.0);
    EmitVertex();
    gl_Position = vec4(_68 + _57, 1.0) * CameraBuffer.MATRIX_VP;
    var_TEXCOORD0 = vec2(1.0, 0.0);
    EmitVertex();
}

