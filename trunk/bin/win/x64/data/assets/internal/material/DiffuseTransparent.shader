Shader "Transparent/Diffuse" {
    Properties {
        _MainTex ("Main Texture", 2D) = "white" {}
        _SpecularPower ("Specular Power", Float) = 1.0
    }
    
    SubShader {
        Tags {
            "RenderType" = "Transparent"
            "Queue" = "Transparent"
            "EnableLight" = "true"
        }
        
        Pass {
            Tags { "LightMode" = "ForwardBase" }
            
            Blend SrcAlpha OneMinusSrcAlpha
            
            CGPROGRAM
            #include "DiffuseTransparent.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
}