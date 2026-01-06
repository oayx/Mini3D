Shader "Hidden/ShowDepth" {
    Properties {
        _CameraDepthTexture ("Base (RGB)", 2D) = "white" {}
    }

    SubShader {
        Pass {
            CGPROGRAM
            #include "ShowDepthMap.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
}