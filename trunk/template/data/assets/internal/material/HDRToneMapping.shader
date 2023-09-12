<Shader Hide="true">
    <Tag CastShadow="false" ReceiveShadow="false"/>
    <Properties>
        <Propertie Name="_Exposure" Type="float" Value="1"/>
    </Properties>
    <TextureUnits>
        <TextureUnit Type="2D" Name="_MainTex"/>
    </TextureUnits>
    <Pass>
        <CG File="HDRToneMapping.hlsl">
            <VS/>
            <PS/>
            <Reflect>
                <Semantics Value="17"/>
                <CBuffers>
                    <CBuffer Name="CustomBuffer" Slot="0" Size="16" Stage="4">
                        <Variables>
                            <Variable Name="_Exposure" Offset="0" Size="4" Stage="4"/>
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
