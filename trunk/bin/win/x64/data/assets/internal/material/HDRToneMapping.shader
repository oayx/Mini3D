Shader "Hidden/HDRToneMapping" {
    Properties {
        [HideInInspector] _Exposure ("Exposure", Float) = 1.0
        [HideInInspector] _MainTex ("Main Texture", 2D) = "white" {}
    }

    SubShader {
        Tags {
            "Queue" = "Overlay"
            "CastShadow" = "False"
            "ReceiveShadow" = "False"
        }

        Pass {
            Cull Off
            ZWrite Off

            CGPROGRAM
            #include "HDRToneMapping.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
    FallBack Off
}