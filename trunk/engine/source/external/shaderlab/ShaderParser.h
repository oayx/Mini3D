#pragma once

#include <cassert>
#include "ShaderLexer.h"
#include "ParserDefined.h"

#define SHADERLAB_VALIDCHECK

namespace shaderlab {
#ifdef SHADERLAB_LOG
	#define ThrowParseError(msg) \
        do { \
            std::string fullMsg = std::string("shader file:") + _fileName + ", line:" + std::to_string(peek().line) \
                + " message: " + std::string(msg) \
                + " (thrown at " + std::string(__FILE__) + ":" + std::to_string(__LINE__) + ")"; \
            std::cerr << fullMsg << std::endl; \
        } while(0)
#else
	#define ThrowParseError(msg) \
        do { \
            std::string fullMsg = std::string("shader file:") + _fileName + ", line:" + std::to_string(peek().line) \
                + " message: " + std::string(msg) \
                + " (thrown at " + std::string(__FILE__) + ":" + std::to_string(__LINE__) + ")"; \
            throw ParseError(fullMsg); \
        } while(0)
#endif

	class ShaderParser {
	private:
		const std::string _fileName;
		std::vector<Token> _tokens;
		size_t _position;

	public:
		explicit ShaderParser(const std::string& fileName, std::vector<Token> tokens)
			: _fileName(fileName), _tokens(tokens), _position(0) {}

		std::shared_ptr<ASTNode> parse() {
			auto root = std::make_shared<ASTShaderNode>(advance());
			root->Name = parseShaderName();
			expect(TokenType::SymbolLBrace);

			while (!match(TokenType::SymbolRBrace)) {
				if (match(TokenType::Properties)) {
					parseProperties(root);
				}
				else if (match(TokenType::Category)) {
					parseCategory(root);
				}
				else if (match(TokenType::SubShader)) {
					parseSubShader(root);
				}
				else if (match(TokenType::Fallback)) {
					parseFallback(root);
				}
				else if (match(TokenType::CustomEditor)) {
					parseCustomEditor(root);
				}
				else if (match(TokenType::CGInclude) || match(TokenType::HLSLInclude) || match(TokenType::GLSLInclude)) {
					parseShaderInclude(root);
				}
				else if (match(TokenType::Dependency)){
					parseDependency(root);
				}
#if defined SHADERLAB_VALIDCHECK
				else {
					ThrowParseError("Unexpected token");
				}
#endif
			}
			expect(TokenType::SymbolRBrace);
			expect(TokenType::EndOfFile);
			return root;
		}

	private:
		Token& peek() {
			return _tokens[_position];
		}
		Token& advance() {
			return _tokens[_position++];
		}

		bool match(TokenType type) { return _position < _tokens.size() && peek().type == type; }
		TokenType match(const std::initializer_list<TokenType>& types) {
			for (auto type : types) {
				if (match(type)) {
					return type;
				}
			}
			ThrowParseError("Unexpected token type");
		}
		bool match(const std::initializer_list<std::string>& types, const std::string& dstType) {
			for (const auto& type : types) {
				if (StringUtils::EqualIgnoreCase(type, dstType)) {
					return true;
				}
			}
			return false;
		}

		Token& expect(TokenType type) {
			if (!match(type)) {
				ThrowParseError("Expected " + tokenTypeToString(type) + " but got " + tokenTypeToString(peek().type));
			}
			return advance();
		}
		TokenType expect(const std::initializer_list<TokenType>& types) {
			for (auto type : types) {
				if (match(type)) {
					advance();
					return type;
				}
			}
			ThrowParseError("Unexpected token type");
		}

		std::string tokenTypeToString(TokenType type) {
			auto it = Token2Keywords.find(type);
			return it != Token2Keywords.end() ? it->second : "Unknown";
		}

	private:
		std::string parseShaderName() {
			if (match(TokenType::StringLiteral)) {
				return advance().value;
			}
			return advance().value; // 处理没有引号的着色器名称
		}

