Shader "Unlit/Color" {
    Properties {
        _Color ("Main Color", Color) = (1,1,1,1)
    }
    
    SubShader {
        Tags {
            "RenderType" = "Opaque"
            "Queue" = "Geometry"
        }
        
        Pass {
            ZWrite On
            ZTest LEqual
            
            CGPROGRAM
            #include "UnlitColor.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
}