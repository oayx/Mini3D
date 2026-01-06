Shader "Unlit/Instance/Transparent" {
    Properties {
        _MainTex ("Main Texture", 2D) = "white" {}
        _Color ("Tint Color", Color) = (1,1,1,1)
    }

    SubShader {
        Tags {
            "Queue" = "3000"
            "RenderType" = "Transparent"
        }

        Pass {
            ZTest LEqual
            ZWrite Off
            Blend SrcAlpha OneMinusSrcAlpha
            
            CGPROGRAM
            #include "UnlitTransparentInstance.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }

        Pass {
            Tags { "LightMode" = "ShadowCaster" }
            
            ZWrite On
            ZTest LEqual
            
            CGPROGRAM
            #include "UnlitTransparentInstance.hlsl" 
            #pragma vertex vs_shadow
            #pragma fragment ps_shadow
            ENDCG
        }
    }
}