		void parseProperties(std::shared_ptr<ASTNode> parentNode) {
			auto node = std::make_shared<ASTPropertiesNode>(advance());
			expect(TokenType::SymbolLBrace);

			while (!match(TokenType::SymbolRBrace)) {
				parsePropertyDeclaration(node);
			}
			expect(TokenType::SymbolRBrace);
			parentNode->children.push_back(node);
		}
		// Property format: [xxx]_Name("Display Name", type) = default
		void parsePropertyDeclaration(std::shared_ptr<ASTNode> parentNode) {

			std::string header = "";
			if (match(TokenType::SymbolLBracket))
			{//[HideInInspector] 
				expect(TokenType::SymbolLBracket);
				while (!match(TokenType::SymbolRBracket)) {
					header += advance().value;
				}
				expect(TokenType::SymbolRBracket);
			}

			auto node = std::make_shared<ASTPropertyNode>(advance());
			node->Name = node->Value;
			node->Header = header;
			expect(TokenType::SymbolLParent);
			node->DisplayName = expect(TokenType::StringLiteral).value;
			expect(TokenType::SymbolComma);
			parsePropertyType(node);
			expect(TokenType::SymbolRParent);

			//default value
			if (match(TokenType::SymbolEqual)) {
				advance();
				node->DefaultValue = parsePropertyDefault();
			}

			parentNode->children.push_back(node);
		}
		void parsePropertyType(std::shared_ptr<ASTPropertyNode> parentNode) {
			if (match(TokenType::Range)) {
				advance();// consume 'Range'
				parentNode->PropType = PropertyType::Range;
				expect(TokenType::SymbolLParent);
				parentNode->Range.min = parseNumber(); // min
				expect(TokenType::SymbolComma);
				parentNode->Range.max = parseNumber(); // max
				expect(TokenType::SymbolRParent);
			}
			else {
				TokenType type = expect({ TokenType::Int, TokenType::Float, TokenType::Color, TokenType::Vector,TokenType::Rect,
					TokenType::Texture2D, TokenType::Texture2DArray, TokenType::Cubemap, TokenType::CubemapArray, TokenType::Texture3D, TokenType::Any });
				switch (type)
				{
				case TokenType::Int:parentNode->PropType = PropertyType::Int; break;
				case TokenType::Float:parentNode->PropType = PropertyType::Float; break;
				case TokenType::Color:parentNode->PropType = PropertyType::Color; break;
				case TokenType::Vector:parentNode->PropType = PropertyType::Vector; break;
				case TokenType::Rect:parentNode->PropType = PropertyType::Rect; break;
				case TokenType::Texture2D:parentNode->PropType = PropertyType::Texture2D; break;
				case TokenType::Texture2DArray:parentNode->PropType = PropertyType::Texture2DArray; break;
				case TokenType::Cubemap:parentNode->PropType = PropertyType::TextureCube; break;
				case TokenType::CubemapArray:parentNode->PropType = PropertyType::TextureCubeArray; break;
				case TokenType::Texture3D:parentNode->PropType = PropertyType::Texture3D; break;
				case TokenType::Any:parentNode->PropType = PropertyType::Any; break;
				}
			}
		}
		std::string parsePropertyDefault() {
			std::string defaultValue;
			if (match(TokenType::SymbolLParent)) {
				// 解析颜色或向量格式 (x, y, z, w)
				advance(); // 消耗 '('
				defaultValue += "(";
				bool first = true;

				while (!match(TokenType::SymbolRParent)) {
					if (!first) {
						if (match(TokenType::SymbolComma)) {
							advance(); // 消耗 ','
							defaultValue += ",";
						}
#if defined SHADERLAB_VALIDCHECK
						else {
							ThrowParseError("Expected comma in vector default value");
						}
#endif
					}

					if (match(TokenType::NumberLiteral)) {
						defaultValue += advance().value;
						first = false;
					}
#if defined SHADERLAB_VALIDCHECK
					else {
						ThrowParseError("Expected number in vector default value");
					}
#endif
				}

				advance(); // 消耗 ')'
				defaultValue += ")";
			}
			else if (match(TokenType::StringLiteral)) {
				// 解析纹理默认值格式 "white" 或 "name" {}
				defaultValue = advance().value;

				// 处理可能的纹理初始化参数
				if (match(TokenType::SymbolLBrace)) {
					advance(); // 消耗 '{'
					defaultValue += "{";

					// 跳过所有内部设置直到遇到 }
					while (!match(TokenType::SymbolRBrace) && !match(TokenType::EndOfFile)) {
						defaultValue += advance().value;
					}

					if (match(TokenType::SymbolRBrace)) {
						advance(); // 消耗 '}'
						defaultValue += "}";
					}
				}
			}
			else if (match(TokenType::NumberLiteral)) {
				// 解析简单数值格式 (整数或浮点数)
				defaultValue = advance().value;
			}
#if defined SHADERLAB_VALIDCHECK
			else {
				ThrowParseError("Unexpected default value format");
			}
#endif
			return defaultValue;
		}

