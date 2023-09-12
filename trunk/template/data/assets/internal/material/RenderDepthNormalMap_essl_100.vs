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
attribute vec3 in_var_NORMAL;
varying float varying_TEXCOORD0;
varying vec3 varying_TEXCOORD1;

void main()
{
    gl_Position = in_var_POSITION * TransformBuffer.MATRIX_MVP;
    varying_TEXCOORD0 = (in_var_POSITION * TransformBuffer.MATRIX_MV).z * CameraBuffer._ProjectionParams.w;
    varying_TEXCOORD1 = normalize(in_var_NORMAL * mat3(TransformBuffer.MATRIX_IT_MV[0].xyz, TransformBuffer.MATRIX_IT_MV[1].xyz, TransformBuffer.MATRIX_IT_MV[2].xyz));
}

