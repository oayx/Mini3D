<Shader Group="Post Processing/DepthOfField">
    <TextureUnits>
        <TextureUnit Type="2D" Name="_MainTex"/>
        <TextureUnit Name="_BlurTex"/>
    </TextureUnits>
    <Pass>
        <CG File="DepthOfField.hlsl">
            <VS Enter="vert_blur"/>
            <PS Enter="frag_blur"/>
            <Reflect>
                <Semantics Value="17"/>
                <CBuffers>
                    <CBuffer Name="CustomBuffer" Slot="0" Size="48" Stage="0">
                        <Variables>
                            <Variable Name="_BlurOffsets" Offset="16" Size="16" Stage="0"/>
                            <Variable Name="_FarBlurScale" Offset="44" Size="4" Stage="0"/>
                            <Variable Name="_FocalDistance" Offset="32" Size="4" Stage="0"/>
                            <Variable Name="_FocalLength" Offset="36" Size="4" Stage="0"/>
                            <Variable Name="_MainTex_TexelSize" Offset="0" Size="16" Stage="0"/>
                            <Variable Name="_NearBlurScale" Offset="40" Size="4" Stage="0"/>
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
        <CG File="DepthOfField.hlsl">
            <VS Enter="vert_dof"/>
            <PS Enter="frag_dof"/>
            <Reflect>
                <Semantics Value="17"/>
                <CBuffers>
                    <CBuffer Name="CustomBuffer" Slot="0" Size="48" Stage="4">
                        <Variables>
                            <Variable Name="_BlurOffsets" Offset="16" Size="16" Stage="4"/>
                            <Variable Name="_FarBlurScale" Offset="44" Size="4" Stage="4"/>
                            <Variable Name="_FocalDistance" Offset="32" Size="4" Stage="4"/>
                            <Variable Name="_FocalLength" Offset="36" Size="4" Stage="4"/>
                            <Variable Name="_MainTex_TexelSize" Offset="0" Size="16" Stage="4"/>
                            <Variable Name="_NearBlurScale" Offset="40" Size="4" Stage="4"/>
                        </Variables>
                    </CBuffer>
                </CBuffers>
                <Textures>
                    <Texture Name="_BlurTex" Type="1" Slot="1" Stage="4"/>
                    <Texture Name="_CameraDepthTexture" Type="1" Slot="2" Stage="4"/>
                    <Texture Name="_MainTex" Type="1" Slot="0" Stage="4"/>
                </Textures>
            </Reflect>
        </CG>
    </Pass>
</Shader>
