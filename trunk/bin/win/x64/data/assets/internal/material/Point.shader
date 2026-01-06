Shader "Point/Default" {
    SubShader {
        Tags { 
            "RenderType" = "Opaque"
            "Queue" = "Geometry"
            "CastShadow" = "False"
        }

        Pass {
            ZWrite Off
            ZTest LEqual

            CGPROGRAM
            #include "Point.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
    FallBack Off
}