#version 450

out gl_PerVertex
{
    vec4 gl_Position;
};

struct type_CustomBuffer
{
    vec4 _MainTex_TexelSize;
    vec4 _BlurOffsets;
    float _FocalDistance;
    float _FocalLength;
    float _NearBlurScale;
    float _FarBlurScale;
};

uniform type_CustomBuffer CustomBuffer;

layout(location = 0) in vec3 in_var_POSITION;
layout(location = 1) in vec2 in_var_TEXCOORD0;
layout(location = 0) out vec2 var_TEXCOORD0;
layout(location = 1) out vec4 var_TEXCOORD1;
layout(location = 2) out vec4 var_TEXCOORD2;
layout(location = 3) out vec4 var_TEXCOORD3;

void main()
{
    vec4 _46 = (CustomBuffer._BlurOffsets * CustomBuffer._MainTex_TexelSize.xyxy).xyxy * vec4(1.0, 1.0, -1.0, -1.0);
    gl_Position = vec4(in_var_POSITION, 1.0);
    var_TEXCOORD0 = in_var_TEXCOORD0;
    var_TEXCOORD1 = in_var_TEXCOORD0.xyxy + _46;
    var_TEXCOORD2 = in_var_TEXCOORD0.xyxy + (_46 * 2.0);
    var_TEXCOORD3 = in_var_TEXCOORD0.xyxy + (_46 * 3.0);
}

