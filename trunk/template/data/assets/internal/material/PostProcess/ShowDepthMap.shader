<Shader Hide="true">
    <Properties/>
    <TextureUnits>
        <TextureUnit Type="2D" Name="_CameraDepthTexture"/>
    </TextureUnits>
    <Pass>
        <CG File="ShowDepthMap.hlsl">
            <VS/>
            <PS/>
            <Reflect>
                <Semantics Value="17"/>
                <Textures>
                    <Texture Name="_CameraDepthTexture" Type="1" Slot="0" Stage="4"/>
                </Textures>
            </Reflect>
        </CG>
    </Pass>
</Shader>
