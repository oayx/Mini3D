#include "CGInclude.hlsli"

BEGIN_CONSTANTS(CustomBuffer, 0)
	matrix MATRIX_SKYBOX;
END_CONSTANTS

DECLARE_CUBEMAP(_MainTex, 0);

struct v2f
{
	float4 pos	: SV_POSITION;
	float3 uv   : TEXCOORD0;
};
v2f vs_main(float4 Pos : POSITION)
{
	v2f o;
	float4 pos = mul(MATRIX_SKYBOX, float4(Pos.xyz, 1));
	o.pos = pos.xyww;// 设置z = w使得z/w = 1(天空盒保持在远平面，这样设置后，天空盒可以最后渲染，利用前置深度测试，减少ps执行(大部分情况下会测试失败，因为被场景物体挡住了))
	o.uv = Pos.xyz;
	return o;
}
float4 ps_main(v2f i) : SV_Target
{
	float4 final_col = SAMPLE_CUBEMAP(_MainTex, i.uv);
	return LinearToGammaSpace(final_col);
}