#version 320 es

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

layout(location = 0) in vec4 in_var_POSITION;
layout(location = 1) in vec3 in_var_NORMAL;
layout(location = 0) out float out_var_TEXCOORD0;
layout(location = 1) out vec3 out_var_TEXCOORD1;

void main()
{
    gl_Position = in_var_POSITION * TransformBuffer.MATRIX_MVP;
    out_var_TEXCOORD0 = (in_var_POSITION * TransformBuffer.MATRIX_MV).z * CameraBuffer._ProjectionParams.w;
    out_var_TEXCOORD1 = normalize(in_var_NORMAL * mat3(TransformBuffer.MATRIX_IT_MV[0].xyz, TransformBuffer.MATRIX_IT_MV[1].xyz, TransformBuffer.MATRIX_IT_MV[2].xyz));
}

