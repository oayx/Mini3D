#pragma once

#include <vector>
#include "PropertyDefined.h"

namespace shaderlab {
	enum class ASTNodeType
	{
		Unknow = -1,
		Shader,
		Category,
		Properties,
		Property,
		SubShader,
		LOD,
		Tags,
		UsePass,
		GrabPass,
		Pass,
		Blend,
		BlendOp,
		AlphaToMask,
		ColorMask,
		Cull,
		Offset,
		Conservative,
		ZClip,
		ZTest,
		ZWrite,
		Stencil,
		Fog,
		ShaderBlock,
		ShaderInclude,
		FallBack,
		CustomEditor,
		Dependency,
	};

	struct ASTNode {
		virtual ~ASTNode() = default;
		ASTNodeType Type = ASTNodeType::Unknow;
		std::string Value;
		size_t Line;
		std::vector<std::shared_ptr<ASTNode>> children;

		explicit ASTNode(ASTNodeType type, const Token& t) : Type(type)
		{
			Value = t.value;
			Line = t.line;
		}
	};

	struct ASTShaderNode : ASTNode {
		using ASTNode::ASTNode;
		explicit ASTShaderNode(const Token& t) : ASTNode(ASTNodeType::Shader, t) { }

		std::string Name;
	};

	/*
	document：https://docs.unity3d.com/Manual/SL-Other.html
	*/
	struct ASTCategoryNode : ASTNode {
		using ASTNode::ASTNode;
		explicit ASTCategoryNode(const Token& t) : ASTNode(ASTNodeType::Category, t) { }
	};

	struct ASTPropertiesNode : ASTNode {
		using ASTNode::ASTNode;
		explicit ASTPropertiesNode(const Token& t) : ASTNode(ASTNodeType::Properties, t) { }
	};
	struct ASTPropertyNode : ASTNode {
		using ASTNode::ASTNode;
		explicit ASTPropertyNode(const Token& t) : ASTNode(ASTNodeType::Property, t) { }

		std::string Header;
		std::string Name;					// 属性内部名称（如_MainTex）
		std::string DisplayName;			// 显示名称（如"Albedo Texture"）
		PropertyType PropType;
		PropertyRange Range;				// 仅当type=Range时有效
		std::string DefaultValue;
	};

	/*
	example：Fallback Off,Fallback "ExampleOtherShader"
	document：https://docs.unity3d.com/Manual/SL-Fallback.html
	*/
	struct ASTFallbackNode : ASTNode {
		using ASTNode::ASTNode;
		explicit ASTFallbackNode(const Token& t) : ASTNode(ASTNodeType::FallBack, t) { }

		std::string Value;
	};

	/*
	example：CustomEditor "custom editor class name"
	document：https://docs.unity3d.com/Manual/SL-CustomEditor.html
	*/
	struct ASTCustomEditorNode : ASTNode {
		using ASTNode::ASTNode;
		explicit ASTCustomEditorNode(const Token& t) : ASTNode(ASTNodeType::CustomEditor, t) { }

		std::string Value;
	};

	/*
	example：CustomEditor "custom editor class name"
	document：用于声明当前Shader对其他Shader依赖关系的指令。它的主要作用是确保被依赖的Shader在打包时被正确包含，避免运行时因缺失引用而导致的错误。以下是其核心要点：
	*/
	struct ASTDependencyNode : ASTNode {
		using ASTNode::ASTNode;
		explicit ASTDependencyNode(const Token& t) : ASTNode(ASTNodeType::Dependency, t) { }

		std::string Key; std::string Value;
	};

	/*
	document：https://docs.unity3d.com/Manual/SL-SubShader-object.html
	*/
	struct ASTSubShaderNode : ASTNode {
		using ASTNode::ASTNode;
		explicit ASTSubShaderNode(const Token& t) : ASTNode(ASTNodeType::SubShader, t) { }
	};

	/*
	example：LOD 100
	document：https://docs.unity3d.com/Manual/SL-ShaderLOD.html
	*/
	struct ASTLODNode : ASTNode {
		using ASTNode::ASTNode;
		explicit ASTLODNode(const Token& t) : ASTNode(ASTNodeType::LOD, t) { }

		int LOD;
	};

	/*
	example：Tags { "RenderType"="Opaque" }
	document：https://docs.unity3d.com/Manual/SL-SubShaderTags.html
	document：https://docs.unity3d.com/Manual/SL-PassTags.html
	*/
	struct ASTTagsNode : public ASTNode {
		using ASTNode::ASTNode;
		explicit ASTTagsNode(const Token& t) : ASTNode(ASTNodeType::Tags, t) { }

		std::unordered_map<std::string, std::string> tags;
	};

	/*
	example：UsePass "Shader object name/PASS NAME IN UPPERCASE"
	document：https://docs.unity3d.com/Manual/SL-UsePass.html
	*/
	struct ASTUsePassNode : ASTNode {
		using ASTNode::ASTNode;
		explicit ASTUsePassNode(const Token& t) : ASTNode(ASTNodeType::UsePass, t) { }

		std::string Name;//https://docs.unity3d.com/Manual/SL-Name.html
	};

	/*
	example：GrabPass { },GrabPass { "ExampleTextureName" }
	document：https://docs.unity3d.com/Manual/SL-GrabPass.html
	*/
	struct ASTGrabPassNode : ASTNode {
		using ASTNode::ASTNode;
		explicit ASTGrabPassNode(const Token& t) : ASTNode(ASTNodeType::GrabPass, t) { }

		std::string TextureName;
	};

	/*
	document：https://docs.unity3d.com/Manual/SL-SubShader-pass.html
	*/
	struct ASTPassNode : ASTNode {
		using ASTNode::ASTNode;
		explicit ASTPassNode(const Token& t) : ASTNode(ASTNodeType::Pass, t) { }

