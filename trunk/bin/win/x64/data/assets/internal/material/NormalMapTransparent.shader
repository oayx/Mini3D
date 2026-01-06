Shader "Transparent/Bumped Diffuse" {
    Properties {
        _MainTex ("Main Texture", 2D) = "white" {}
        _NormalTexture ("Normal Map", 2D) = "bump" {}
    }

    SubShader {
        Tags { 
            "EnableLight" = "true"
        }

        Pass {
            Blend Off
            ZWrite Off

            CGPROGRAM
            #include "NormalMap.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
}