		void parseSemantic(std::shared_ptr<ASTNode> node) {
			while (!match(TokenType::SymbolRBrace)) {
				if (match(TokenType::SubShader)) {
					parseSubShader(node);
				}
				else if (match(TokenType::Pass)) {
					parsePass(node);
				}
				else if (match(TokenType::Name)) {
					if(node->Type == ASTNodeType::Pass)
						parsePassName(std::dynamic_pointer_cast<ASTPassNode>(node));
#if defined SHADERLAB_VALIDCHECK
					else
						ThrowParseError("Unknow name");
#endif
				}
				else if (match(TokenType::Tags)) {
					parseTags(node);
				}
				else if (match(TokenType::LOD)) {
					parseLOD(node);
				}
				else if (match(TokenType::UsePass)) {
					parseUsePass(node);
				}
				else if (match(TokenType::GrabPass)) {
					parseGrabPass(node);
				}
				else if (match(TokenType::Blend)) {
					parseBlend(node);
				}
				else if (match(TokenType::BlendOp)) {
					parseBlendOp(node);
				}
				else if (match(TokenType::AlphaToMask)) {
					parseAlphaToMask(node);
				}
				else if (match(TokenType::ColorMask)) {
					parseColorMask(node);
				}
				else if (match(TokenType::Conservative)) {
					parseConservative(node);
				}
				else if (match(TokenType::Cull)) {
					parseCull(node);
				}
				else if (match(TokenType::Offset)) {
					parseOffset(node);
				}
				else if (match(TokenType::ZClip)) {
					parseZClip(node);
				}
				else if (match(TokenType::ZTest)) {
					parseZTest(node);
				}
				else if (match(TokenType::ZWrite)) {
					parseZWrite(node);
				}
				else if (match(TokenType::Stencil)) {
					parseStencil(node);
				}
				else if (match(TokenType::Fog)) {
					parseFog(node);
				}
				else if (match(TokenType::CGProgram) || match(TokenType::HLSLProgram) || match(TokenType::GLSLProgram)) {
					parseShaderBlock(node);
				}
				else if (match(TokenType::CGInclude) || match(TokenType::HLSLInclude) || match(TokenType::GLSLInclude)) {
					parseShaderInclude(node);
				}
				else if (IsLegacyKeyword(peek().type)) {
					parseLegacyKeyword(node);
				}
				else {
#if defined SHADERLAB_VALIDCHECK
					ThrowParseError("Unexpected token in subshader");
#endif
				}
			}
		}

		void parseCategory(std::shared_ptr<ASTNode> parentNode) {
			auto node = std::make_shared<ASTCategoryNode>(advance());
			expect(TokenType::SymbolLBrace);
			parseSemantic(node);
			expect(TokenType::SymbolRBrace);
			parentNode->children.push_back(node);
		}

		void parseSubShader(std::shared_ptr<ASTNode> parentNode) {
			auto node = std::make_shared<ASTSubShaderNode>(advance());
			expect(TokenType::SymbolLBrace);
			parseSemantic(node);
			expect(TokenType::SymbolRBrace);
			parentNode->children.push_back(node);
		}

