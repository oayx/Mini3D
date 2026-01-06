Shader "Unlit/Transparent Cutoff" {
    Properties {
        _MainTex ("Main Texture", 2D) = "white" {}
        _Color ("Tint Color", Color) = (1,1,1,1)
        _Cutoff ("Alpha Cutoff", Range(0,1)) = 0.1
    }
    
    SubShader {
        Tags {
            "Queue" = "AlphaTest"
            "RenderType" = "TransparentCutout"
        }
        
        Pass {
            ZTest LEqual
            ZWrite Off
            Blend Off
            
            CGPROGRAM
            #include "UnlitTextureCutoff.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
}