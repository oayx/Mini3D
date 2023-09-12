#version 100

struct type_TransformBuffer
{
    mat4 MATRIX_MVP;
    mat4 MATRIX_M;
    mat4 MATRIX_MV;
    mat4 MATRIX_I_M;
    mat4 MATRIX_T_M;
    mat4 MATRIX_T_MV;
    mat4 MATRIX_IT_M;
    mat4 MATRIX_IT_MV;
};

uniform type_TransformBuffer TransformBuffer;

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

attribute vec4 in_var_POSITION;
attribute vec4 in_var_COLOR0;
attribute vec2 in_var_TEXCOORD0;
attribute mat4 in_var_POSITION1;
varying float varying_TEXCOORD0;

void main()
{
    vec4 _36 = in_var_POSITION * in_var_POSITION1;
    gl_Position = _36 * TransformBuffer.MATRIX_MVP;
    varying_TEXCOORD0 = (_36 * TransformBuffer.MATRIX_MV).z * CameraBuffer._ProjectionParams.w;
}

