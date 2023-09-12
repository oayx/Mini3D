<Shader Hide="true">
    <Properties/>
    <TextureUnits>
        <TextureUnit Type="2D" Name="_CameraDepthNormalTexture"/>
    </TextureUnits>
    <Pass>
        <CG File="ShowDepthNormal.hlsl">
            <VS/>
            <PS/>
            <Reflect>
                <Semantics Value="17"/>
                <Textures>
                    <Texture Name="_CameraDepthNormalTexture" Type="1" Slot="0" Stage="4"/>
                </Textures>
            </Reflect>
        </CG>
    </Pass>
</Shader>
