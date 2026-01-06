Shader "Post Processing/MotionBlur" {

    SubShader {
        Pass {
            CGPROGRAM
            #include "MotionBlur.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
}
