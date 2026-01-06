Shader "Particle/AlphaBlend" {
    Properties {
        _TintColor ("Tint Color", Color) = (1,1,1,1)
        _MainTex ("Particle Texture", 2D) = "white" {}
        _USpeed ("U Scroll Speed", Float) = 0
        _VSpeed ("V Scroll Speed", Float) = 0
    }

    SubShader {
        Tags {
            "Queue" = "Transparent"
            "RenderType" = "Transparent"
            "CastShadow" = "False"
        }

        Pass {
            Cull Off
            ZWrite Off
            ZTest LEqual
            Blend SrcAlpha OneMinusSrcAlpha

            CGPROGRAM
            #include "ParticleAlphaBlend.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
}