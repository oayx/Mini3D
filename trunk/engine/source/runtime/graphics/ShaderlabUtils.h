 
/*****************************************************************************
* Author： hannibal
* Date：2025/5/5
* Description：cg代码块解析
*****************************************************************************/
#pragma once

#include <regex>
#include "core/Object.h"
#include "external/shaderlab/ShaderParser.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
struct CGProgramData 
{
	std::vector<std::string> Includes;
	std::vector<std::string> Defines;
	std::map<std::string, std::string> Pragmas;
};

class ShaderlabUtils
{
public:
	static std::shared_ptr<shaderlab::ASTNode> ParseShader(const String& filePath)
	{
		//读取文件
		String shaderCode = File::ReadAllText(filePath);
		if (shaderCode.IsEmpty())
		{
			Debuger::Error("Shader::Serialize -  open file error (%s)", filePath.c_str());
			return nullptr;
		}

		//词法分析
		shaderlab::ShaderLexer lexer(filePath.c_str(), shaderCode);
		auto tokens = lexer.tokenize();

		//语法分析
		shaderlab::ShaderParser parser(filePath.c_str(), tokens);
		std::shared_ptr<shaderlab::ASTNode> root = parser.parse();
		if (root->Type != shaderlab::ASTNodeType::Shader)
		{
			Debuger::Error("ShaderlabUtils::ParseShader -  parse shaderlab error (%s)", filePath.c_str());
			return nullptr;
		}
		return root;
	}

	static CGProgramData ParseCG(const std::string& cg_content)
	{
		CGProgramData result;

		// 解析 #include 内容
		std::regex include_regex(R"(^\s*#include\s+["<](.*?)[">])", std::regex::icase);
		std::sregex_iterator inc_it(cg_content.begin(), cg_content.end(), include_regex);
		for (; inc_it != std::sregex_iterator(); ++inc_it)
		{
			result.Includes.push_back((*inc_it)[1]);
		}

		// 解析 #define 内容（支持多宏逗号分隔）
		std::regex define_regex(R"(^\s*#define\s+([^\n]+))", std::regex::icase);
		std::smatch define_match;
		if (regex_search(cg_content, define_match, define_regex))
		{
			std::string define_str = define_match[1];
			// 分割逗号分隔的宏
			std::regex comma_re(R"(\s*,\s*)");
			std::sregex_token_iterator macro_it(define_str.begin(), define_str.end(), comma_re, -1);
			while (macro_it != std::sregex_token_iterator())
			{
				std::string macro = *macro_it++;
				if (!macro.empty())
				{// 去除可能的注释
					size_t comment_pos = macro.find("//");
					if (comment_pos != std::string::npos)
					{
						macro = macro.substr(0, comment_pos);
					}
					result.Defines.push_back(macro);
				}
			}
		}

		// 解析 #pragma 指令
		std::regex pragma_regex(R"((?:^|\n)\s*#pragma\s+(\w+)\s+(\w+))", std::regex::icase | std::regex::ECMAScript);
		std::sregex_iterator pragma_it(cg_content.begin(), cg_content.end(), pragma_regex);
		for (; pragma_it != std::sregex_iterator(); ++pragma_it)
		{
			std::smatch match = *pragma_it;
			result.Pragmas[match[1]] = match[2];
		}

		return result;
	}
};
DC_END_NAMESPACE
