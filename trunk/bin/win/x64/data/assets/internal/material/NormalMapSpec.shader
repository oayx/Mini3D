Shader "Bumped Specular" {
    Properties {
        _MainTex ("Main Texture", 2D) = "white" {}
        _BumpMap ("Normal Map", 2D) = "bump" {}
        [PowerSlider(3.0)] _SpecularPower ("Specular Intensity", Range(0, 1)) = 0.1
    }

    SubShader {
        Tags { 
            "RenderType" = "Opaque"
            "EnableLight" = "true"
        }

        Pass {
            CGPROGRAM
            #include "NormalMapSpec.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
}