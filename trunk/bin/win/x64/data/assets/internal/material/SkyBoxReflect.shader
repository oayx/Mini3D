Shader "Hidden/SkyBoxReflect" {
    Properties {
        _MainTex ("Main Texture", 2D) = "white" {}
    }
    
    SubShader {
        Tags {
            "CastShadow" = "False"
        }
        
        Pass {
            Blend SrcAlpha OneMinusSrcAlpha

            CGPROGRAM
            #include "SkyBoxReflect.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
}