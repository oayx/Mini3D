#include "CGInclude.hlsli"

BEGIN_CONSTANTS(CustomBuffer, 0)
float4 _MainTex_TexelSize;
float4 _RcpFrame;
float4 _RcpFrameOpt;
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
    float2 uv  			: TEXCOORD0;
	float4 uvAux		: TEXCOORD1;
};



// 顶点渲染器主函数
v2f vs_main(appdata v)
{
	v2f o;

    //顶点位置 
	o.pos = float4(v.vertex, 1);
    //纹理
	float2 uv = v.uv;
#if !defined(UV_STARTS_AT_TOP)
	uv.y = 1.0 - uv.y;
#endif
	o.uv = uv;
	
	o.uvAux.xy = uv + float2(-_MainTex_TexelSize.x, +_MainTex_TexelSize.y) * 0.5f;
	o.uvAux.zw = uv + float2(+_MainTex_TexelSize.x, -_MainTex_TexelSize.y) * 0.5f;

    return o;
}

/*============================================================================
                     NVIDIA FXAA 3.9 by TIMOTHY LOTTES
------------------------------------------------------------------------------
COPYRIGHT (C) 2010, 2011 NVIDIA CORPORATION. ALL RIGHTS RESERVED.
------------------------------------------------------------------------------
TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED 
*AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS 
OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF 
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL NVIDIA 
OR ITS SUPPLIERS BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR 
CONSEQUENTIAL DAMAGES WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR 
LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, 
OR ANY OTHER PECUNIARY LOSS) ARISING OUT OF THE USE OF OR INABILITY TO USE 
THIS SOFTWARE, EVEN IF NVIDIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH 
DAMAGES.
------------------------------------------------------------------------------
                            INTEGRATION CHECKLIST
------------------------------------------------------------------------------
(1.) 
In the shader source,
setup defines for the desired configuration. 
Example,

  #define FXAA_PC 1
  #define FXAA_HLSL_3 1

(2.) 
Then include this file,

  #include "Fxaa3.h"

(3.) 
Then call the FXAA pixel shader from within your desired shader,

  return FxaaPixelShader(pos, posPos, tex, rcpFrame, rcpFrameOpt);

(4.) 
Insure pass prior to FXAA outputs RGBL.
See next section.

(5.) 
Setup engine to provide "rcpFrame" and "rcpFrameOpt" constants.
Not using constants will result in a performance loss.

  // {x_} = 1.0/screenWidthInPixels
  // {_y} = 1.0/screenHeightInPixels
  float2 rcpFrame

  // This must be from a constant/uniform.
  // {x___} = 2.0/screenWidthInPixels
  // {_y__} = 2.0/screenHeightInPixels
  // {__z_} = 0.5/screenWidthInPixels
  // {___w} = 0.5/screenHeightInPixels
  float4 rcpFrameOpt 
    
(6.) 
Have FXAA vertex shader run as a full screen triangle,
and output "pos" and "posPos" such that inputs in the pixel shader provide,
  
  // {xy} = center of pixel
  float2 pos,
    
  // {xy__} = upper left of pixel
  // {__zw} = lower right of pixel
  float4 posPos,

(7.) 
Insure the texture sampler used by FXAA is set to bilinear filtering.


------------------------------------------------------------------------------
                     INTEGRATION - RGBL AND COLORSPACE
------------------------------------------------------------------------------
FXAA3 requires RGBL as input.

RGB should be LDR (low dynamic range). 
Specifically do FXAA after tonemapping.

RGB data as returned by a texture fetch can be linear or non-linear.
Note an "sRGB format" texture counts as linear, 
because the result of a texture fetch is linear data.
Regular "RGBA8" textures in the sRGB colorspace are non-linear.

Luma must be stored in the alpha channel prior to running FXAA.
This luma should be in a perceptual space (could be gamma 2.0).
Example pass before FXAA where output is gamma 2.0 encoded,

  color.rgb = ToneMap(color.rgb); // linear color output
  color.rgb = sqrt(color.rgb);    // gamma 2.0 color output
  return color;

To use FXAA,

  color.rgb = ToneMap(color.rgb);  // linear color output
  color.rgb = sqrt(color.rgb);     // gamma 2.0 color output
  color.a = dot(color.rgb, float3(0.299, 0.587, 0.114)); // compute luma
  return color;

Another example where output is linear encoded,
say for instance writing to an sRGB formated render target,
where the render target does the conversion back to sRGB after blending,

  color.rgb = ToneMap(color.rgb); // linear color output
  return color;

To use FXAA,

  color.rgb = ToneMap(color.rgb); // linear color output
  color.a = sqrt(dot(color.rgb, float3(0.299, 0.587, 0.114))); // compute luma
  return color;

Getting luma correct is required for the algorithm to work correctly.

------------------------------------------------------------------------------
                            COMPLEX INTEGRATION
------------------------------------------------------------------------------
Q. What if the engine is blending into RGB before wanting to run FXAA?

A. In the last opaque pass prior to FXAA,
   have the pass write out luma into alpha.
   Then blend into RGB only.
   FXAA should be able to run ok 
   assuming the blending pass did not any add aliasing.
   This should be the common case for particles and common blending passes.

============================================================================*/