		std::string Name;
	};

	/*
	example：Blend Off, Blend One Zero, Blend 1 One Zero, Zero One
	document：https://docs.unity3d.com/Manual/SL-Blend.html
	*/
	struct ASTBlendNode : public ASTNode {
		using ASTNode::ASTNode;
		explicit ASTBlendNode(const Token& t) : ASTNode(ASTNodeType::Blend, t) { }

		struct BlendFactors {
			std::string src;
			std::string dst;
			std::string srcAlpha;
			std::string dstAlpha;
			bool separateAlpha = false;
		};

		bool blendOff = false;
		int renderTarget = -1;	//-1表示未设置
		BlendFactors factors;
	};

	/*
	example：BlendOp Sub
	document：https://docs.unity3d.com/Manual/SL-BlendOp.html
	*/
	struct ASTBlendOpNode : public ASTNode {
		using ASTNode::ASTNode;
		explicit ASTBlendOpNode(const Token& t) : ASTNode(ASTNodeType::BlendOp, t) { }

		std::string BlendOp;
	};

	/*
	example：AlphaToMask Off
	document：https://docs.unity3d.com/Manual/SL-AlphaToMask.html
	*/
	struct ASTAlphaToMaskNode : public ASTNode {
		using ASTNode::ASTNode;
		explicit ASTAlphaToMaskNode(const Token& t) : ASTNode(ASTNodeType::AlphaToMask, t) { }

		std::string Value;
	};

	/*
	example：ColorMask RGB 2
	document：https://docs.unity3d.com/Manual/SL-ColorMask.html
	*/
	struct ASTColorMaskNode : public ASTNode {
		using ASTNode::ASTNode;
		explicit ASTColorMaskNode(const Token& t) : ASTNode(ASTNodeType::ColorMask, t) { }

		std::string Value;
		int RenderTarget;//可选
	};

	/*
	example：Conservative True
	document：https://docs.unity3d.com/Manual/SL-Conservative.html
	*/
	struct ASTConservativeNode : public ASTNode {
		using ASTNode::ASTNode;
		explicit ASTConservativeNode(const Token& t) : ASTNode(ASTNodeType::Conservative, t) { }

		std::string Value;
	};

	/*
	example：Cull Back
	document：https://docs.unity3d.com/Manual/SL-Cull.html
	*/
	struct ASTCullNode : public ASTNode {
		using ASTNode::ASTNode;
		explicit ASTCullNode(const Token& t) : ASTNode(ASTNodeType::Cull, t) { }

		std::string Value;
	};

	/*
	example：Offset 1, 1
	document：https://docs.unity3d.com/Manual/SL-Offset.html
	*/
	struct ASTOffsetNode : public ASTNode {
		using ASTNode::ASTNode;
		explicit ASTOffsetNode(const Token& t) : ASTNode(ASTNodeType::Offset, t) { }

		std::string Factor;	//[-1,1]
		std::string Units;	//[-1,1]
	};

	/*
	example:
	Stencil
	{
		Ref 2
		Comp equal
		Pass keep
		ZFail decrWrap
	}
	document：https://docs.unity3d.com/Manual/SL-Stencil.html
	*/
	struct ASTStencilNode : public ASTNode {
		using ASTNode::ASTNode;
		explicit ASTStencilNode(const Token& t) : ASTNode(ASTNodeType::Stencil, t) { }

		std::unordered_map<std::string, std::string> Values;
	};

	/*
	example：ZTest Less
	document：https://docs.unity3d.com/Manual/SL-ZTest.html
	*/
	struct ASTZTestNode : public ASTNode {
		using ASTNode::ASTNode;
		explicit ASTZTestNode(const Token& t) : ASTNode(ASTNodeType::ZTest, t) { }

		std::string Value;
	};

	/*
	example：ZClip True
	document：https://docs.unity3d.com/Manual/SL-ZClip.html
	*/
	struct ASTZClipNode : public ASTNode {
		using ASTNode::ASTNode;
		explicit ASTZClipNode(const Token& t) : ASTNode(ASTNodeType::ZClip, t) { }

		std::string Value;
	};

	/*
	example：ZWrite Off
	document：https://docs.unity3d.com/Manual/SL-ZWrite.html
	*/
	struct ASTZWriteNode : public ASTNode {
		using ASTNode::ASTNode;
		explicit ASTZWriteNode(const Token& t) : ASTNode(ASTNodeType::ZWrite, t) { }

		std::string Value;
	};

	/*
	example：Fog Off, Fog { Color (0,0,0,0) }
	document：https://docs.unity3d.com/Manual/SL-Fog.html
	*/
	struct ASTFogNode : public ASTNode {
		using ASTNode::ASTNode;
		explicit ASTFogNode(const Token& t) : ASTNode(ASTNodeType::Fog, t) { }

		bool IsOff;
		std::array<float, 4> Color;
	};

	/*
	example：
	document：https://docs.unity3d.com/Manual/shader-shaderlab-code-blocks.html
	*/
	struct ASTShaderBlockNode : ASTNode {
		using ASTNode::ASTNode;
		explicit ASTShaderBlockNode(const Token& t) : ASTNode(ASTNodeType::ShaderBlock, t) { }

		std::string ShaderCode;
	};
	struct ASTShaderIncludeNode : ASTNode {
		using ASTNode::ASTNode;
		explicit ASTShaderIncludeNode(const Token& t) : ASTNode(ASTNodeType::ShaderInclude, t) { }

		std::string ShaderCode;
	};

	class ParseError : public std::runtime_error {
	public:
		explicit ParseError(const std::string& msg)
			: std::runtime_error("Parse error: " + msg) {}
	};
}