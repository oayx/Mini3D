#include "CGInclude.hlsli"

BEGIN_CONSTANTS(CustomBuffer, 0)
	float _Exposure;
END_CONSTANTS
DECLARE_TEXTURE(_MainTex, 0);

struct v2f
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
};
v2f vs_main( float3 Pos : POSITION, float2 uv : TEXCOORD0)
{
	v2f o;
	o.pos = float4(Pos, 1);
	o.uv = uv;
	return o;
}
float4 ps_main(v2f i) : SV_Target
{
	const float A = 2.51f;
	const float B = 0.03f;
	const float C = 2.43f;
	const float D = 0.59f;
	const float E = 0.14f;
	
	float4 hdr_color = SAMPLE_TEXTURE(_MainTex, i.uv);
	// 1.Reinhard
	//hdr_color.rgb = hdr_color.rgb / (hdr_color.rgb + float3(1.0, 1.0, 1.0));
	// 2.Cryengine
	//hdr_color.rgb = float3(1.0, 1.0, 1.0) - exp(-hdr_color.rgb * _Exposure);
	// 3.Academy Color Encoding System（ACES）
	hdr_color.rgb *= _Exposure;
	hdr_color.rgb = (hdr_color.rgb * (A * hdr_color.rgb + B)) / (hdr_color.rgb * (C * hdr_color.rgb + D) + E);
	return hdr_color;
}