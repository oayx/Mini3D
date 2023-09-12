#include "CGInclude.hlsli"

BEGIN_CONSTANTS(CustomBuffer,0)
	float4 _MainTex_ST;
END_CONSTANTS

// 纹理采样器
DECLARE_TEXTURE(_MainTex, 0);
DECLARE_TEXTURE(_NormalTexture, 1);

struct v2f
{
    float4 pos				: SV_POSITION;
	float2 uv				: TEXCOORD0;
	float3 tangent_light_dir: TEXCOORD1;
	FOG_COORDS(2)
};

// 顶点渲染器主函数
v2f vs_main(appdata_tan v)
{
	v2f o;

	o.pos = mul(MATRIX_MVP,v.vertex);
    o.uv = TRANSFORM_TEX(v.uv, _MainTex);
	
	//计算切线空间
	float3x3 rotation = GetTangentSpace(v.normal, v.tangent);
	
	//灯光方向
	float3 obj_light_dir = ObjSpaceLightDir(v.vertex.xyz); //模型空间灯光方向
	o.tangent_light_dir = normalize(mul(rotation, obj_light_dir)); //转换到切线空间
	
	TRANSFER_FOG(o, o.pos);
    return o;
}
// 像素渲染器主函数
float4 ps_main(v2f i) : SV_Target
{ 
	float4 albedo = SAMPLE_TEXTURE(_MainTex, i.uv);
	float3 tangent_normal = UnpackNormal(SAMPLE_TEXTURE(_NormalTexture, i.uv));
	//根据兰伯特模型计算像素的光照信息  
	float3 lambert = dot(tangent_normal, i.tangent_light_dir) * 0.5 + 0.5;

	float3 ambient_col = _LIGHTMODEL_AMBIENT;
	float3 diffuse_col = _LightColor0 * lambert;
	float3 rgb_col = (ambient_col + diffuse_col) * albedo.rgb;
	float4 final_col = float4(rgb_col, albedo.a);

	//APPLY_FOG(i.fogCoord, final_col);
	return LinearToGammaSpace(final_col);
}