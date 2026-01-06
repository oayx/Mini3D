Shader "Post Processing/FXAA" {
    SubShader {
        Pass {
            CGPROGRAM
            #include "FXAA.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
}