<Material Group="Spine/Default">
    <Tag CastShadow="false" ReceiveShadow="false"/>
    <Properties/>
    <TextureUnits>
        <TextureUnit Type="2D" Name="_MainTex"/>
    </TextureUnits>
    <Pass>
        <AlphaBlend Enable="true"/>
        <CG File="Spine.hlsl">
            <VS/>
            <PS/>
            <Reflect>
                <Semantics Value="19"/>
                <CBuffers>
                    <CBuffer Name="CustomBuffer" Slot="0" Size="16" Stage="0">
                        <Variables>
                            <Variable Name="_MainTex_ST" Offset="0" Size="16" Stage="0"/>
                        </Variables>
                    </CBuffer>
                    <CBuffer Name="TransformBuffer" Slot="10" Size="512" Stage="0">
                        <Variables>
                            <Variable Name="MATRIX_IT_M" Offset="384" Size="64" Stage="0"/>
                            <Variable Name="MATRIX_IT_MV" Offset="448" Size="64" Stage="0"/>
                            <Variable Name="MATRIX_I_M" Offset="192" Size="64" Stage="0"/>
                            <Variable Name="MATRIX_M" Offset="64" Size="64" Stage="0"/>
                            <Variable Name="MATRIX_MV" Offset="128" Size="64" Stage="0"/>
                            <Variable Name="MATRIX_MVP" Offset="0" Size="64" Stage="0"/>
                            <Variable Name="MATRIX_T_M" Offset="256" Size="64" Stage="0"/>
                            <Variable Name="MATRIX_T_MV" Offset="320" Size="64" Stage="0"/>
                        </Variables>
                    </CBuffer>
                </CBuffers>
                <Textures>
                    <Texture Name="_MainTex" Type="1" Slot="0" Stage="4"/>
                </Textures>
            </Reflect>
        </CG>
    </Pass>
</Material>
