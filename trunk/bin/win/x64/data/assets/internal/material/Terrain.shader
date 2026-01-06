Shader "Terrain/Terrain" {
    Properties {
        _MainTex ("Base Texture", 2D) = "white" {}
        _NormalTexture ("Normal Map", 2D) = "bump" {}
        _SpecularPower ("Specular Power", Range(0,1)) = 0.1
    }
    
    SubShader {
        Tags {
            "Queue" = "1500"
            "RenderType" = "Opaque"
            "CastShadow" = "False"
            "EnableLight" = "true"
        }
        
        Pass {
            CGPROGRAM
            #include "Terrain.hlsl" 
            #define SOFT_SHADOWS
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
}