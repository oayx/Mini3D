<Shader Group="Post Processing/SimpleBlur">
    <Properties/>
    <Pass>
        <CG File="SimpleBlur.hlsl">
            <VS/>
            <PS/>
            <Reflect>
                <Semantics Value="17"/>
                <CBuffers>
                    <CBuffer Name="CustomBuffer" Slot="0" Size="32" Stage="4">
                        <Variables>
                            <Variable Name="_BlurRadius" Offset="16" Size="4" Stage="4"/>
                            <Variable Name="_MainTex_TexelSize" Offset="0" Size="16" Stage="4"/>
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
