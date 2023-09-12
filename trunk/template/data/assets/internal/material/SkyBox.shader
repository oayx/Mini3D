<Shader Group="Skybox/Cubemap">
    <Tag CastShadow="false" ReceiveShadow="false"/>
    <Properties/>
    <TextureUnits>
        <TextureUnit Type="cube" Name="_MainTex" File="internal/cubemap/sky.cube"/>
    </TextureUnits>
    <Pass>
        <CullMode Type="front"/>
        <Depth ZTest="true" ZWrite="true"/>
        <CG File="SkyBox.hlsl">
            <VS/>
            <PS/>
            <Reflect>
                <Semantics Value="1"/>
                <CBuffers>
                    <CBuffer Name="CustomBuffer" Slot="0" Size="64" Stage="0">
                        <Variables>
                            <Variable Name="MATRIX_SKYBOX" Offset="0" Size="64" Stage="0"/>
                        </Variables>
                    </CBuffer>
                </CBuffers>
                <Textures>
                    <Texture Name="_MainTex" Type="3" Slot="0" Stage="4"/>
                </Textures>
            </Reflect>
        </CG>
    </Pass>
</Shader>
