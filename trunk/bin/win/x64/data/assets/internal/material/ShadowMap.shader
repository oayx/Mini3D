Shader "Hidden/ShadowMap" {
    SubShader {
        Tags 
        { 
            "CastShadow" = "False"
        }
        
        Pass {
            // 阴影投射通道
            Tags { "LightMode" = "ShadowCaster" }
            
            // 深度测试配置
            ZTest LEqual
            ZWrite On
            
            CGPROGRAM
            #include "ShadowMap.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }
    }
}
