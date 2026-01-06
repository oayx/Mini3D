Shader "Water/Water" {
    Properties {
        _MainTex ("Main Texture", 2D) = "white" {}
        _TintColor ("Tint Color", Color) = (1,1,1,1)
        _Speed ("Flow Speed", Float) = 1
        _Distort ("Distortion", Float) = 1
        _Intensity ("Effect Intensity", Float) = 1
        _Alpha ("Transparency", Float) = 1
        _FresnelIndensity ("Fresnel Indensity", Range(0,1)) = 0.5
    }

    SubShader {
        Tags {
            "Queue" = "2600"
            "RenderType" = "Transparent"
        }

        Pass {
            Cull Off
            ZTest LEqual
            ZWrite Off
            Blend SrcAlpha One

            CGPROGRAM
            #include "Water.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
}