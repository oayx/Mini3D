Shader "Hidden/GUI" {
    Properties {
        _MainTex ("Main Texture", 2D) = "white" {}
    }
    
    SubShader {
        Tags {
            "Queue" = "Overlay"
            "CastShadow" = "False"
            "ReceiveShadow" = "False"
        }
        
        Pass {
            Tags { 
            }
            
            Cull Off
            ZTest Always
            ZWrite Off
            Blend SrcAlpha OneMinusSrcAlpha
            
            CGPROGRAM
            #include "GUI.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
}