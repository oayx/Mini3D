<Shader Hide="true">
    <Tag CastShadow="false" ReceiveShadow="false"/>
    <Properties/>
    <Pass>
        <Tag LightMode="DepthNormal"/>
        <Depth ZTest="true" ZWrite="true"/>
        <CG File="RenderDepthNormalMap.hlsl">
            <VS/>
            <PS/>
            <Reflect>
                <Semantics Value="5"/>
                <CBuffers>
                    <CBuffer Name="CameraBuffer" Slot="11" Size="336" Stage="0">
                        <Variables>
                            <Variable Name="MATRIX_I_VP" Offset="192" Size="64" Stage="0"/>
                            <Variable Name="MATRIX_P" Offset="64" Size="64" Stage="0"/>
                            <Variable Name="MATRIX_V" Offset="0" Size="64" Stage="0"/>
                            <Variable Name="MATRIX_VP" Offset="128" Size="64" Stage="0"/>
                            <Variable Name="_ProjectionParams" Offset="288" Size="16" Stage="0"/>
                            <Variable Name="_ScreenParams" Offset="304" Size="16" Stage="0"/>
                            <Variable Name="_WorldSpaceCameraDir" Offset="272" Size="12" Stage="0"/>
                            <Variable Name="_WorldSpaceCameraPos" Offset="256" Size="12" Stage="0"/>
                            <Variable Name="_ZBufferParams" Offset="320" Size="16" Stage="0"/>
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
            </Reflect>
        </CG>
    </Pass>
</Shader>
