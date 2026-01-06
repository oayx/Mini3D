Shader "Hidden/RenderDepthMap" {
    SubShader {
        Tags {
            "RenderType" = "Opaque"
            "CastShadow" = "False"
        }

        Pass {
            Tags { "LightMode" = "Depth" }
            
            ZWrite On
            ZTest LEqual

            CGPROGRAM
            #include "RenderDepthMap.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
    FallBack Off
}