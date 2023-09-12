<Shader Group="Character/Default">
    <Tag EnableLight="true"/>
    <Properties/>
    <TextureUnits>
        <TextureUnit Type="2D" Name="_MainTex"/>
    </TextureUnits>
    <Pass>
        <CG File="Character.hlsl">
            <VS/>
            <PS/>
            <Reflect>
                <Semantics Value="221"/>
                <CBuffers>
                    <CBuffer Name="CommbonRarelyBuffer" Slot="13" Size="48" Stage="4">
                        <Variables>
                            <Variable Name="_FogColor" Offset="16" Size="16" Stage="4"/>
                            <Variable Name="_FogDensity" Offset="40" Size="4" Stage="4"/>
                            <Variable Name="_FogEnd" Offset="36" Size="4" Stage="4"/>
                            <Variable Name="_FogMode" Offset="12" Size="4" Stage="4"/>
                            <Variable Name="_FogStart" Offset="32" Size="4" Stage="4"/>
                            <Variable Name="_LIGHTMODEL_AMBIENT" Offset="0" Size="12" Stage="4"/>
                        </Variables>
                    </CBuffer>
                    <CBuffer Name="CustomBuffer" Slot="0" Size="12880" Stage="0">
                        <Variables>
                            <Variable Name="BoneMatrixArray" Offset="80" Size="12800" Stage="0"/>
                            <Variable Name="LIGHT_MATRIX_MVP" Offset="0" Size="64" Stage="0"/>
                            <Variable Name="_MainTex_ST" Offset="64" Size="16" Stage="0"/>
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
    <Pass>
        <Tag LightMode="ShadowCaster"/>
        <CG File="Character.hlsl">
            <VS Enter="vs_shadow"/>
            <PS Enter="ps_shadow"/>
            <Reflect>
                <Semantics Value="221"/>
                <CBuffers>
                    <CBuffer Name="CustomBuffer" Slot="0" Size="12880" Stage="0">
                        <Variables>
                            <Variable Name="BoneMatrixArray" Offset="80" Size="12800" Stage="0"/>
                            <Variable Name="LIGHT_MATRIX_MVP" Offset="0" Size="64" Stage="0"/>
                            <Variable Name="_MainTex_ST" Offset="64" Size="16" Stage="0"/>
                        </Variables>
                    </CBuffer>
                </CBuffers>
            </Reflect>
        </CG>
    </Pass>
</Shader>
