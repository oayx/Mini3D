Shader "Skybox/Cubemap" {
    Properties {
        _MainTex ("Cubemap", Cube) = "internal/cubemap/sky.cube" {}
    }
    
    SubShader {
        Tags {
            "Queue" = "Background"
            "RenderType" = "Skybox"
            "CastShadow" = "False"
        }
        
        Pass {
            Cull Front
            ZTest LEqual
            ZWrite On
            
            CGPROGRAM
            #include "SkyBox.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
}