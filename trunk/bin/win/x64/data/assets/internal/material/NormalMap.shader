Shader "Bumped Diffuse" {
    Properties {
        _MainTex ("Main Texture", 2D) = "white" {}
        _BumpMap ("Normal Map", 2D) = "bump" {}
    }

    SubShader {
        Tags { 
            "RenderType" = "Opaque"
            "EnableLight" = "true"
        }

        Pass {

            Tags { "LightMode" = "ForwardBase" }

            CGPROGRAM
            #include "NormalMap.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
    FallBack "Diffuse"
}