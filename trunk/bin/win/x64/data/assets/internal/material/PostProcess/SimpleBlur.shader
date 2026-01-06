Shader "Post Processing/SimpleBlur" {

    SubShader {
        Pass {
            CGPROGRAM
            #include "SimpleBlur.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
}