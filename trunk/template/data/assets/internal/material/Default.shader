<Shader Hide="true">
    <Tag RenderType="" RenderQueue="2000" EnableLight="false"/>
    <Properties>
        <Propertie Name="_Color" Type="color" Value="1,1,1,1"/>
    </Properties>
    <TextureUnits>
        <TextureUnit Type="2D" Name="_MainTex" Scale="1,1" Offset="0,0"/>
    </TextureUnits>
    <Pass>
        <Tag LightMode="ForwardBase"/>
        <!--剔除模式:off,front,back-->
        <CullMode Type="back"/>
        <!--mask,ColorMask可设置0|1-->
        <Mask ColorMask="1,1,1,1"/>
        <!--深度比较函数:Never,Less,Equal,LessEqual,Greater,NotEqual,GreaterEqual,Always-->
        <Depth ZTest="true" ZWrite="true" CmpFun="LessEqual"/>
        <!--模板比较函数:Never,Less,Equal,LessEqual,Greater,NotEqual,GreaterEqual,Always 结果操作:Keep,Zero,Replace,Incr,Incr_Wrap,Decr,Decr_Wrap -->
        <Stencil Enable="false" CmpFun="Always" Ref="1" ReadMask="255" WriteMask="255" FailOp="Keep" DepthFailOp="Keep" PassOp="Keep"/>
        <!--alpha混合-->
        <AlphaBlend Enable="true" SrcBlend="SrcAlpha" DstBlend="OneMinusSrcAlpha" SrcAlphaBlend="One" DstAlphaBlend="Zero"/>
        <!--alpha测试:Never,Less,Equal,LessEqual,Greater,NotEqual,GreaterEqual,Always-->
        <AlphaTest Enable="false" CmpFun="Greater"/>
        <CG File="Default.hlsl">
            <VS/>
            <PS/>
            <Reflect>
                <Semantics Value="19"/>
                <CBuffers>
                    <CBuffer Name="CommbonRarelyBuffer" Slot="13" Size="48" Stage="4">
                        <Variables>
                            <Variable Name="_FogColor" Offset="16" Size="16" Stage="4"/>
                            <Variable Name="_FogDensity" Offset="40" Size="4" Stage="4"/>
                            <Variable Name="_FogEnd" Offset="36" Size="4" Stage="4"/>
                            <Variable Name="_FogMode" Offset="12" Size="4" Stage="4"/>
                            <Variable Name="_FogStart" Offset="32" Size="4" Stage="4"/>
                            <Variable Name="_LIGHTMODEL_AMBIENT" Offset="0" Size="12" Stage="4"/>
                        </Variables>
                    </CBuffer>
                    <CBuffer Name="CustomBuffer" Slot="0" Size="32" Stage="0">
                        <Variables>
                            <Variable Name="_Color" Offset="0" Size="16" Stage="0"/>
                            <Variable Name="_MainTex_ST" Offset="16" Size="16" Stage="0"/>
                        </Variables>
                    </CBuffer>
                    <CBuffer Name="TransformBuffer" Slot="10" Size="512" Stage="0">
                        <Variables>
                            <Variable Name="MATRIX_IT_M" Offset="384" Size="64" Stage="0"/>
                            <Variable Name="MATRIX_IT_MV" Offset="448" Size="64" Stage="0"/>
                            <Variable Name="MATRIX_I_M" Offset="192" Size="64" Stage="0"/>
                            <Variable Name="MATRIX_M" Offset="64" Size="64" Stage="0"/>
                            <Variable Name="MATRIX_MV" Offset="128" Size="64" Stage="0"/>
                            <Variable Name="MATRIX_MVP" Offset="0" Size="64" Stage="0"/>
                            <Variable Name="MATRIX_T_M" Offset="256" Size="64" Stage="0"/>
                            <Variable Name="MATRIX_T_MV" Offset="320" Size="64" Stage="0"/>
                        </Variables>
                    </CBuffer>
                </CBuffers>
                <Textures>
                    <Texture Name="_MainTex" Type="1" Slot="0" Stage="4"/>
                </Textures>
            </Reflect>
        </CG>
    </Pass>
</Shader>
