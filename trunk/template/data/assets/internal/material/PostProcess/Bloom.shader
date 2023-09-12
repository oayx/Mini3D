<Shader Group="Post Processing/Bloom">
    <TextureUnits>
        <TextureUnit Type="2D" Name="_MainTex"/>
        <TextureUnit Name="_BlurTex"/>
    </TextureUnits>
    <Pass>
        <CG File="Bloom.hlsl">
            <VS Enter="vert_threshold"/>
            <PS Enter="frag_threshold"/>
            <Reflect>
                <Semantics Value="17"/>
                <CBuffers>
                    <CBuffer Name="CustomBuffer" Slot="0" Size="80" Stage="4">
                        <Variables>
                            <Variable Name="_BloomColor" Offset="48" Size="16" Stage="4"/>
                            <Variable Name="_BloomFactor" Offset="64" Size="4" Stage="4"/>
                            <Variable Name="_BlurOffsets" Offset="32" Size="16" Stage="4"/>
                            <Variable Name="_ColorThreshold" Offset="16" Size="16" Stage="4"/>
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
    <Pass>
        <CG File="Bloom.hlsl">
            <VS Enter="vert_blur"/>
            <PS Enter="frag_blur"/>
            <Reflect>
                <Semantics Value="17"/>
                <CBuffers>
                    <CBuffer Name="CustomBuffer" Slot="0" Size="80" Stage="0">
                        <Variables>
                            <Variable Name="_BloomColor" Offset="48" Size="16" Stage="0"/>
                            <Variable Name="_BloomFactor" Offset="64" Size="4" Stage="0"/>
                            <Variable Name="_BlurOffsets" Offset="32" Size="16" Stage="0"/>
                            <Variable Name="_ColorThreshold" Offset="16" Size="16" Stage="0"/>
                            <Variable Name="_MainTex_TexelSize" Offset="0" Size="16" Stage="0"/>
                        </Variables>
                    </CBuffer>
                </CBuffers>
                <Textures>
                    <Texture Name="_MainTex" Type="1" Slot="0" Stage="4"/>
                </Textures>
            </Reflect>
        </CG>
    </Pass>
    <Pass>
        <CG File="Bloom.hlsl">
            <VS Enter="vert_bloom"/>
            <PS Enter="frag_bloom"/>
            <Reflect>
                <Semantics Value="17"/>
                <CBuffers>
                    <CBuffer Name="CustomBuffer" Slot="0" Size="80" Stage="4">
                        <Variables>
                            <Variable Name="_BloomColor" Offset="48" Size="16" Stage="4"/>
                            <Variable Name="_BloomFactor" Offset="64" Size="4" Stage="4"/>
                            <Variable Name="_BlurOffsets" Offset="32" Size="16" Stage="4"/>
                            <Variable Name="_ColorThreshold" Offset="16" Size="16" Stage="4"/>
                            <Variable Name="_MainTex_TexelSize" Offset="0" Size="16" Stage="4"/>
                        </Variables>
                    </CBuffer>
                </CBuffers>
                <Textures>
                    <Texture Name="_BlurTex" Type="1" Slot="1" Stage="4"/>
                    <Texture Name="_MainTex" Type="1" Slot="0" Stage="4"/>
                </Textures>
            </Reflect>
        </CG>
    </Pass>
</Shader>
