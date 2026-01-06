#include "CGInclude.hlsli"

BEGIN_CONSTANTS(CustomBuffer, 0)
	float4 _MainTex_TexelSize;
	float4 _EdgeColor;
END_CONSTANTS

// 基础贴图
DECLARE_TEXTURE(_MainTex, 0);

struct appdata
{
	float3 vertex		: POSITION;
	float2 uv			: TEXCOORD0;
};

struct v2f
{
    float4 pos			: SV_POSITION;
    float2 uv[9]  		: TEXCOORD0;
};

// 顶点渲染器主函数
v2f vs_main(appdata v)
{
	v2f o;

    //顶点位置 
	o.pos = float4(v.vertex, 1);
    //纹理
	float2 uv = v.uv;
	float2 size = _MainTex_TexelSize.xy;
    //计算周围像素的纹理坐标位置，其中4为原始点，右侧乘积因子为偏移的像素单位，坐标轴为左下角原点，右上为+x,+y方向，与uv的坐标轴匹配
	o.uv[0] = uv + size * float2(-1, 1);
	o.uv[1] = uv + size * float2(0, 1);
	o.uv[2] = uv + size * float2(1, 1);
	o.uv[3] = uv + size * float2(-1, 0);
	o.uv[4] = uv + size * float2(0, 0);
	o.uv[5] = uv + size * float2(1, 0);
	o.uv[6] = uv + size * float2(-1, -1);
	o.uv[7] = uv + size * float2(0, -1);
	o.uv[8] = uv + size * float2(1, -1);

    return o;
}
//计算对应像素的最低灰度值并返回
float minGrayCompute(v2f i, int idx)
{
	return Luminance(SAMPLE_TEXTURE(_MainTex, i.uv[idx]).rgb);
}
//利用Sobel算子计算最终梯度值
float sobel(v2f i)
{
	const float Gx[9] =
	{
		-1, 0, 1,
        -2, 0, 2,
        -1, 0, 1
	};
	const float Gy[9] =
	{
		-1, -2, -1,
         0, 0, 0,
         1, 2, 1
	};
	//分别计算横向和纵向的梯度值，方法为各项对应元素相乘并相加
	float graX = 0;
	float graY = 0;

	for (int it = 0; it < 9; it++)
	{
		graX += Gx[it] * minGrayCompute(i, it);
		graY += Gy[it] * minGrayCompute(i, it);
	}
	//绝对值相加近似模拟最终梯度值
	return abs(graX) + abs(graY);
}
// 像素渲染器主函数
float4 ps_main(v2f i) : SV_Target
{
	float gra = sobel(i);
	float4 col = SAMPLE_TEXTURE(_MainTex, i.uv[4]);
	//利用得到的梯度值进行插值操作，其中梯度值越大，越接近边缘的颜色
	float4 final_col = lerp(col, _EdgeColor, gra);
	return final_col;
}