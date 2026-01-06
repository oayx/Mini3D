Shader "Terrain/Terrain2" {
    Properties {
        _ControlTex ("Control Texture", 2D) = "black" {}
        _MainTex0 ("Layer 0 Albedo", 2D) = "white" {}
        _NormalTexture0 ("Layer 0 Normal", 2D) = "bump" {}
        _MainTex1 ("Layer 1 Albedo", 2D) = "white" {}
        _NormalTexture1 ("Layer 1 Normal", 2D) = "bump" {}
        _SpecularPower ("Specular Power", Range(0,1)) = 0.1
    }

    SubShader {
        Tags {
            "Queue" = "1500"
            "RenderType" = "Opaque"
            "CastShadow" = "False"
            "EnableLight" = "true"
        }

        Pass {
            
            CGPROGRAM
            #include "Terrain2.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
}