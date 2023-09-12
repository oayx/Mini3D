<Shader Hide="true">
    <Tag RenderQueue="4000" CastShadow="false" ReceiveShadow="false"/>
    <Properties/>
    <TextureUnits>
        <TextureUnit Type="2D" Name="_MainTex"/>
    </TextureUnits>
    <Pass>
        <CullMode Type="off"/>
        <Depth ZTest="false" ZWrite="false" CmpFun="Always"/>
        <Stencil Enable="false" CmpFun="Always"/>
        <AlphaBlend Enable="true"/>
        <CG File="GUI.hlsl">
            <VS/>
            <PS/>
            <Reflect>
                <Semantics Value="19"/>
                <CBuffers>
                    <CBuffer Name="CustomBuffer" Slot="0" Size="64" Stage="0">
                        <Variables>
                            <Variable Name="ProjectionMatrix" Offset="0" Size="64" Stage="0"/>
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
