<Shader Group="Post Processing/EdgeDetection">
    <Properties/>
    <Pass>
        <CG File="MotionBlur.hlsl">
            <VS/>
            <PS/>
            <Reflect>
                <Semantics Value="17"/>
                <CBuffers>
                    <CBuffer Name="CameraBuffer" Slot="11" Size="336" Stage="4">
                        <Variables>
                            <Variable Name="MATRIX_I_VP" Offset="192" Size="64" Stage="4"/>
                            <Variable Name="MATRIX_P" Offset="64" Size="64" Stage="4"/>
                            <Variable Name="MATRIX_V" Offset="0" Size="64" Stage="4"/>
                            <Variable Name="MATRIX_VP" Offset="128" Size="64" Stage="4"/>
                            <Variable Name="_ProjectionParams" Offset="288" Size="16" Stage="4"/>
                            <Variable Name="_ScreenParams" Offset="304" Size="16" Stage="4"/>
                            <Variable Name="_WorldSpaceCameraDir" Offset="272" Size="12" Stage="4"/>
                            <Variable Name="_WorldSpaceCameraPos" Offset="256" Size="12" Stage="4"/>
                            <Variable Name="_ZBufferParams" Offset="320" Size="16" Stage="4"/>
                        </Variables>
                    </CBuffer>
                    <CBuffer Name="CustomBuffer" Slot="0" Size="96" Stage="4">
                        <Variables>
                            <Variable Name="_Intensity" Offset="16" Size="16" Stage="4"/>
                            <Variable Name="_MainTex_TexelSize" Offset="0" Size="16" Stage="4"/>
                            <Variable Name="_PRE_MATRIX_VP" Offset="32" Size="64" Stage="4"/>
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
