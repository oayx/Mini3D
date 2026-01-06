Shader "Unlit/Texture" {
    Properties {
        _MainTex ("Main Texture", 2D) = "white" {}
        _Color ("Tint Color", Color) = (1,1,1,1)
    }
    
    SubShader {
        Tags {
            "RenderType" = "Opaque"
            "Queue" = "Geometry"
        }
        
        Pass {
            ZWrite On
            ZTest LEqual
            
            CGPROGRAM
            #include "UnlitTexture.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
}