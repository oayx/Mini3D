Shader "Hidden/Geometry" {
    Properties {
        [HideInInspector] _Color ("Color", Color) = (1,1,1,1)
    }

    SubShader {
        Tags { 
            "RenderType" = "Opaque"
            "Queue" = "Geometry"
        }

        Pass {
            CGPROGRAM
            #include "Geometry.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            #pragma geometry gs_main
            ENDCG
        }
    }
    FallBack Off
}