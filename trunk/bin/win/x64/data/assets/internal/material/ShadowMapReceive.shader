Shader "ShadowMap/Receive" {
    Properties {
        _Bias ("Bias", Range(0, 2)) = 0
        _MainTex ("Main Texture", 2D) = "white" {}
    }
    
    SubShader {
        Tags {
            "RenderType" = "Opaque"
            "EnableLight" = "True"
        }
        
        Pass { 
            ZWrite On
            ZTest LEqual
            
            CGPROGRAM
            #include "ShadowMapReceive.hlsl" 
            #define SOFT_SHADOWS
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
}