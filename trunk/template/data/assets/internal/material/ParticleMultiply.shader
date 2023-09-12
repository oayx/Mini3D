<Shader Group="Particle/Multiply">
    <Tag RenderQueue="3000"/>
    <Properties>
        <Propertie Name="_TintColor" Type="color" Value="1,1,1,1"/>
        <Propertie Name="_USpeed" Type="float" Value="0"/>
        <Propertie Name="_VSpeed" Type="float" Value="0"/>
    </Properties>
    <TextureUnits>
        <TextureUnit Type="2D" Name="_MainTex"/>
    </TextureUnits>
    <Pass>
        <CullMode Type="off"/>
        <Mask ColorMask="1,1,1,1"/>
        <Depth ZTest="true" ZWrite="false"/>
        <AlphaBlend Enable="true" SrcBlend="Zero" DstBlend="SrcColor"/>
        <CG File="ParticleMultiply.hlsl">
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
                    <CBuffer Name="TransformBuffer" Slot="0" Size="512" Stage="4">
                        <Variables>
                            <Variable Name="MATRIX_IT_M" Offset="384" Size="64" Stage="0"/>
                            <Variable Name="MATRIX_IT_MV" Offset="448" Size="64" Stage="0"/>
                            <Variable Name="MATRIX_I_M" Offset="192" Size="64" Stage="0"/>
                            <Variable Name="MATRIX_M" Offset="64" Size="64" Stage="0"/>
                            <Variable Name="MATRIX_MV" Offset="128" Size="64" Stage="0"/>
                            <Variable Name="MATRIX_MVP" Offset="0" Size="64" Stage="0"/>
                            <Variable Name="MATRIX_T_M" Offset="256" Size="64" Stage="0"/>
                            <Variable Name="MATRIX_T_MV" Offset="320" Size="64" Stage="0"/>
                            <Variable Name="_MainTex_ST" Offset="0" Size="16" Stage="4"/>
                            <Variable Name="_Time" Offset="16" Size="16" Stage="4"/>
                            <Variable Name="_TintColor" Offset="32" Size="16" Stage="4"/>
                            <Variable Name="_USpeed" Offset="48" Size="4" Stage="4"/>
                            <Variable Name="_VSpeed" Offset="52" Size="4" Stage="4"/>
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
