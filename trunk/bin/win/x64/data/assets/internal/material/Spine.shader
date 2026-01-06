Shader "Spine/Default" {
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
            Blend SrcAlpha OneMinusSrcAlpha
            ZWrite Off
            Cull Off
            
            CGPROGRAM
            #include "Spine.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
}