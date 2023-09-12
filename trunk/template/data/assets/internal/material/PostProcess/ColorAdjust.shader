<Shader Group="Post Processing/ColorAdjust">
    <Properties/>
    <Pass>
        <CG File="ColorAdjust.hlsl">
            <VS/>
            <PS/>
            <Reflect>
                <Semantics Value="17"/>
                <CBuffers>
                    <CBuffer Name="CustomBuffer" Slot="0" Size="16" Stage="4">
                        <Variables>
                            <Variable Name="_Brightness" Offset="0" Size="4" Stage="4"/>
                            <Variable Name="_Contrast" Offset="8" Size="4" Stage="4"/>
                            <Variable Name="_Saturation" Offset="4" Size="4" Stage="4"/>
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
