#pragma once

#include <string>
#include <unordered_map>

namespace shaderlab {
	enum class TokenType {
		// Symbol
		SymbolLBrace,     // {
		SymbolRBrace,     // }
		SymbolLBracket,   // [
		SymbolRBracket,   // ]
		SymbolLParent,    // (
		SymbolRParent,    // )
		SymbolEqual,      // =
		SymbolComma,      // ,
		SymbolSemicolon,  // ;
		SymbolColon,      // :

		// Literals
		Identifier,
		StringLiteral,
		NumberLiteral,

		// Special
		Comment,
		Unknown,
		EndOfFile,

		// Shader
		Shader,
		Category,
		Properties,
		Fallback,
		CustomEditor,
		Dependency,

		// Subshader
		SubShader,
		Pass,
		Tags,
		LOD,
		UsePass,
		GrabPass,

		// Property
		Int,
		Float,
		Range,
		Color,
		Vector,
		Rect,
		Texture2D,
		Texture2DArray,
		Cubemap,
		CubemapArray,
		Texture3D,
		Any,

		// Pass
		Name,
		AlphaToMask,
		ColorMask,
		Cull,
		Offset,
		Conservative,
		ZClip,
		ZTest,
		ZWrite,
		Stencil,
		Blend,
		BlendOp,
		Off,// blend close
		Fog,
		Mode,

		//CG
		CGProgram,
		EndCG,
		HLSLProgram,
		EndHLSL,
		GLSLProgram,
		EndGLSL,
		CGInclude,
		HLSLInclude,
		GLSLInclude,

		//legacy:https://docs.unity3d.com/Manual/SL-Material.html
		//Color,
		Alphatest,//https://docs.unity3d.com/Manual/SL-AlphaTest.html
		Material,
		Diffuse, Ambient, Specular, Shininess, Emission,
		Lighting,
		SeparateSpecular,
		ColorMaterial,
		SetTexture,
		BindChannels,//https://docs.unity3d.com/Manual/SL-BindChannels.html
	};

	struct Token {
		TokenType type;
		std::string value;
		size_t line;
		size_t column;
	};

	inline bool IsLegacyKeyword(const TokenType& type) {
		std::initializer_list<TokenType> types = { TokenType::Alphatest, TokenType::Color, TokenType::Material,
			TokenType::Diffuse,TokenType::Ambient, TokenType::Specular, TokenType::Shininess, TokenType::Emission,
			TokenType::Lighting, TokenType::SeparateSpecular, TokenType::ColorMaterial, TokenType::SetTexture,TokenType::BindChannels };
		for (const auto& t : types)
		{
			if (t == type)
				return true;
		}
		return false;
	}

	struct CaseInsensitiveHash {
		size_t operator()(const std::string& key) const {
			std::string lower_key;
			lower_key.reserve(key.size());
			for (char c : key) {
				lower_key.push_back(std::tolower(c));
			}
			return std::hash<std::string>{}(lower_key);
		}
	};
	struct CaseInsensitiveEqual {
		bool operator()(const std::string& lhs, const std::string& rhs) const {
			return std::equal(
				lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
				[](char a, char b) { return std::tolower(a) == std::tolower(b); }
			);
		}
	};
	const std::unordered_map<std::string, TokenType, CaseInsensitiveHash, CaseInsensitiveEqual> Keyword2Tokens = {
		// Shader
		{"Shader", TokenType::Shader},
		{"Category", TokenType::Category},
		{"Properties", TokenType::Properties},
		{"Fallback", TokenType::Fallback, },
		{"CustomEditor", TokenType::CustomEditor},
		{"Dependency", TokenType::Dependency},

		// SubShader
		{"SubShader", TokenType::SubShader},
		{"Pass", TokenType::Pass},
		{"Tags", TokenType::Tags},
		{"LOD", TokenType::LOD},
		{"UsePass", TokenType::UsePass},
		{"GrabPass", TokenType::GrabPass},

		// Properties
		{"Int", TokenType::Int},
		{"Float", TokenType::Float},
		{"Range", TokenType::Range},
		{"Color", TokenType::Color},
		{"Vector", TokenType::Vector},
		{"Rect", TokenType::Rect},
		{"2D", TokenType::Texture2D},
		{"2DArray", TokenType::Texture2DArray},
		{"Cube", TokenType::Cubemap},
		{"CubeArray", TokenType::CubemapArray},
		{"3D", TokenType::Texture3D},
		{"Any", TokenType::Any},

		// Pass
		{"Name", TokenType::Name},
		{"AlphaToMask", TokenType::AlphaToMask},
		{"ColorMask", TokenType::ColorMask},
		{"Conservative", TokenType::Conservative},
		{"Cull", TokenType::Cull},
		{"Offset", TokenType::Offset},
		{"ZClip", TokenType::ZClip},
		{"ZTest", TokenType::ZTest},
		{"ZWrite", TokenType::ZWrite},
		{"Stencil", TokenType::Stencil},
		{"Blend", TokenType::Blend},
		{"Off", TokenType::Off},
		{"BlendOp", TokenType::BlendOp},
		{"Fog", TokenType::Fog},
		{"Mode", TokenType::Mode},

		{"CGProgram", TokenType::CGProgram},
		{"ENDCG", TokenType::EndCG},
		{"HLSLPROGRAM", TokenType::HLSLProgram},
		{"ENDHLSL", TokenType::EndHLSL},
		{"GLSLProgram", TokenType::GLSLProgram},
		{"ENDGLSL", TokenType::EndGLSL},
		{"CGINCLUDE", TokenType::CGInclude},
		{"HLSLINCLUDE", TokenType::HLSLInclude},
		{"GLSLINCLUDE", TokenType::GLSLInclude},

		//legacy
		{"Material", TokenType::Material},
		{"Lighting", TokenType::Lighting},
		{"SeparateSpecular", TokenType::SeparateSpecular},
		{"ColorMaterial", TokenType::ColorMaterial},
		{"SetTexture", TokenType::SetTexture},
		{"Alphatest", TokenType::Alphatest},
		{"BindChannels", TokenType::BindChannels},
	};

