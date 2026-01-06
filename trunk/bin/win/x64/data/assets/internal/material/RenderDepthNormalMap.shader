Shader "Hidden/RenderDepthNormals" {
    SubShader {
        Tags {
            "RenderType" = "Opaque"
            "CastShadow" = "False"
        }

        Pass {
            Tags { "LightMode" = "DepthNormal" }
            
            ZWrite On
            ZTest LEqual

            CGPROGRAM
            #include "RenderDepthNormalMap.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
    FallBack Off
}