		void parseTags(std::shared_ptr<ASTNode> parentNode) {
			auto node = std::make_shared<ASTTagsNode>(advance()); // Tags
			expect(TokenType::SymbolLBrace);

			while (!match(TokenType::SymbolRBrace)) {
				std::string key = expect(TokenType::StringLiteral).value;
				expect(TokenType::SymbolEqual);
				std::string value = expect(TokenType::StringLiteral).value;
#if defined SHADERLAB_VALIDCHECK
				if (match({ "RenderPipeline" ,"Queue" ,"RenderType",
					"DisableBatching", "ForceNoShadowCasting" , "CanUseSpriteAtlas", "IgnoreProjector" , "PreviewType",
					"LightMode" ,"PassFlags" ,"RequireOptions", "RequireOption", "PerformanceChecks",
					"ShadowmapFilter", "ForceSupported" }, key))
				{
					if (key == "RenderPipeline")
					{//TODO:支持自定义
						if (!match({ "UniversalPipeline" ,"HDRenderPipeline" ,"" }, value))//不设置（或空）	Built-in Render Pipeline	默认值，用于旧版内置渲染管线。
							ThrowParseError("Invalid parseTags");
					}
					else if (key == "Queue")
					{//TODO:支持+1等offset操作
						if (value.find('+') == std::string::npos && value.find('-') == std::string::npos)
						{
							if (!value.empty() && std::isdigit(value[0]))
							{//number
								if(!StringUtils::IsNumber(value))
									ThrowParseError("Invalid parseTags");
							}
							else if (!match({ "Background", "Geometry", "AlphaTest" , "Transparent" , "Overlay" }, value))
								ThrowParseError("Invalid parseTags");
						}
					}
					else if (key == "RenderType")
					{//TODO:支持自定义,https://docs.unity3d.com/2022.3/Documentation/Manual/SL-ShaderReplacement.html
						if (!match({ "Opaque","Transparent","TransparentCutout","Background","Overlay",
							"TreeOpaque","TreeTransparentCutout","TreeBillboard","TreeBark","TreeLeaf",
							"Grass" ,"GrassBillboard" ,"Skybox" }, value))
							ThrowParseError("Invalid parseTags");
					}
					else if (key == "ForceNoShadowCasting")
					{
						if (!match({ "True", "False" }, value))
							ThrowParseError("Invalid parseTags");
					}
					else if (key == "DisableBatching")
					{
						if (!match({ "True", "False", "LODFading" }, value))
							ThrowParseError("Invalid parseTags");
					}
					else if (key == "IgnoreProjector")
					{
						if (!match({ "True", "False" }, value))
							ThrowParseError("Invalid parseTags");
					}
					else if (key == "PreviewType")
					{
						if (!match({ "Sphere", "Plane", "Skybox" }, value))
							ThrowParseError("Invalid parseTags");
					}
					else if (key == "LightMode")
					{
						if (!match({ "Always" ,"ForwardBase" ,"ForwardAdd" ,"Deferred" ,"ShadowCaster"
							 ,"MotionVectors"  ,"Vertex"  ,"VertexLMRGBM"  ,"VertexLM"  ,"Meta",
							"SceneSelectionPass", "Picking", "Depth", "DepthNormal" }, value))
							ThrowParseError("Invalid parsePassTags");
					}
					else if (key == "PassFlags")
					{
						if (!match({ "OnlyDirectional" }, value))
							ThrowParseError("Invalid parsePassTags");
					}
					else if (key == "RequireOptions" || key == "RequireOption")
					{
						if (!match({ "SoftVegetation" }, value))
							ThrowParseError("Invalid parsePassTags");
					}
					else if (key == "PerformanceChecks")
					{
						if (!match({ "True", "False" }, value))
							ThrowParseError("Invalid parseTags");
					}
					else if (key == "ForceSupported")
					{
						if (!match({ "True", "False" }, value))
							ThrowParseError("Invalid parseTags");
					}
				}
				else
				{
					//undo
				}
#endif
				node->tags[key] = value;
			}

			expect(TokenType::SymbolRBrace);
			parentNode->children.push_back(node);
		}

		void parseLOD(std::shared_ptr<ASTNode> parentNode)
		{
			auto node = std::make_shared<ASTLODNode>(advance());
			match(TokenType::NumberLiteral);
			node->LOD = std::atoi(advance().value.c_str());
			parentNode->children.push_back(node);
		}

		void parseUsePass(std::shared_ptr<ASTNode> parentNode)
		{
			auto node = std::make_shared<ASTUsePassNode>(advance());
			match(TokenType::StringLiteral);
			node->Name = advance().value;
			parentNode->children.push_back(node);
		}

		void parseGrabPass(std::shared_ptr<ASTNode> parentNode)
		{
			auto node = std::make_shared<ASTGrabPassNode>(advance());
			expect(TokenType::SymbolLBrace);
			while (!match(TokenType::SymbolRBrace))
			{
				if (match(TokenType::Tags)) {
					parseTags(node);
				}
				else if (match(TokenType::StringLiteral))
					node->TextureName = advance().value;
				else if (match(TokenType::Name))
					node->TextureName = advance().value;
			}
			expect(TokenType::SymbolRBrace);
			parentNode->children.push_back(node);
		}

		void parsePass(std::shared_ptr<ASTNode> parentNode) {
			auto node = std::make_shared<ASTPassNode>(advance());
			expect(TokenType::SymbolLBrace);
			parseSemantic(node);
			expect(TokenType::SymbolRBrace);
			parentNode->children.push_back(node);
		}