/*============================================================================

                            INTEGRATION KNOBS

============================================================================*/
//
// FXAA_PS3 and FXAA_360 choose the console algorithm (FXAA3 CONSOLE).
// FXAA_360_OPT is a prototype for the new optimized 360 version.
//
// 1 = Use API.
// 0 = Don't use API.
//
/*--------------------------------------------------------------------------*/
#ifndef FXAA_PS3
#define FXAA_PS3 0
#endif
/*--------------------------------------------------------------------------*/
#ifndef FXAA_360
#define FXAA_360 0
#endif
/*--------------------------------------------------------------------------*/
#ifndef FXAA_360_OPT
#define FXAA_360_OPT 0
#endif
/*==========================================================================*/
#ifndef FXAA_PC
    //
    // FXAA Quality 
    // The high quality PC algorithm.
    //
#define FXAA_PC 0
#endif
/*--------------------------------------------------------------------------*/
#ifndef FXAA_PC_CONSOLE
    //
    // The console algorithm for PC is included 
    // for developers targeting really low spec machines.
    //
#define FXAA_PC_CONSOLE 0
#endif
/*--------------------------------------------------------------------------*/
#ifndef FXAA_GLSL_120
#define FXAA_GLSL_120 0
#endif
/*--------------------------------------------------------------------------*/
#ifndef FXAA_GLSL_130
#define FXAA_GLSL_130 0
#endif
/*--------------------------------------------------------------------------*/
#ifndef FXAA_HLSL_3
#define FXAA_HLSL_3 0
#endif
/*--------------------------------------------------------------------------*/
#ifndef FXAA_HLSL_4
#define FXAA_HLSL_4 0
#endif    
/*--------------------------------------------------------------------------*/
#ifndef FXAA_HLSL_5
#define FXAA_HLSL_5 0
#endif    
/*==========================================================================*/
#ifndef FXAA_EARLY_EXIT
    // 
    // Controls algorithm's early exit path.
    // On PS3 turning this on adds 2 cycles to the shader.
    // On 360 turning this off adds 10ths of a millisecond to the shader.
    // Turning this off on console will result in a more blurry image.
    // So this defaults to on.
    // 
    // 1 = On.
    // 0 = Off.
    // 
#define FXAA_EARLY_EXIT 1
#endif
/*--------------------------------------------------------------------------*/
#ifndef FXAA_DISCARD
    // 
    // Only valid for PC OpenGL currently.
    // 
    // 1 = Use discard on pixels which don't need AA.
    //     For APIs which enable concurrent TEX+ROP from same surface.
    // 0 = Return unchanged color on pixels which don't need AA.
    // 
#define FXAA_DISCARD 0
#endif    
/*--------------------------------------------------------------------------*/
#ifndef FXAA_FAST_PIXEL_OFFSET
    //
    // Used for GLSL 120 only.
    //
    // 1 = GL API supports fast pixel offsets
    // 0 = do not use fast pixel offsets
    // 
#ifdef GL_EXT_gpu_shader4
#define FXAA_FAST_PIXEL_OFFSET 1
#endif
#ifdef GL_NV_gpu_shader5
#define FXAA_FAST_PIXEL_OFFSET 1
#endif
#ifdef GL_ARB_gpu_shader5
#define FXAA_FAST_PIXEL_OFFSET 1
#endif
#ifndef FXAA_FAST_PIXEL_OFFSET
#define FXAA_FAST_PIXEL_OFFSET 0
#endif
#endif
/*--------------------------------------------------------------------------*/
#ifndef FXAA_GATHER4_ALPHA
    //
    // 1 = API supports gather4 on alpha channel.
    // 0 = API does not support gather4 on alpha channel.
    //
