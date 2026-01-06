Shader "Terrain/Terrain3" {
    Properties {
        _ControlTex ("Control", 2D) = "black" {}
        _MainTex0 ("Layer0 Albedo", 2D) = "white" {}
        _NormalTexture0 ("Layer0 Normal", 2D) = "bump" {}
        _MainTex1 ("Layer1 Albedo", 2D) = "white" {}
        _NormalTexture1 ("Layer1 Normal", 2D) = "bump" {}
        _MainTex2 ("Layer2 Albedo", 2D) = "white" {}
        _NormalTexture2 ("Layer2 Normal", 2D) = "bump" {}
        _SpecularPower ("Specular", Range(0,1)) = 0.1
    }

    SubShader {
        Tags {
            "Queue" = "1500" 
            "RenderType" = "Opaque"
            "CastShadow" = "False"
        }

        Pass {
            CGPROGRAM
            #include "Terrain3.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
}