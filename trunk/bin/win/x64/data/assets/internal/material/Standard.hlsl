#include "CGInclude.hlsli"

BEGIN_CONSTANTS(CustomBuffer, 0)
	float4 _MainTex_ST;
	float _SpecularPower = 0.5;	//高光强度
	float _Reflections = 0; //反射
END_CONSTANTS

// 纹理采样器
DECLARE_TEXTURE(_MainTex, 0);
DECLARE_TEXTURE(_NormalTexture, 1);
DECLARE_CUBEMAP(_SkyBox, 2);

struct v2f
{
    float4 pos			: SV_POSITION;
	float2 uv			: TEXCOORD0;
	float3x3 rotation	: TEXCOORD1;
	float3 world_pos	: TEXCOORD4;
	float3 world_view_dir : TEXCOORD5;
	float3 world_normal : TEXCOORD6;
	FOG_COORDS(7)
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
	o.world_pos = mul(MATRIX_M,v.vertex).xyz;
	o.world_view_dir = WorldSpaceViewDir(o.world_pos);
	o.world_normal = ObjectToWorldNormal(normalize(v.normal));

	TRANSFER_FOG(o, o.pos);
    return o;
}
// 像素渲染器主函数
float4 ps_main(v2f i) : SV_Target
{ 
	float4 albedo = SAMPLE_TEXTURE(_MainTex, i.uv);
	
	//世界空间下的法线坐标
	float3 world_normal = UnpackNormal(SAMPLE_TEXTURE(_NormalTexture, i.uv));
	world_normal = normalize(mul(world_normal, i.rotation));
	
	float3 ambient_col = _LIGHTMODEL_AMBIENT;
	float3 diffuse_col = float3(0,0,0);
	float3 specu_col = float3(0, 0, 0);
	
	//光照计算
    [unroll]
	for (int count = 0; count < _LightCount; ++count)
	{
		Light light = _Light[count];
		
		float atten = 1;//点光源衰减
		//灯光方向
		float3 world_light_dir = float3(0,0,0);

		if (light.Type == 0)//方向光
		{
			world_light_dir = -_LightDir0;
		}
		else if (light.Type == 1)//点光源
		{
			world_light_dir = light.Position - i.world_pos;
			
			//是否在有效范围
			float distance = length(world_light_dir);
			if (distance > light.Range)
				continue;
			
			//计算衰减
			atten = 1.0f / dot(light.Attenuation, float3(1.0f, distance, distance * distance));
		}
		else if (light.Type == 2)//聚光灯
		{
			world_light_dir = light.Position - i.world_pos;
			//聚光灯角度范围[0,180]，所以如果点积为负，就不可能照射到
			if (dot(world_light_dir, world_normal) <= 0)
				continue;
			
			//是否在有效范围
			float distance = length(world_light_dir);
			if (distance > light.Range)
				continue;
			
			//确定是否在角度范围内
			float radianCutoff = light.SpotAngle * Deg2Rad; //角度转弧度
			float cosThta = cos(radianCutoff);
			float currentCosThta = max(0.0, dot(-normalize(world_light_dir), light.Dir));
			if (currentCosThta <= cosThta)
				continue;
			
			//计算衰减
			float spot = pow(currentCosThta, 2);//衰减
			atten = spot / dot(light.Attenuation, float3(1.0f, distance, distance * distance));
		}
		//根据兰伯特模型计算像素的光照信息  
		world_light_dir = normalize(world_light_dir);
		float3 lambert = max(0, dot(world_normal, world_light_dir));

		//镜面反射
		float3 half_dir = normalize(i.world_view_dir + world_light_dir); //半角向量
		float3 specu = pow(max(0, dot(half_dir, world_normal)), exp2(_SpecularPower * 10 + 1)); //计算镜面反射强度
		
		diffuse_col += light.Color * lambert * atten;
		specu_col += specu * atten;
	}
	float3 rgb_col = (ambient_col + diffuse_col) * albedo.rgb;
	
	//反射
	if (_Reflections > 0)
	{
		float3 I = normalize(i.world_pos - _WorldSpaceCameraPos.xyz);
		float3 R = normalize(reflect(I, i.world_normal));
		float3 reflect_col = SAMPLE_CUBEMAP(_SkyBox, R).rgb;
		rgb_col = lerp(rgb_col, reflect_col, _Reflections);
	}
	
	float4 final_col = float4(rgb_col, albedo.a);
	APPLY_FOG(i.fogCoord, final_col);
	return LinearToGammaSpace(final_col);
}