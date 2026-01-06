Shader "Hidden/Gizmos" {
    Properties {
        [HideInInspector] _MainTex ("Main Texture", 2D) = "white" {}
    }

    SubShader {
        Tags {
            "Queue" = "Transparent"
            "RenderType" = "Transparent"
            "CastShadow" = "False"
            "ReceiveShadow" = "False"
        }

        Pass {
            Blend SrcAlpha OneMinusSrcAlpha
            ZWrite Off
            ZTest LEqual

            CGPROGRAM
            #include "Gizmos.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
    FallBack Off
}