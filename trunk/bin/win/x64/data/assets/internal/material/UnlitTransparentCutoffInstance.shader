Shader "Unlit/Instance/Transparent Cutoff" {
    Properties {
        _MainTex ("Main Texture", 2D) = "white" {}
        _Color ("Tint Color", Color) = (1,1,1,1)
        _Cutoff ("Alpha Cutoff", Range(0,1)) = 0.1
    }

    SubShader {
        Tags {
            "Queue" = "3000"
            "RenderType" = "TransparentCutout"
        }

        Pass {
            ZTest LEqual
            ZWrite Off
            Blend SrcAlpha OneMinusSrcAlpha
            
            CGPROGRAM
            #include "UnlitTransparentCutoffInstance.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }

        Pass {
            Tags { "LightMode" = "ShadowCaster" }
            
            ZWrite On
            ZTest LEqual
            
            CGPROGRAM
            #include "UnlitTransparentCutoffInstance.hlsl" 
            #pragma vertex vs_shadow
            #pragma fragment ps_shadow
            ENDCG
        }
    }
}