		void parsePassName(std::shared_ptr<ASTPassNode> parentNode) {
			Token nameToken = advance(); // Name
			Token valueToken = expect(TokenType::StringLiteral);
			parentNode->Name = valueToken.value;
		}

		void parseAlphaToMask(std::shared_ptr<ASTNode> parentNode) {
			auto node = std::make_shared<ASTAlphaToMaskNode>(advance());

			if (match(TokenType::SymbolLBracket))
			{//[]
				expect(TokenType::SymbolLBracket);
				node->Value = advance().value;
				expect(TokenType::SymbolRBracket);
			}
			else
			{
				node->Value = advance().value; 
#if defined SHADERLAB_VALIDCHECK
				if (!match({ "On" ,"Off","True" ,"False" }, node->Value))
					ThrowParseError("Invalid parseAlphaToMask");
#endif
			}

			parentNode->children.push_back(node);
		}

		void parseColorMask(std::shared_ptr<ASTNode> parentNode) {
			auto node = std::make_shared<ASTColorMaskNode>(advance());

			bool hasBracket = false;
			if (match(TokenType::SymbolLBracket))
			{//[
				hasBracket = true;
				expect(TokenType::SymbolLBracket);
			}

			node->Value = advance().value;

			if (hasBracket)//]
				expect(TokenType::SymbolRBracket);

			if (match(TokenType::NumberLiteral))
			{
				hasBracket = false;
				if (match(TokenType::SymbolLBracket))
				{//[
					hasBracket = true;
					expect(TokenType::SymbolLBracket);
				}

				node->RenderTarget = std::atoi(advance().value.c_str());

				if (hasBracket)//]
					expect(TokenType::SymbolRBracket);

			}
			parentNode->children.push_back(node);
		}

		void parseConservative(std::shared_ptr<ASTNode> parentNode) {
			auto node = std::make_shared<ASTConservativeNode>(advance());

			if (match(TokenType::SymbolLBracket))
			{//[]
				expect(TokenType::SymbolLBracket);
				node->Value = advance().value;
				expect(TokenType::SymbolRBracket);
			}
			else
			{
				node->Value = advance().value;
#if defined SHADERLAB_VALIDCHECK
				if (!match({ "True" ,"False" }, node->Value))
					ThrowParseError("Invalid parseConservative");
#endif
			}

			parentNode->children.push_back(node);
		}

		void parseCull(std::shared_ptr<ASTNode> parentNode) {
			auto node = std::make_shared<ASTCullNode>(advance());

			if (match(TokenType::SymbolLBracket))
			{//[]
				expect(TokenType::SymbolLBracket);
				node->Value = advance().value;
				expect(TokenType::SymbolRBracket);
			}
			else
			{
				node->Value = advance().value;
#if defined SHADERLAB_VALIDCHECK
				if (!match({ "Back" ,"Front", "Off" }, node->Value))
					ThrowParseError("Invalid parseCull");
#endif
			}

			parentNode->children.push_back(node);
		}

		void parseOffset(std::shared_ptr<ASTNode> parentNode) {
			auto node = std::make_shared<ASTOffsetNode>(advance());

			if (match(TokenType::SymbolLBracket))
			{//[]
				expect(TokenType::SymbolLBracket);
				node->Factor = advance().value;
				expect(TokenType::SymbolRBracket);
			}
			else
			{
				node->Factor = advance().value.c_str();
			}

			if (match(TokenType::SymbolComma))
			{
				expect(TokenType::SymbolComma);

				if (match(TokenType::SymbolLBracket))
				{//[]
					expect(TokenType::SymbolLBracket);
					node->Units = advance().value;
					expect(TokenType::SymbolRBracket);
				}
				else
				{
					node->Units = advance().value.c_str();
				}
			}

			parentNode->children.push_back(node);
		}

		void parseZClip(std::shared_ptr<ASTNode> parentNode) {
			auto node = std::make_shared<ASTZClipNode>(advance());

			if (match(TokenType::SymbolLBracket))
			{//[]
				expect(TokenType::SymbolLBracket);
				node->Value = advance().value;
				expect(TokenType::SymbolRBracket);
			}
			else
			{
				node->Value = advance().value;
#if defined SHADERLAB_VALIDCHECK
				if (!match({ "True" ,"False" }, node->Value))
					ThrowParseError("Invalid parseZClip");
#endif
			}

			parentNode->children.push_back(node);
		}

