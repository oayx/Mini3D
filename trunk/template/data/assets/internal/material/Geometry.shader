<Shader Hide="true">
    <Properties>
        <Propertie Name="_Color" Type="color" Value="1,1,1,1"/>
    </Properties>
    <Pass>
        <CG File="Geometry.hlsl">
            <VS/>
            <GS/>
            <PS/>
            <Reflect>
                <Semantics Value="3"/>
                <CBuffers>
                    <CBuffer Name="CustomBuffer" Slot="0" Size="16" Stage="0">
                        <Variables>
                            <Variable Name="_Color" Offset="0" Size="16" Stage="0"/>
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
