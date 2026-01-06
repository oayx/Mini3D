Shader "Hidden/ShowDepth" {
    Properties {
        _CameraDepthNormalTexture ("Base (RGB)", 2D) = "white" {}
    }
    SubShader {
        Pass {
            CGPROGRAM
            #include "ShowDepthNormal.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
}