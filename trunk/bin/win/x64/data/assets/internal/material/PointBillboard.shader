Shader "Point/Billboard" {
    Properties {
        _MainTex ("Billboard Texture", 2D) = "white" {}
    }

    SubShader {
        Tags { 
            "RenderType" = "Transparent"
            "Queue" = "Transparent"
            "CastShadow" = "False"
        }

        Pass {
            Cull Off
            ZWrite Off
            Blend SrcAlpha OneMinusSrcAlpha

            CGPROGRAM
            #include "PointBillboard.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            #pragma geometry gs_main
            ENDCG
        }
    }
    FallBack Off
}