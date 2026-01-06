#pragma once

#include <algorithm>
#include <array>
#include <cctype>
#include <regex>
#include <string>
#include <sstream>

namespace shaderlab {
	class StringUtils
	{
	public:
		inline static bool EqualIgnoreCase(const std::string& s1, const std::string& s2)
		{
			std::string s11 = ToLower(s1);
			std::string s22 = ToLower(s2);
			return s11 == s22;
		}

		// 转换为小写
		inline static std::string ToLower(std::string s)
		{
			std::transform(s.begin(), s.end(), s.begin(),
				[](unsigned char c) { return std::tolower(c); });
			return s;
		}

		// 转换为大写
		inline static std::string ToUpper(std::string s)
		{
			std::transform(s.begin(), s.end(), s.begin(),
				[](unsigned char c) { return std::toupper(c); });
			return s;
		}

		// 去除字符串两端空白
		inline static std::string Trim(const std::string& s) {
			auto start = s.begin();
			while (start != s.end() && std::isspace(*start)) {
				++start;
			}

			auto end = s.end();
			while (end != start && std::isspace(*(end - 1))) {
				--end;
			}

			return std::string(start, end);
		}

		// 去除字符串包裹的引号
		inline static std::string UnwrapString(const std::string& s) {
			if (s.length() >= 2 &&
				((s.front() == '"' && s.back() == '"') ||
					(s.front() == '\'' && s.back() == '\''))) {
				return s.substr(1, s.length() - 2);
			}
			return s;
		}

		// 解析范围参数（例如 "(0.0, 1.0)"）
		inline static std::pair<float, float> ParseRangeParams(const std::string& s) {
			// 去除括号并分割参数
			size_t start = s.find('(');
			size_t end = s.find_last_of(')');
			const std::string content = StringUtils::Trim((start != std::string::npos) ? s.substr(start + 1, end - start - 1) : s);

			// 分割逗号分隔的值
			std::istringstream iss(content);
			std::string first, second;
			std::getline(iss, first, ',');
			std::getline(iss, second);

			// 转换并返回浮点数对
			return { std::stof(StringUtils::Trim(first)),std::stof(StringUtils::Trim(second)) };
		}

		// 解析矢量值（例如 "(1, 0.5, 0, 1)"）
		inline static std::array<float, 4> ParseVector(const std::string& s) {
			std::array<float, 4> result{};
			std::string content = StringUtils::Trim(s.substr(1, s.length() - 2)); // 去掉括号

			std::istringstream iss(content);
			std::string token;
			for (int i = 0; std::getline(iss, token, ',') && i < 4; ++i) {
				result[i] = std::stof(StringUtils::Trim(token));
			}
			return result;
		}

		// 正则表达式匹配所有数字格式：整数、浮点数、科学计数法
		inline static bool IsNumber(const std::string& s) {
			std::regex pattern("^[+-]?\\d+$");
			return !s.empty() && std::regex_match(s, pattern);
		}

	private:// 解析tags的Queue
		// 预设队列值映射表（不区分大小写）
		inline static const std::unordered_map<std::string, int> PRESET_VALUES = {
			{"background", 1000},
			{"geometry", 2000},
			{"alphatest", 2450},
			{"transparent", 3000},
			{"overlay", 4000}
		};
		// 替换字符串中所有子串
		inline static std::string replaceAllIC(const std::string& input, const std::string& from, const std::string& to) {
			std::string result = input;
			size_t pos = 0;
			while ((pos = result.find(from, pos)) != std::string::npos) {
				result.replace(pos, from.length(), to);
				pos += to.length();
			}
			return result;
		}
		// 移除所有空格
		inline static std::string removeSpaces(std::string str) {
			str.erase(remove_if(str.begin(), str.end(), ::isspace), str.end());
			return str;
		}
		// 计算表达式
		inline static int evaluateExpression(const std::string& expr) {
			std::regex numRegex(R"(([+-]?\d+))");
			std::sregex_iterator it(expr.begin(), expr.end(), numRegex);
			std::sregex_iterator end;

			int total = 0;
			for (; it != end; ++it) {
				std::smatch match = *it;
				total += stoi(match.str());
			}
			return total;
		}
	public:
		// 解析tags的Queue
		inline static int ParseShaderlabQueue(const std::string& input) {
			// 预处理
			std::string processed = ToLower(removeSpaces(input));

			// 替换预设关键字（不区分大小写）
			for (const auto& [key, value] : PRESET_VALUES) {
				size_t pos = 0;
				while ((pos = processed.find(key, pos)) != std::string::npos) {
					// 检查是否为完整单词
					if ((pos == 0 || !isalnum(processed[pos - 1])) &&
						(pos + key.length() >= processed.length() || !isalnum(processed[pos + key.length()]))) {
						processed.replace(pos, key.length(), std::to_string(value));
						pos += std::to_string(value).length();
					}
					else {
						++pos;
					}
				}
			}

			// 验证格式
			std::regex validFormat(R"(^[+-]?\d+([+-]\d+)*$)");
			if (!regex_match(processed, validFormat)) {
				throw std::invalid_argument("Invalid queue format: " + input);
			}

			return evaluateExpression(processed);
		}
	};
}