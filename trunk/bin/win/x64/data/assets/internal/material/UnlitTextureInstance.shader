Shader "Unlit/Instance/Texture" {
    Properties {
        _MainTex ("Main Texture", 2D) = "white" {}
        _Color ("Tint Color", Color) = (1,1,1,1)
    }

    SubShader {
        Tags {
            "RenderType" = "Opaque"
            "Queue" = "Geometry"
        }

        // 主渲染通道
        Pass {
            ZWrite On
            ZTest LEqual

            CGPROGRAM
            #include "UnlitTextureInstance.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }

        // 阴影投射通道
        Pass {
            Tags { "LightMode" = "ShadowCaster" }
            
            ZWrite On
            ZTest LEqual

            CGPROGRAM
            #include "UnlitTextureInstance.hlsl" 
            #pragma vertex vs_shadow
            #pragma fragment ps_shadow
            ENDCG
        }

        // 深度通道
        Pass {
            Tags { "LightMode" = "Depth" }
            
            ZWrite On
            ZTest LEqual

            CGPROGRAM
            #include "UnlitTextureInstance.hlsl" 
            #pragma vertex vs_depth
            #pragma fragment ps_depth
            ENDCG
        }

        // 深度法线通道
        Pass {
            Tags { "LightMode" = "DepthNormal" }
            
            ZWrite On
            ZTest LEqual

            CGPROGRAM
            #include "UnlitTextureInstance.hlsl" 
            #pragma vertex vs_depthnormal
            #pragma fragment ps_depthnormal
            ENDCG
        }
    }
}