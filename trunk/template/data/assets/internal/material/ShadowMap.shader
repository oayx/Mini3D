<Shader Hide="true">
    <Tag CastShadow="false" ReceiveShadow="false"/>
    <Properties/>
    <Pass>
        <Tag LightMode="ShadowCaster"/>
        <Depth ZTest="true" ZWrite="true"/>
        <CG File="ShadowMap.hlsl">
            <VS/>
            <PS/>
            <Reflect>
                <Semantics Value="1"/>
                <CBuffers>
                    <CBuffer Name="CustomBuffer" Slot="0" Size="64" Stage="0">
                        <Variables>
                            <Variable Name="LIGHT_MATRIX_MVP" Offset="0" Size="64" Stage="0"/>
                        </Variables>
                    </CBuffer>
                </CBuffers>
            </Reflect>
        </CG>
    </Pass>
</Shader>
