Shader "Particle/Multiply" {
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
            Blend Zero SrcColor

            CGPROGRAM
            #include "ParticleMultiply.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
}