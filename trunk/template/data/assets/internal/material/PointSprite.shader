<Shader Group="Point/Sprite">
    <Tag CastShadow="false" ReceiveShadow="false"/>
    <Properties/>
    <TextureUnits>
        <TextureUnit Type="2D" Name="_MainTex"/>
    </TextureUnits>
    <Pass>
        <CG File="PointSprite.hlsl">
            <VS/>
            <GS/>
            <PS/>
            <Reflect>
                <Semantics Value="257"/>
                <CBuffers>
                    <CBuffer Name="CameraBuffer" Slot="11" Size="336" Stage="3">
                        <Variables>
                            <Variable Name="MATRIX_I_VP" Offset="192" Size="64" Stage="3"/>
                            <Variable Name="MATRIX_P" Offset="64" Size="64" Stage="3"/>
                            <Variable Name="MATRIX_V" Offset="0" Size="64" Stage="3"/>
                            <Variable Name="MATRIX_VP" Offset="128" Size="64" Stage="3"/>
                            <Variable Name="_ProjectionParams" Offset="288" Size="16" Stage="3"/>
                            <Variable Name="_ScreenParams" Offset="304" Size="16" Stage="3"/>
                            <Variable Name="_WorldSpaceCameraDir" Offset="272" Size="12" Stage="3"/>
                            <Variable Name="_WorldSpaceCameraPos" Offset="256" Size="12" Stage="3"/>
                            <Variable Name="_ZBufferParams" Offset="320" Size="16" Stage="3"/>
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
</Shader>
