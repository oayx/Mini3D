<Shader Group="Water/Water">
    <Tag RenderQueue="2600"/>
    <Properties>
        <Propertie Name="_TintColor" Type="color" Value="1,1,1,1"/>
        <Propertie Name="_Speed" Type="float" Value="1"/>
        <Propertie Name="_Distort" Type="float" Value="1"/>
        <Propertie Name="_Intensity" Type="float" Value="1"/>
        <Propertie Name="_Alpha" Type="float" Value="1"/>
        <Propertie Name="_FresnelIndensity" Type="Slider" Value="0.5" Range="0,1"/>
    </Properties>
    <TextureUnits>
        <TextureUnit Type="2D" Name="_MainTex"/>
    </TextureUnits>
    <Pass>
        <CullMode Type="off"/>
        <Mask ColorMask="1,1,1,1"/>
        <Depth ZTest="true" ZWrite="false"/>
        <AlphaBlend Enable="true" SrcBlend="SrcAlpha" DstBlend="One"/>
        <CG File="Water.hlsl">
            <VS/>
            <PS/>
            <Reflect>
                <Semantics Value="21"/>
                <CBuffers>
                    <CBuffer Name="CameraBuffer" Slot="11" Size="336" Stage="4">
                        <Variables>
                            <Variable Name="MATRIX_I_VP" Offset="192" Size="64" Stage="4"/>
                            <Variable Name="MATRIX_P" Offset="64" Size="64" Stage="4"/>
                            <Variable Name="MATRIX_V" Offset="0" Size="64" Stage="4"/>
                            <Variable Name="MATRIX_VP" Offset="128" Size="64" Stage="4"/>
                            <Variable Name="_ProjectionParams" Offset="288" Size="16" Stage="4"/>
                            <Variable Name="_ScreenParams" Offset="304" Size="16" Stage="4"/>
                            <Variable Name="_WorldSpaceCameraDir" Offset="272" Size="12" Stage="4"/>
                            <Variable Name="_WorldSpaceCameraPos" Offset="256" Size="12" Stage="4"/>
                            <Variable Name="_ZBufferParams" Offset="320" Size="16" Stage="4"/>
                        </Variables>
                    </CBuffer>
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
                    <CBuffer Name="CustomBuffer" Slot="0" Size="80" Stage="4">
                        <Variables>
                            <Variable Name="_Alpha" Offset="44" Size="4" Stage="4"/>
                            <Variable Name="_Distort" Offset="36" Size="4" Stage="4"/>
                            <Variable Name="_FresnelIndensity" Offset="64" Size="4" Stage="4"/>
                            <Variable Name="_Intensity" Offset="40" Size="4" Stage="4"/>
                            <Variable Name="_MainTex_ST" Offset="0" Size="16" Stage="4"/>
                            <Variable Name="_Speed" Offset="32" Size="4" Stage="4"/>
                            <Variable Name="_Time" Offset="48" Size="16" Stage="4"/>
                            <Variable Name="_TintColor" Offset="16" Size="16" Stage="4"/>
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
                    <Texture Name="_SkyBox" Type="3" Slot="1" Stage="4"/>
                </Textures>
            </Reflect>
        </CG>
    </Pass>
</Shader>
