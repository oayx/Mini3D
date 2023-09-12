#include "CGInclude.hlsli"

BEGIN_CONSTANTS(CustomBuffer, 0)
	matrix MATRIX_MVP_LIGHT;
	float4 _MainTex_ST;
	float _SpecularPower = 0.5; //高光强度
	float4 _ShadowMap_TexelSize;
	float _Strength;
	float _Bias;
	int _ShadowType;
END_CONSTANTS

// 纹理采样器
DECLARE_TEXTURE(_MainTex, 0);
DECLARE_TEXTURE(_NormalTexture, 1);
DECLARE_TEXTURE(_ShadowMap, 2);

struct v2f
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
	float3x3 rotation : TEXCOORD1;
	float3 world_view_dir : TEXCOORD4;
	float3 world_light_dir : TEXCOORD5;
	float4 light_space_pos : TEXCOORD6;
	FOG_COORDS(7)
};

// 顶点渲染器主函数
v2f vs_main(appdata_tan v)
{
	v2f o;

	o.pos = mul(MATRIX_MVP, v.vertex);
	o.uv = TRANSFORM_TEX(v.uv, _MainTex);

	//计算切线空间
	o.rotation = GetTangentSpace(v.normal, v.tangent);
	
	//观察方向转换到世界空间
	float3 world_pos = mul(MATRIX_M, v.vertex).xyz;
	o.world_view_dir = WorldSpaceViewDir(world_pos);
	//灯光方向
	o.world_light_dir = WorldSpaceLightDir(world_pos);

	//在灯光投影坐标系
	o.light_space_pos = mul(MATRIX_MVP_LIGHT, v.vertex);

	TRANSFER_FOG(o, o.pos);
	return o;
}
// PCF滤波(bias设置偏移，防止采样不足导致的阴影粉刺)
float PercentCloaerFilter(float2 uv, float scene_depth, float bias)
{
	float shadow = 0.0;
	for (int x = -FilterSize; x <= FilterSize; ++x)
	{
		for (int y = -FilterSize; y <= FilterSize; ++y)
		{
			float2 uv_offset = float2(x, y) * _ShadowMap_TexelSize.xy;
			float depth = SAMPLE_TEXTURE(_ShadowMap, uv + uv_offset).r;
			shadow += (scene_depth - bias > depth ? 1.0 : 0.0);
		}
	}
	float total = (FilterSize * 2 + 1) * (FilterSize * 2 + 1);
	shadow /= total;
	return shadow;
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

	float shadow = 0;
	if (_ShadowType != 0)
	{
		//NDC空间坐标(-1,1)转UV坐标(0,1)
		float4 light_space_pos = i.light_space_pos;
		float2 shadow_uv = 0.5 * (light_space_pos.xy / light_space_pos.w) + float2(0.5, 0.5);
#if defined(SHADER_API_DIRECTX)
		//DX纹理坐标在Y方向是反的
		shadow_uv.y = 1 - shadow_uv.y;

		//场景在灯光坐标系下的深度
		float light_depth = light_space_pos.z / light_space_pos.w;
#else
		shadow_uv.y = shadow_uv.y;

		//场景在灯光坐标系下的深度(OpenGL NDC默认z是(-1,1),这里z转换到0-1)
		float light_depth = 0.5 * (light_space_pos.z / light_space_pos.w) + 0.5;
#endif

		//使用PCF对边缘过滤
		if (_ShadowType == 2)
			shadow = PercentCloaerFilter(shadow_uv, light_depth, _Bias);
		else if (_ShadowType == 1)
		{
			float depth = SAMPLE_TEXTURE(_ShadowMap, shadow_uv).r;
			shadow = (light_depth - _Bias > depth ? 1.0 : 0.0);
		}
	}
	final_col.rgb = final_col.rgb * (1 - shadow * _Strength);

	APPLY_FOG(i.fogCoord, final_col);
	return LinearToGammaSpace(final_col);
}