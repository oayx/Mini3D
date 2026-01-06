Shader "Hidden/Default" {
    Properties {
        _Color ("Color", Color) = (1,1,1,1)
        _MainTex ("Main Texture", 2D) = "white" {}
    }
    SubShader {
        Tags {
        }
        
        Pass {
            Tags { "LightMode" = "ForwardBase" }
            
            Cull Back
            ZWrite On
            ZTest LEqual
            ColorMask RGBA
            
            Blend SrcAlpha OneMinusSrcAlpha
            
            CGPROGRAM
            #include "Default.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
}