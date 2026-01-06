Shader "Post Processing/ColorAdjust" {
    SubShader {
        Pass {
            CGPROGRAM
            #include "ColorAdjust.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
}