	struct EnumHash {
		template <typename T>
		std::size_t operator()(T t) const {
			return static_cast<std::size_t>(t);
		}
	};
	const std::unordered_map<TokenType, std::string, EnumHash> Token2Keywords = {
		{TokenType::SymbolLBrace, "{"},
		{TokenType::SymbolRBrace, "}"},
		{TokenType::SymbolLParent, "("},
		{TokenType::SymbolRParent, ")"},
		{TokenType::SymbolEqual, "="},
		{TokenType::SymbolComma, ","},
		{TokenType::SymbolSemicolon, ";"},

		{TokenType::StringLiteral, "Identifier"},
		{TokenType::StringLiteral, "StringLiteral"},
		{TokenType::NumberLiteral, "NumberLiteral"},

		{TokenType::EndOfFile, "EndOfFile"},
		{TokenType::Unknown, "Unknown"},

		// Shader
		{TokenType::Shader, "Shader" },
		{TokenType::Category, "Category"},
		{TokenType::Properties, "Properties"},
		{TokenType::Fallback, "Fallback"},
		{TokenType::CustomEditor, "CustomEditor"},
		{TokenType::Dependency, "Dependency"},

		// SubShader
		{TokenType::SubShader, "SubShader"},
		{TokenType::Pass, "Pass"},
		{TokenType::Tags, "Tags"},
		{TokenType::LOD, "LOD"},
		{TokenType::UsePass, "UsePass"},
		{TokenType::GrabPass, "GrabPass"},

		// Properties
		{TokenType::Int, "Int"},
		{TokenType::Float, "Float"},
		{TokenType::Range, "Range"},
		{TokenType::Color, "Color"},
		{TokenType::Vector, "Vector"},
		{TokenType::Rect, "Rect"},
		{TokenType::Texture2D, "2D"},
		{TokenType::Texture2DArray, "2DArray"},
		{TokenType::Cubemap, "Cube"},
		{TokenType::CubemapArray, "CubeArray"},
		{TokenType::Texture3D, "3D"},
		{TokenType::Any, "Any"},

		// Pass
		{TokenType::Name, "Name"},
		{TokenType::AlphaToMask, "AlphaToMask"},
		{TokenType::ColorMask, "ColorMask"},
		{TokenType::Conservative, "Conservative"},
		{TokenType::Cull, "Cull"},
		{TokenType::Offset, "Offset"},
		{TokenType::ZClip, "ZClip"},
		{TokenType::ZTest, "ZTest"},
		{TokenType::ZWrite, "ZWrite"},
		{TokenType::Stencil, "Stencil"},
		{TokenType::Blend, "Blend"},
		{TokenType::Off, "Off"},
		{TokenType::BlendOp, "BlendOp"},
		{TokenType::Fog, "Fog"},
		{TokenType::Mode, "Mode"},

		// Shader Programs
		{TokenType::CGProgram, "CGProgram"},
		{TokenType::EndCG, "ENDCG"},
		{TokenType::HLSLProgram, "HLSLPROGRAM"},
		{TokenType::EndHLSL, "ENDHLSL"},
		{TokenType::GLSLProgram, "GLSLProgram"},
		{TokenType::EndGLSL, "ENDGLSL"},
		{TokenType::CGInclude, "CGINCLUDE"},
		{TokenType::HLSLInclude, "HLSLINCLUDE"},
		{TokenType::GLSLInclude, "GLSLINCLUDE"},

		// Legacy
		{TokenType::Material, "Material"},
		{TokenType::Lighting, "Lighting"},
		{TokenType::SeparateSpecular, "SeparateSpecular"},
		{TokenType::ColorMaterial, "ColorMaterial"},
		{TokenType::SetTexture, "SetTexture"},
		{TokenType::Alphatest, "Alphatest"},
		{TokenType::BindChannels, "BindChannels"},
	};
}