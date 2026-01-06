Shader "Sprite/Default" {
    Properties {
        _MainTex ("Main Texture", 2D) = "white" {}
    }
    
    SubShader {
        Tags {
            "Queue" = "Transparent"
            "RenderType" = "Transparent"
            "CastShadow" = "False"
        }
        
        Pass {
            
            
            ZWrite On
            ZTest LEqual
            Blend SrcAlpha OneMinusSrcAlpha
            Cull Off
            
            CGPROGRAM
            #include "Sprite.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
}