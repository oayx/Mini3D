Shader "Diffuse" {
    Properties {
        _SpecularPower ("Specular Power", Float) = 1
        _MainTex ("Main Texture", 2D) = "white" {}
    }

    SubShader {
        Tags {
            "EnableLight" = "true"
        }

        Pass {
            ZWrite On
            ZTest LEqual
            Blend Off

            CGPROGRAM
            #include "Diffuse.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
}