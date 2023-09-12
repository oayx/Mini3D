<Shader Group="ShadowMap/Receive">
    <Tag EnableLight="true"/>
    <Properties>
        <Propertie Name="_Bias" Alias="Bias" Type="Slider" Range="0,2" Value="0"/>
    </Properties>
    <TextureUnits>
        <TextureUnit Type="2D" Scale="1,1" Offset="0,0"/>
    </TextureUnits>
    <Pass>
        <Mask ColorMask="1,1,1,1" DepthMask="true" StencilMask="255"/>
        <Depth ZTest="true" ZWrite="true" CmpFun="LessEqual"/>
        <CG File="ShadowMapReceive.hlsl">
            <VS/>
            <PS/>
            <Reflect>
                <Semantics Value="17"/>
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
                    <CBuffer Name="CustomBuffer" Slot="0" Size="112" Stage="4">
                        <Variables>
                            <Variable Name="MATRIX_MVP_LIGHT" Offset="0" Size="64" Stage="4"/>
                            <Variable Name="_Bias" Offset="96" Size="4" Stage="4"/>
                            <Variable Name="_MainTex_ST" Offset="64" Size="16" Stage="4"/>
                            <Variable Name="_ShadowMap_TexelSize" Offset="80" Size="16" Stage="4"/>
                            <Variable Name="_ShadowType" Offset="100" Size="4" Stage="4"/>
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
                    <Texture Name="_ShadowMap" Type="1" Slot="1" Stage="4"/>
                </Textures>
            </Reflect>
        </CG>
    </Pass>
</Shader>
