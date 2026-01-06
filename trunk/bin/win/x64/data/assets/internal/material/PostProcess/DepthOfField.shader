Shader "Post Processing/DepthOfField" {
    Properties {
        _MainTex ("Base (RGB)", 2D) = "white" {}
        _BlurTex ("Blur Texture", 2D) = "black" {}
    }

    SubShader {
        // 模糊处理阶段
        Pass {
            CGPROGRAM
            #include "DepthOfField.hlsl" 
            #pragma vertex vert_blur
            #pragma fragment frag_blur
            ENDCG
        }

        // 景深合成阶段
        Pass {
            CGPROGRAM
            #include "DepthOfField.hlsl" 
            #pragma vertex vert_dof
            #pragma fragment frag_dof
            ENDCG
        }
    }
}