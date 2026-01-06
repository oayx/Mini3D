#include "CGInclude.hlsli"

BEGIN_CONSTANTS(CustomBuffer, 0)
	float4 _MainTex_ST;
	float _SpecularPower = 0.5;//高光强度
END_CONSTANTS

// 纹理采样器
DECLARE_TEXTURE(_MainTex, 0);

struct v2f
{
    float4 pos			: SV_POSITION;
    float2 uv  			: TEXCOORD0;
	float3 world_pos	: TEXCOORD1;	//世界坐标下的位置
	float3 world_normal	: TEXCOORD2;	//世界坐标下的法线
	float3 world_light_dir	: TEXCOORD3;
	FOG_COORDS(4)
};
// 顶点渲染器主函数
v2f vs_main(appdata_tan v)
{
	v2f o;

    //顶点位置 
	o.pos = mul(MATRIX_MVP,v.vertex); //坐标变换
    //纹理
    o.uv = TRANSFORM_TEX(v.uv, _MainTex);

	float3 world_pos = mul(MATRIX_M,v.vertex).xyz;
	o.world_pos = world_pos;
	o.world_normal = normalize(mul((float3x3) MATRIX_M, v.normal).xyz); //计算法向量

	//灯光方向
	o.world_light_dir = WorldSpaceLightDir(world_pos);

	TRANSFER_FOG(o, o.pos);
    return o;
}
// 像素渲染器主函数
float4 ps_main(v2f i) : SV_Target
{ 
	float4 albedo = SAMPLE_TEXTURE(_MainTex, i.uv);
	
    //漫反射
	float3 world_light_dir = i.world_light_dir; //计算光的方向,由顶点指向灯源所在位置；由于是方向光，直接取反就行了
	float3 lambert = dot(i.world_normal, world_light_dir) * 0.5 + 0.5; //半兰伯特光照，将原来（-1，1）区间的光照条件转化到了（0，1）区间，既保证了结果的正确，又整体提升了亮度，保证非受光面也能有光，而不是全黑
	
	//镜面反射
	float3 world_view_dir = normalize(_WorldSpaceCameraPos - i.world_pos); //计算观察方向 
	float3 half_dir = normalize(world_view_dir + world_light_dir); //半角向量
	float specu = pow(max(0, dot(half_dir, i.world_normal)), exp2( _SpecularPower * 10 + 1 )); //计算镜面反射强度

	float3 ambient_col = _LIGHTMODEL_AMBIENT;
	float3 diffuse_col = _Light[0].Color * lambert;
	float3 specu_col = specu;
	float3 rgb_col = (ambient_col + diffuse_col + specu_col) * albedo.rgb;
	float4 final_col = float4(rgb_col, 1);

	APPLY_FOG(i.fogCoord, final_col);
	return LinearToGammaSpace(final_col);
}