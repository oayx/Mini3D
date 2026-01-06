Shader "Post Processing/EdgeDetection" {
    SubShader {
        Pass {
            CGPROGRAM
            #include "EdgeDetection.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
}