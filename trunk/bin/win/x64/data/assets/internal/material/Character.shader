Shader "Character/Default" {
    Properties {
        _MainTex ("Main Texture", 2D) = "white" {}
    }

    SubShader {
        Tags { "RenderType"="Opaque" "EnableLight"="true" }

        // 主光照Pass
        Pass {
            Tags { "LightMode" = "ForwardBase" }

            CGPROGRAM
            #include "Character.hlsl" 
            #pragma vertex vs_main
            #pragma fragment ps_main
            ENDCG
        }

        // 阴影投射Pass
        Pass {
            Tags { "LightMode" = "ShadowCaster" }

            CGPROGRAM
            #include "Character.hlsl" 
            #pragma vertex vs_shadow
            #pragma fragment ps_shadow
            ENDCG
        }
    }
}