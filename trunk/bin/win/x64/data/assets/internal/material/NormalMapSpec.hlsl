#include "CGInclude.hlsli"

BEGIN_CONSTANTS(CustomBuffer, 0)
	float4 _MainTex_ST;
	float _SpecularPower = 0.5;//高光强度
END_CONSTANTS

// 纹理采样器
DECLARE_TEXTURE(_MainTex, 0);
DECLARE_TEXTURE(_NormalTexture, 1);

struct v2f
{
    float4 pos			: SV_POSITION;
	float2 uv			: TEXCOORD0;
	float3x3 rotation	: TEXCOORD1;
	float3 world_view_dir	: TEXCOORD4;
	float3 world_light_dir	: TEXCOORD5;
	FOG_COORDS(6)
};

// 顶点渲染器主函数
v2f vs_main(appdata_tan v)
{
	v2f o;

	o.pos = mul(MATRIX_MVP,v.vertex);
    o.uv = TRANSFORM_TEX(v.uv, _MainTex);

	//计算切线空间
	o.rotation = GetTangentSpace(v.normal, v.tangent);
	
	//观察方向转换到世界空间
	float3 world_pos = mul(MATRIX_M,v.vertex).xyz;
	o.world_view_dir = WorldSpaceViewDir(world_pos);
	//灯光方向
	o.world_light_dir = WorldSpaceLightDir(world_pos);

	TRANSFER_FOG(o, o.pos);
    return o;
}
// 像素渲染器主函数
float4 ps_main(v2f i) : SV_Target
{ 
	float4 albedo = SAMPLE_TEXTURE(_MainTex, i.uv);
	float3 world_normal = UnpackNormal(SAMPLE_TEXTURE(_NormalTexture, i.uv));
	world_normal = normalize(mul(world_normal, i.rotation));
	//根据兰伯特模型计算像素的光照信息  
	float3 lambert = max(0, dot(world_normal, i.world_light_dir));

	//镜面反射
	float3 half_dir = normalize(i.world_view_dir + i.world_light_dir); //半角向量
	float3 specu = pow(max(0, dot(half_dir, world_normal)), exp2(_SpecularPower * 10 + 1)); //计算镜面反射强度

	float3 ambient_col = _LIGHTMODEL_AMBIENT;
	float3 diffuse_col = _LightColor0 * lambert;
	float3 specu_col = specu;
	float3 rgb_col = (ambient_col + diffuse_col + specu_col) * albedo.rgb;
	float4 final_col = float4(rgb_col, albedo.a);

	APPLY_FOG(i.fogCoord, final_col);
	return LinearToGammaSpace(final_col);
}