Shader "UI/Default" {
    Properties {
        _MainTex ("Main Texture", 2D) = "white" {}
        _Color ("Tint Color", Color) = (1,1,1,1)
    }
    
    SubShader {
        Tags {
            "Queue" = "Overlay"
            "RenderType" = "Transparent"
            "CastShadow" = "False"
        }
        
        Pass {
            
            
            Cull Off
            ZTest Always
            ZWrite Off
            Blend SrcAlpha OneMinusSrcAlpha
            
            CGPROGRAM
            #include "UI.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
}