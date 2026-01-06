#include "CGInclude.hlsli"

BEGIN_CONSTANTS(CustomBuffer, 0)
	float4 _MainTex0_ST;
	float4 _MainTex1_ST;
	float _SpecularPower = 0.5; //高光强度
END_CONSTANTS

// 纹理采样器
DECLARE_TEXTURE(_MainTex0, 0);
DECLARE_TEXTURE(_NormalTexture0, 1);
DECLARE_TEXTURE(_MainTex1, 2);
DECLARE_TEXTURE(_NormalTexture1, 3);
DECLARE_TEXTURE(_ControlTex, 4);

struct v2f
{
	float4 pos : SV_POSITION;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
	float2 uv_control : TEXCOORD2;
	float3x3 rotation : TEXCOORD3;
	float3 world_light_dir : TEXCOORD6;
	float3 world_view_dir : TEXCOORD7;
	FOG_COORDS(8)
};

// 顶点渲染器主函数
v2f vs_main(appdata_tan v)
{
	v2f o;

	o.pos = mul(MATRIX_MVP, v.vertex);
	o.uv0 = TRANSFORM_TEX(v.uv, _MainTex0);
	o.uv1 = TRANSFORM_TEX(v.uv, _MainTex1);
	o.uv_control = v.uv;
	
	//计算切线空间
	o.rotation = GetTangentSpace(v.normal, v.tangent);
	//灯光方向
	float3 world_pos = mul(MATRIX_M, v.vertex).xyz;
	o.world_light_dir = WorldSpaceLightDir(world_pos);
	o.world_view_dir = WorldSpaceViewDir(world_pos);

	TRANSFER_FOG(o, o.pos);
	return o;
}
// 像素渲染器主函数
float4 ps_main(v2f i) : SV_Target
{
	float4 layer0 = SAMPLE_TEXTURE(_MainTex0, i.uv0);
	float4 layer1 = SAMPLE_TEXTURE(_MainTex1, i.uv1);
	float4 layer_control = SAMPLE_TEXTURE(_ControlTex, i.uv_control);
	float4 albedo = float4(layer0.rgb * layer_control.r + layer1.rgb * layer_control.g, 1.0);
	
	float3 normal0_w = UnpackNormal(SAMPLE_TEXTURE(_NormalTexture0, i.uv0));
	float3 normal1_w = UnpackNormal(SAMPLE_TEXTURE(_NormalTexture1, i.uv1));
	normal0_w = normalize(mul(normal0_w, i.rotation));
	normal1_w = normalize(mul(normal1_w, i.rotation));
	float3 world_normal = normal0_w * layer_control.r + normal1_w * layer_control.g;
	
	//根据兰伯特模型计算像素的光照信息  
	float3 lambert = dot(world_normal, i.world_light_dir);
	
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