#if (FXAA_HLSL_5 == 1)
#define FXAA_GATHER4_ALPHA 1
#endif
#ifdef GL_ARB_gpu_shader5
#define FXAA_GATHER4_ALPHA 1
#endif
#ifdef GL_NV_gpu_shader5
#define FXAA_GATHER4_ALPHA 1
#endif
#ifndef FXAA_GATHER4_ALPHA
#define FXAA_GATHER4_ALPHA 0
#endif
#endif

/*============================================================================
                         FXAA CONSOLE - TUNING KNOBS
============================================================================*/
#ifndef FXAA_CONSOLE__EDGE_SHARPNESS
    //
    // Consoles the sharpness of edges.
    // 
    // Due to the PS3 being ALU bound, 
    // there are only two safe values here: 4 and 8.
    // These options use the shaders ability to a free *|/ by 4|8.
    //
    // 8.0 is sharper
    // 4.0 is softer
    // 2.0 is really soft (good for vector graphics inputs)
    //
#if 1 
#define FXAA_CONSOLE__EDGE_SHARPNESS 8.0
#endif
#if 0
#define FXAA_CONSOLE__EDGE_SHARPNESS 4.0
#endif
#if 0
#define FXAA_CONSOLE__EDGE_SHARPNESS 2.0
#endif
#endif
/*--------------------------------------------------------------------------*/
#ifndef FXAA_CONSOLE__EDGE_THRESHOLD
    //
    // The minimum amount of local contrast required to apply algorithm.
    // The console setting has a different mapping than the quality setting.
    //
    // This only applies when FXAA_EARLY_EXIT is 1.
    //
    // Due to the PS3 being ALU bound, 
    // there are only two safe values here: 0.25 and 0.125.
    // These options use the shaders ability to a free *|/ by 4|8.
    //
    // 0.125 leaves less aliasing, but is softer
    // 0.25 leaves more aliasing, and is sharper
    //
#if 1
#define FXAA_CONSOLE__EDGE_THRESHOLD 0.125
#else
#define FXAA_CONSOLE__EDGE_THRESHOLD 0.25
#endif        
#endif
/*--------------------------------------------------------------------------*/
#ifndef FXAA_CONSOLE__EDGE_THRESHOLD_MIN
    //
    // Trims the algorithm from processing darks.
    // The console setting has a different mapping than the quality setting.
    //
    // This only applies when FXAA_EARLY_EXIT is 1.
    //
    // This does not apply to PS3.
    // PS3 was simplified to avoid more shader instructions.
    // 
#define FXAA_CONSOLE__EDGE_THRESHOLD_MIN 0.05
#endif

/*============================================================================
                         FXAA QUALITY - TUNING KNOBS
============================================================================*/
#ifndef FXAA_QUALITY__EDGE_THRESHOLD
    //
    // The minimum amount of local contrast required to apply algorithm.
    //
    // 1/3 - too little
    // 1/4 - low quality
    // 1/6 - default
    // 1/8 - high quality
    // 1/16 - overkill
    //
#define FXAA_QUALITY__EDGE_THRESHOLD (1.0/6.0)
#endif
/*--------------------------------------------------------------------------*/
#ifndef FXAA_QUALITY__EDGE_THRESHOLD_MIN
    //
    // Trims the algorithm from processing darks.
    //
    // 1/32 - visible limit
    // 1/16 - high quality
    // 1/12 - upper limit (default, the start of visible unfiltered edges)
    //
#define FXAA_QUALITY__EDGE_THRESHOLD_MIN (1.0/12.0)
#endif
/*--------------------------------------------------------------------------*/
#ifndef FXAA_QUALITY__SUBPIX
    //
    // Choose the amount of sub-pixel aliasing removal.
    //
    // 1   - upper limit (softer)
    // 3/4 - default amount of filtering
    // 1/2 - lower limit (sharper, less sub-pixel aliasing removal)
    //
    //
#define FXAA_QUALITY__SUBPIX (3.0/4.0)
#endif

/*============================================================================
                                 API PORTING
============================================================================*/
float4 luma(float4 color)
{
	color.a = dot(color.rgb, float3(0.299f, 0.587f, 0.114f));
	return color;
}