		void parseZTest(std::shared_ptr<ASTNode> parentNode) {
			auto node = std::make_shared<ASTZTestNode>(advance());

			if (match(TokenType::SymbolLBracket))
			{//[]
				expect(TokenType::SymbolLBracket);
				node->Value = advance().value;
				expect(TokenType::SymbolRBracket);
			}
			else
			{
				node->Value = advance().value;
#if defined SHADERLAB_VALIDCHECK
				if (!match({ "Off" , "Disabled" ,"Never", "Less", "Equal" , "LEqual", "Greater" , "NotEqual" , "GEqual" , "Always" }, node->Value))
					ThrowParseError("Invalid parseZTest");
#endif
			}

			parentNode->children.push_back(node);
		}

		void parseStencil(std::shared_ptr<ASTNode> parentNode) {
			auto node = std::make_shared<ASTStencilNode>(advance());

			expect(TokenType::SymbolLBrace);
			while (!match(TokenType::SymbolRBrace)) {
				const std::string key = advance().value;
				if (match(TokenType::SymbolLBracket))
				{//{
					expect(TokenType::SymbolLBracket);

					const std::string value = advance().value;
					node->Values.insert(std::pair<std::string, std::string>(key, value));

					expect(TokenType::SymbolRBracket);
				}
				else
				{
#if defined SHADERLAB_VALIDCHECK
					if (!match({ "Ref" ,"ReadMask", "WriteMask", "Comp" , "Pass", "Fail" , "ZFail" , "CompBack" , "PassBack"
						, "FailBack", "ZFailBack" , "CompFront" , "PassFront" , "FailFront" , "ZFailFront" }, key))
					{
						ThrowParseError("Invalid parseStencil");
					}
#endif
					const std::string value = advance().value;
					node->Values.insert(std::pair<std::string, std::string>(key, value));
				}
			}
			expect(TokenType::SymbolRBrace);

			parentNode->children.push_back(node);
		}

		void parseZWrite(std::shared_ptr<ASTNode> parentNode) {
			auto node = std::make_shared<ASTZWriteNode>(advance());

			if (match(TokenType::SymbolLBracket))
			{//[
				expect(TokenType::SymbolLBracket);
				node->Value = advance().value;//Custom name, no detection
				expect(TokenType::SymbolRBracket);//]
			}
			else
			{
				node->Value = advance().value;
#if defined SHADERLAB_VALIDCHECK
				if (!match({ "On" ,"Off" }, node->Value))
					ThrowParseError("Invalid parseZWrite");
#endif
			}

			parentNode->children.push_back(node);
		}

		void parseBlend(std::shared_ptr<ASTNode> parentNode) {
			auto node = std::make_shared<ASTBlendNode>(advance()); // consumeBlend

			if (match(TokenType::NumberLiteral)) {
				node->renderTarget = std::atoi(advance().value.c_str());
			}

			if (match(TokenType::Off)) {
				node->blendOff = true;
				advance(); // consumeOff
			}
			else {
				// color blend
				node->factors.src = parseBlendFactor();
				node->factors.dst = parseBlendFactor();

				// alpha blend
				if (match(TokenType::SymbolComma)) {
					advance(); // consume ','
					node->factors.separateAlpha = true;
					node->factors.srcAlpha = parseBlendFactor();
					node->factors.dstAlpha = parseBlendFactor();
				}
			}

			parentNode->children.push_back(node);
		}
		void parseBlendOp(std::shared_ptr<ASTNode> parentNode) {
			auto node = std::make_shared<ASTBlendOpNode>(advance()); // consumeBlendOp
			if (match(TokenType::SymbolLBracket))
			{//[
				expect(TokenType::SymbolLBracket);
				node->BlendOp = advance().value;//Custom name, no detection
				expect(TokenType::SymbolRBracket);//]
			}
			else
			{
#if defined SHADERLAB_VALIDCHECK
				if (!match(TokenType::StringLiteral) && !match(TokenType::Identifier)) {
					ThrowParseError("Invalid parseBlendOp");
				}
#endif
				const std::string op = advance().value;
				//TODO:这里还有很多操作，就不做严格验证了
				//if (!match({ "Add" ,"Sub" ,"RevSub"  ,"Min"  ,"Max" }, op))
				//	ThrowParseError("Invalid parseBlendOp");
				node->BlendOp = op;
			}
			parentNode->children.push_back(node);
		}
		std::string parseBlendFactor() {
			if (match(TokenType::SymbolLBracket))
			{//[
				expect(TokenType::SymbolLBracket);
				const std::string factor = advance().value;//Custom name, no detection
				expect(TokenType::SymbolRBracket);//]
				return factor;
			}

			if (match(TokenType::StringLiteral) || match(TokenType::Identifier)) {
				const std::string factor = advance().value;
				if (match({ "One" ,"Zero" ,"SrcColor"  ,"SrcAlpha"  ,"SrcAlphaSaturate"  ,"DstColor"  ,"DstAlpha"
					,"OneMinusSrcColor"  ,"OneMinusSrcAlpha"  ,"OneMinusDstColor"  ,"OneMinusDstAlpha" }, factor))
				{
					return factor;
				}
			}

#if defined SHADERLAB_VALIDCHECK
			ThrowParseError("Invalid parseBlendFactor");
#endif
		}

