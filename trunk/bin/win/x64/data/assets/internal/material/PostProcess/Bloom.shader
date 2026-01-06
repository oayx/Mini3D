Shader "Post Processing/Bloom" {
    Properties {
        _MainTex ("Base (RGB)", 2D) = "white" {}
        _BlurTex ("Blur Texture", 2D) = "black" {}
    }

    SubShader {
        // Threshold提取阶段
        Pass {
            CGPROGRAM
            #include "Bloom.hlsl" 
            #pragma vertex vert_threshold
            #pragma fragment frag_threshold
            ENDCG
        }

        // 模糊处理阶段
        Pass {
            CGPROGRAM
            #include "Bloom.hlsl" 
            #pragma vertex vert_blur
            #pragma fragment frag_blur
            ENDCG
        }

        // Bloom合成阶段
        Pass {
            CGPROGRAM
            #include "Bloom.hlsl" 
            #pragma vertex vert_bloom
            #pragma fragment frag_bloom
            ENDCG
        }
    }
}