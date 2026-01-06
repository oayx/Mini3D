Shader "Line/Default" {
    SubShader {
        Tags {
            "RenderType" = "Opaque"
            "Queue" = "Geometry"
            "CastShadow" = "False"
            "ReceiveShadow" = "False"
        }

        Pass {
            ZWrite Off
            ZTest LEqual

            CGPROGRAM
            #include "Line.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
    FallBack Off
}