Shader "Standard" {
    Properties {
        _MainTex ("Main Texture", 2D) = "white" {}
        _NormalTexture ("Normal Map", 2D) = "bump" {}
        _SpecularPower ("Specular Power", Range(0,1)) = 0.1
        _Reflections ("Reflections Intensity", Range(0,1)) = 0.0
    }
    
    SubShader {
        Tags {
            "EnableLight" = "true"
        }
        
        Pass {
            CGPROGRAM
            #include "Standard.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
    FallBack "Diffuse"
}