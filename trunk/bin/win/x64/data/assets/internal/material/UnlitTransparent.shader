Shader "Unlit/Transparent" {
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
            #include "UnlitTransparent.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
}