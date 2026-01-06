Shader "Hidden/ReflectionProbe" {
    Properties {
        [HideInInspector] _MainTex ("Reflection Texture", 2D) = "white" {}
    }

    SubShader {
        Tags {
            "RenderType" = "Opaque"
            "CastShadow" = "False"
        }

        Pass {
            Blend SrcAlpha OneMinusSrcAlpha

            CGPROGRAM
            #include "ReflectionProbe.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
    FallBack Off
}