#define int2 float2
#define FxaaInt2 float2
#define FxaaFloat2 float2
#define FxaaFloat3 float3
#define FxaaFloat4 float4
#define FxaaDiscard clip(-1)
#define FxaaDot3(a, b) dot(a, b)
#define FxaaSat(x) saturate(x)
#define FxaaLerp(x,y,s) lerp(x,y,s)
#define FxaaTex Texture2D
#define FxaaSampler sampler
#define FxaaTexTop(t, p) luma( SAMPLE_TEXTURE(t, float2(p.xy)) )
#define FxaaTexOff(t, p, o, r) luma( SAMPLE_TEXTURE(t, float2(p + (o * r))) )
// 像素渲染器主函数
float4 ps_main(v2f i) : SV_Target
{
    // {xy} = center of pixel
	float2 pos = i.uv;
    // {xyzw} = not used on FXAA3 Quality
	float4 posPos = i.uvAux;
	// This must be from a constant/uniform.
    // {x_} = 1.0/screenWidthInPixels
    // {_y} = 1.0/screenHeightInPixels
	float2 rcpFrame = _RcpFrame.xy;
	float2 posM;
	posM.x = pos.x;
	posM.y = pos.y;
	float4 rgbyM = FxaaTexTop(_MainTex, posM);
#define lumaM rgbyM.w
	float lumaS = FxaaTexOff(_MainTex, posM, FxaaInt2( 0, 1), rcpFrame.xy).w;
	float lumaE = FxaaTexOff(_MainTex, posM, FxaaInt2( 1, 0), rcpFrame.xy).w;
	float lumaN = FxaaTexOff(_MainTex, posM, FxaaInt2( 0,-1), rcpFrame.xy).w;
	float lumaW = FxaaTexOff(_MainTex, posM, FxaaInt2(-1, 0), rcpFrame.xy).w;
/*--------------------------------------------------------------------------*/
	float maxSM = max(lumaS, lumaM);
	float minSM = min(lumaS, lumaM);
	float maxESM = max(lumaE, maxSM);
	float minESM = min(lumaE, minSM);
	float maxWN = max(lumaN, lumaW);
	float minWN = min(lumaN, lumaW);
	float rangeMax = max(maxWN, maxESM);
	float rangeMin = min(minWN, minESM);
	float rangeMaxScaled = rangeMax * FXAA_QUALITY__EDGE_THRESHOLD;
	float range = rangeMax - rangeMin;
	float rangeMaxClamped = max(FXAA_QUALITY__EDGE_THRESHOLD_MIN, rangeMaxScaled);
/*--------------------------------------------------------------------------*/
	[flatten]
	if (range < rangeMaxClamped)
		return rgbyM;
/*--------------------------------------------------------------------------*/
#if (FXAA_GATHER4_ALPHA == 0) 
	float lumaNW = FxaaTexOff(_MainTex, posM, FxaaInt2(-1,-1), rcpFrame.xy).w;
	float lumaSE = FxaaTexOff(_MainTex, posM, FxaaInt2( 1, 1), rcpFrame.xy).w;
	float lumaNE = FxaaTexOff(_MainTex, posM, FxaaInt2( 1,-1), rcpFrame.xy).w;
	float lumaSW = FxaaTexOff(_MainTex, posM, FxaaInt2(-1, 1), rcpFrame.xy).w;
#else
        float lumaNE = FxaaTexOff(_MainTex, samp,posM, FxaaInt2(1, -1), rcpFrame.xy).w;
        float lumaSW = FxaaTexOff(_MainTex, samp,posM, FxaaInt2(-1, 1), rcpFrame.xy).w;
#endif
/*--------------------------------------------------------------------------*/
	float lumaNS = lumaN + lumaS;
	float lumaWE = lumaW + lumaE;
	float subpixRcpRange = 1.0 / range;
	float subpixNSWE = lumaNS + lumaWE;
	float edgeHorz1 = (-2.0 * lumaM) + lumaNS;
	float edgeVert1 = (-2.0 * lumaM) + lumaWE;
/*--------------------------------------------------------------------------*/
	float lumaNESE = lumaNE + lumaSE;
	float lumaNWNE = lumaNW + lumaNE;
	float edgeHorz2 = (-2.0 * lumaE) + lumaNESE;
	float edgeVert2 = (-2.0 * lumaN) + lumaNWNE;
/*--------------------------------------------------------------------------*/
	float lumaNWSW = lumaNW + lumaSW;
	float lumaSWSE = lumaSW + lumaSE;
	float edgeHorz4 = (abs(edgeHorz1) * 2.0) + abs(edgeHorz2);
	float edgeVert4 = (abs(edgeVert1) * 2.0) + abs(edgeVert2);
	float edgeHorz3 = (-2.0 * lumaW) + lumaNWSW;
	float edgeVert3 = (-2.0 * lumaS) + lumaSWSE;
	float edgeHorz = abs(edgeHorz3) + edgeHorz4;
	float edgeVert = abs(edgeVert3) + edgeVert4;
/*--------------------------------------------------------------------------*/
	float subpixNWSWNESE = lumaNWSW + lumaNESE;
	float lengthSign = rcpFrame.x;
	bool horzSpan = edgeHorz >= edgeVert;
	float subpixA = subpixNSWE * 2.0 + subpixNWSWNESE;
/*--------------------------------------------------------------------------*/
	if (!horzSpan)
		lumaN = lumaW;
	if (!horzSpan)
		lumaS = lumaE;
	if (horzSpan)
		lengthSign = rcpFrame.y;
	float subpixB = (subpixA * (1.0 / 12.0)) - lumaM;
/*--------------------------------------------------------------------------*/
	float gradientN = lumaN - lumaM;
	float gradientS = lumaS - lumaM;
	float lumaNN = lumaN + lumaM;
	float lumaSS = lumaS + lumaM;
	bool pairN = abs(gradientN) >= abs(gradientS);
	float gradient = max(abs(gradientN), abs(gradientS));
	if (pairN)
		lengthSign = -lengthSign;
	float subpixC = FxaaSat(abs(subpixB) * subpixRcpRange);
/*--------------------------------------------------------------------------*/
	float2 posB;
	posB.x = posM.x;
	posB.y = posM.y;
	float2 offNP;
	offNP.x = (!horzSpan) ? 0.0 : rcpFrame.x;
	offNP.y = (horzSpan) ? 0.0 : rcpFrame.y;
	if (!horzSpan)
		posB.x += lengthSign * 0.5;
	if (horzSpan)
		posB.y += lengthSign * 0.5;
/*--------------------------------------------------------------------------*/
	float2 posN;
	posN.x = posB.x - offNP.x;
	posN.y = posB.y - offNP.y;
	float2 posP;
	posP.x = posB.x + offNP.x;
	posP.y = posB.y + offNP.y;
	float subpixD = ((-2.0) * subpixC) + 3.0;
	float lumaEndN = FxaaTexTop(_MainTex, posN).w;
	float subpixE = subpixC * subpixC;
	float lumaEndP = FxaaTexTop(_MainTex, posP).w;
/*--------------------------------------------------------------------------*/
	if (!pairN)
		lumaNN = lumaSS;
	float gradientScaled = gradient * 1.0 / 4.0;
	float lumaMM = lumaM - lumaNN * 0.5;
	float subpixF = subpixD * subpixE;
	bool lumaMLTZero = lumaMM < 0.0;
/*--------------------------------------------------------------------------*/
	lumaEndN -= lumaNN * 0.5;
	lumaEndP -= lumaNN * 0.5;
	bool doneN = abs(lumaEndN) >= gradientScaled;
	bool doneP = abs(lumaEndP) >= gradientScaled;
	if (!doneN)
		posN.x -= offNP.x * 1.5;
	if (!doneN)
		posN.y -= offNP.y * 1.5;
	bool doneNP = (!doneN) || (!doneP);
	if (!doneP)
		posP.x += offNP.x * 1.5;
	if (!doneP)
		posP.y += offNP.y * 1.5;
	if (doneNP)
	{
/*--------------------------------------------------------------------------*/
		if (!doneN)
			lumaEndN = FxaaTexTop(_MainTex, posN.xy).w;
		if (!doneP)
			lumaEndP = FxaaTexTop(_MainTex, posP.xy).w;
		if (!doneN)
			lumaEndN = lumaEndN - lumaNN * 0.5;
		if (!doneP)
			lumaEndP = lumaEndP - lumaNN * 0.5;
		doneN = abs(lumaEndN) >= gradientScaled;
		doneP = abs(lumaEndP) >= gradientScaled;
		if (!doneN)
			posN.x -= offNP.x * 2.0;
		if (!doneN)
			posN.y -= offNP.y * 2.0;
		doneNP = (!doneN) || (!doneP);
		if (!doneP)
			posP.x += offNP.x * 2.0;
		if (!doneP)
			posP.y += offNP.y * 2.0;
		if (doneNP)
		{
/*--------------------------------------------------------------------------*/
			if (!doneN)
				lumaEndN = FxaaTexTop(_MainTex, posN.xy).w;
			if (!doneP)
				lumaEndP = FxaaTexTop(_MainTex, posP.xy).w;
			if (!doneN)
				lumaEndN = lumaEndN - lumaNN * 0.5;
			if (!doneP)
				lumaEndP = lumaEndP - lumaNN * 0.5;
			doneN = abs(lumaEndN) >= gradientScaled;
			doneP = abs(lumaEndP) >= gradientScaled;
			if (!doneN)
				posN.x -= offNP.x * 2.0;
			if (!doneN)
				posN.y -= offNP.y * 2.0;
			doneNP = (!doneN) || (!doneP);
			if (!doneP)
				posP.x += offNP.x * 2.0;
			if (!doneP)
				posP.y += offNP.y * 2.0;
			if (doneNP)
			{
/*--------------------------------------------------------------------------*/
				if (!doneN)
					lumaEndN = FxaaTexTop(_MainTex, posN.xy).w;
				if (!doneP)
					lumaEndP = FxaaTexTop(_MainTex, posP.xy).w;
				if (!doneN)
					lumaEndN = lumaEndN - lumaNN * 0.5;
				if (!doneP)
					lumaEndP = lumaEndP - lumaNN * 0.5;
				doneN = abs(lumaEndN) >= gradientScaled;
				doneP = abs(lumaEndP) >= gradientScaled;
				if (!doneN)
					posN.x -= offNP.x * 4.0;
				if (!doneN)
					posN.y -= offNP.y * 4.0;
				doneNP = (!doneN) || (!doneP);
				if (!doneP)
					posP.x += offNP.x * 4.0;
				if (!doneP)
					posP.y += offNP.y * 4.0;
				if (doneNP)
				{
/*--------------------------------------------------------------------------*/
					if (!doneN)
						lumaEndN = FxaaTexTop(_MainTex, posN.xy).w;
					if (!doneP)
						lumaEndP = FxaaTexTop(_MainTex, posP.xy).w;
					if (!doneN)
						lumaEndN = lumaEndN - lumaNN * 0.5;
					if (!doneP)
						lumaEndP = lumaEndP - lumaNN * 0.5;
					doneN = abs(lumaEndN) >= gradientScaled;
					doneP = abs(lumaEndP) >= gradientScaled;
					if (!doneN)
						posN.x -= offNP.x * 2.0;
					if (!doneN)
						posN.y -= offNP.y * 2.0;
					if (!doneP)
						posP.x += offNP.x * 2.0;
					if (!doneP)
						posP.y += offNP.y * 2.0;
				}
			}
		}
	}
/*--------------------------------------------------------------------------*/
	float dstN = posM.x - posN.x;
	float dstP = posP.x - posM.x;
	if (!horzSpan)
		dstN = posM.y - posN.y;
	if (!horzSpan)
		dstP = posP.y - posM.y;
/*--------------------------------------------------------------------------*/
	bool goodSpanN = (lumaEndN < 0.0) != lumaMLTZero;
	float spanLength = (dstP + dstN);
	bool goodSpanP = (lumaEndP < 0.0) != lumaMLTZero;
	float spanLengthRcp = 1.0 / spanLength;
/*--------------------------------------------------------------------------*/
	bool directionN = dstN < dstP;
	float dst = min(dstN, dstP);
	bool goodSpan = directionN ? goodSpanN : goodSpanP;
	float subpixG = subpixF * subpixF;
	float pixelOffset = (dst * (-spanLengthRcp)) + 0.5;
	float subpixH = subpixG * FXAA_QUALITY__SUBPIX;
/*--------------------------------------------------------------------------*/
	float pixelOffsetGood = goodSpan ? pixelOffset : 0.0;
	float pixelOffsetSubpix = max(pixelOffsetGood, subpixH);
	if (!horzSpan)
		posM.x += pixelOffsetSubpix * lengthSign;
	if (horzSpan)
		posM.y += pixelOffsetSubpix * lengthSign;
	return FxaaTexTop(_MainTex, posM);
}