Shader "Hidden/PostProcessBlit" {
    Properties {
        [HideInInspector] _MainTex ("Main Texture", 2D) = "white" {}
    }

    SubShader {
        Tags {
            "CastShadow" = "False"
        }

        Pass {
            Cull Off
            ZWrite Off
            ZTest Always
            Blend Off

            CGPROGRAM
            #include "PostProcessBlit.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
    FallBack Off
}