		void parseFog(std::shared_ptr<ASTNode> parentNode) {
			auto node = std::make_shared<ASTFogNode>(advance());

			if (match(TokenType::SymbolLBrace))
			{//{
				node->IsOff = false;
				expect(TokenType::SymbolLBrace);
				while (!match(TokenType::SymbolRBrace)) {
					if (match(TokenType::Color))
					{
						expect(TokenType::Color);
						node->Color = parseVector4();
					}
					else if (match(TokenType::Mode))
					{//Fog { Mode Off }
						expect(TokenType::Mode);		
						const std::string mode = advance().value;
#if defined SHADERLAB_VALIDCHECK
						if (!match({ "Off" ,"On" }, mode))
							ThrowParseError("Invalid parseFog-Mode");
#endif
						node->IsOff = StringUtils::ToLower(mode) == "off";
					}
					else
						advance();
				}
				expect(TokenType::SymbolRBrace);
			}
			else
			{
				std::string value = advance().value;
#if defined SHADERLAB_VALIDCHECK
				if (!match({ "Off" }, value))
					ThrowParseError("Invalid parseZTest");
#endif
				node->IsOff = true;
			}

			parentNode->children.push_back(node);
		}

		void parseShaderBlock(std::shared_ptr<ASTNode> parentNode) {
			auto node = std::make_shared<ASTShaderBlockNode>(advance());
			node->ShaderCode = node->Value;
			parentNode->children.push_back(node);
		}
		void parseShaderInclude(std::shared_ptr<ASTNode> parentNode) {
			auto node = std::make_shared<ASTShaderIncludeNode>(advance());
			node->ShaderCode = node->Value;
			parentNode->children.push_back(node);
		}

		void parseFallback(std::shared_ptr<ASTNode> parentNode) {
			auto node = std::make_shared<ASTFallbackNode>(advance());
			if (match(TokenType::Off))
				advance();
			else
				node->Value = advance().value;
			parentNode->children.push_back(node);
		}

		void parseCustomEditor(std::shared_ptr<ASTNode> parentNode) {
			auto node = std::make_shared<ASTCustomEditorNode>(advance());
			node->Value = advance().value;
			parentNode->children.push_back(node);
		}

		void parseDependency(std::shared_ptr<ASTNode> parentNode) {
			auto node = std::make_shared<ASTDependencyNode>(advance());
			node->Key = advance().value;
			expect(TokenType::SymbolEqual);
			node->Value = advance().value;
			parentNode->children.push_back(node);
		}

		void parseLegacyKeyword(std::shared_ptr<ASTNode> parentNode) {
			auto node = advance();
			switch (node.type)
			{
			case TokenType::Alphatest:
			{//Alphatest Greater [_Cutoff],AlphaTest Off
				const std::string op = advance().value;
				if (match({ "Off" }, op))
				{
				}
				else if (match({ "Greater","GEqual","Less" ,"LEqual" ,"Equal" ,"NotEqual" ,"Always" ,"Never" }, op))
				{
					if (match(TokenType::SymbolLBracket))
					{//[_Cutoff]
						expect(TokenType::SymbolLBracket);
						advance();
						expect(TokenType::SymbolRBracket);
					}
				}
				else
				{
#if defined SHADERLAB_VALIDCHECK
					ThrowParseError("Invalid parseLegacyKeyword-Alphatest");
#endif
				}
				break;
			}
			case TokenType::Color:
			{
				if (match(TokenType::SymbolLParent)) {//Color (1,0,0,0)
					expect(TokenType::SymbolLParent);
					while (!match(TokenType::SymbolRParent)) {
						advance();
					}
					expect(TokenType::SymbolRParent);
				}
				else if (match(TokenType::SymbolLBracket)) {//Color [_Color]
					expect(TokenType::SymbolLBracket);
					advance();
					expect(TokenType::SymbolRBracket);
				}
#if defined SHADERLAB_VALIDCHECK
				else
					ThrowParseError("Invalid parseLegacyKeyword-Color");
#endif
				break;
			}
			case TokenType::Material:
			{
				/*
				Material {
					Diffuse (1,1,1,1)
					Ambient (1,1,1,1)
				}
				*/
				expect(TokenType::SymbolLBrace);
				while (!match(TokenType::SymbolRBrace)) {
					advance();
				}
				expect(TokenType::SymbolRBrace);
				break;
			}
			case TokenType::Lighting:
			{//Lighting On
				const std::string op = advance().value;
#if defined SHADERLAB_VALIDCHECK
				if (!match({ "On" ,"Off" }, op))
					ThrowParseError("Invalid parseLegacyKeyword-Lighting");
#endif
				break;
			}
			case TokenType::SeparateSpecular:
			{//SeparateSpecular On
				const std::string op = advance().value;
#if defined SHADERLAB_VALIDCHECK
				if (!match({ "On" ,"Off" }, op))
					ThrowParseError("Invalid parseLegacyKeyword-SeparateSpecular");
#endif
				break;
			}
			case TokenType::ColorMaterial:
			{//ColorMaterial AmbientAndDiffuse | Emission
				const std::string op = advance().value;
#if defined SHADERLAB_VALIDCHECK
				if (!match({ "AmbientAndDiffuse" ,"Emission" }, op))
					ThrowParseError("Invalid parseLegacyKeyword-ColorMaterial");
#endif
				break;
			}
			case TokenType::SetTexture:
			{
				/*
				SetTexture[_MainTex]{
					Combine texture * primary DOUBLE, texture * primary
				}
				*/
				expect(TokenType::SymbolLBracket);
				advance();
				expect(TokenType::SymbolRBracket);

				expect(TokenType::SymbolLBrace);
				while (!match(TokenType::SymbolRBrace)) {
					advance();
				}
				expect(TokenType::SymbolRBrace);
				break;
			}
			case TokenType::BindChannels:
			{
				/*
				BindChannels {
					Bind "Vertex", vertex
					Bind "normal", normal
					Bind "texcoord1", texcoord0 // lightmap uses 2nd uv
					Bind "texcoord1", texcoord1 // lightmap uses 2nd uv
					Bind "texcoord", texcoord2 // main uses 1st uv
				}
				*/
				expect(TokenType::SymbolLBrace);
				while (!match(TokenType::SymbolRBrace)) {
					advance();
				}
				expect(TokenType::SymbolRBrace);
				break;
			}
			default:
#if defined SHADERLAB_VALIDCHECK
				ThrowParseError("Invalid parseLegacyKeyword");
#endif
				break;
			}
		}
	private:// 辅助解析方法
		float parseNumber() {
#if defined SHADERLAB_VALIDCHECK
			if (!match(TokenType::NumberLiteral)) {
				ThrowParseError("Invalid parseNumber");
			}
#endif
			return std::stof(advance().value.c_str());
		}

		// color or vector4 (x, y, z, w)
		std::array<float, 4> parseVector4() {
			std::array<float, 4> vec4 = { 0,0,0,0 };
			int index = 0;
			advance(); // consume '('
			while (!match(TokenType::SymbolRParent)) {
				if (index != 0) {
					if (match(TokenType::SymbolComma)) {
						advance(); // consume ','
					}
#if defined SHADERLAB_VALIDCHECK
					else {
						ThrowParseError("Invalid parseVector4");
					}
#endif
				}

				if (match(TokenType::NumberLiteral)) {
					vec4[index++] = std::stof(advance().value.c_str());
				}
#if defined SHADERLAB_VALIDCHECK
				else {
					ThrowParseError("Invalid parseVector4");
				}
#endif
			}

			advance(); // consume ')'
			return vec4;
		}
	};

#ifdef ThrowParseError
#undef ThrowParseError
#endif // undef
}