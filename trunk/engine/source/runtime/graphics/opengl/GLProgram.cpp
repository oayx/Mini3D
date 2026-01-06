#include "GLProgram.h"
#include "GLDevice.h"
#include "core/stream/DataStream.h"
#include "runtime/graphics/TextureUnit.h"
#include "runtime/graphics/null/Texture.h"
#include "runtime/graphics/null/RenderTexture.h"
#include "runtime/Application.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
static bool GL_CheckShaderSuccess(GLuint object, PFNGLGETSHADERIVPROC infoFunc, PFNGLGETSHADERINFOLOGPROC logFunc, int StatusId)
{
	GLint success = 0, logLength = 0, charsWritten = 0;
	infoFunc(object, StatusId, &success);
	if (success)
		return true;

	infoFunc(object, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength < 1)
		return true;

	GLchar logBytes[2048];
	logFunc(object, sizeof(logBytes) - 1, &charsWritten, logBytes);
	logBytes[charsWritten] = 0;
	AssertEx(false, "GL_CheckShaderSuccess:%s", logBytes);
	return false;
}
static String GetCSOFile(const String& name)
{
	String file = name;
	if (file.IndexOf("assets") >= 0)
		file = file.Substring(file.IndexOf("assets") + 7);
	file = file.Replace('.', '_');
	file = file.Replace('/', '_');
	file = file.Replace('\\', '_');
	return file;
}
static bool GL_SupportShader(GLenum shader_type)
{
	switch (shader_type)
	{
	case GL_VERTEX_SHADER:
	case GL_FRAGMENT_SHADER:return true;
	case GL_TESS_CONTROL_SHADER:return GetGraphicsCaps()->hasHullShader;
	case GL_TESS_EVALUATION_SHADER:return GetGraphicsCaps()->hasDomainShader;
	case GL_GEOMETRY_SHADER:return GetGraphicsCaps()->hasGeometryShader;
	case GL_COMPUTE_SHADER: return GetGraphicsCaps()->hasComputeShader;
	}
	return false;
}
GLuint GL_CompileShader(const String& name, const VecString& codes)
{
	String filePath = "";
	if (GetGraphicsCaps()->useProgramBinary)
	{
		//查看是否有已经编译过的
		filePath = GetCSOFile(name);
		filePath = Resource::GetFullSavePath("gl/" + filePath + ".bin");
		if (File::Exist(filePath))
		{
			MemoryDataStream stream;
			if (File::ReadAllBytes(filePath, stream))
			{
				GLuint program = glCreateProgram();
				byte* binary = stream.Buffer();
				GL_ERROR(glProgramBinary(program, *((GLenum*)binary), binary + sizeof(GLenum), stream.Size() - sizeof(GLenum)));

				int linked = 0;
				GL_ERROR(glGetProgramiv(program, GL_LINK_STATUS, &linked));
				if (linked != 0)
					return program;
				else
					GL_ERROR(glDeleteProgram(program));
			}
		}
	}
	bool has_valid_shader = false;
	GLuint shader_id[int(ShaderType::Max)] = { 0 };
	for (int i = 0; i < codes.Size(); ++i)
	{
		const String& code = codes[i];
		if (code.IsEmpty())continue;

		GLenum shader_type = GL_ShaderType[i];
		if (!GL_SupportShader(shader_type))continue;

		const char* shader_code = code.c_str();
		//Debuger::Log(shader_code);
		shader_id[i] = glCreateShader(GL_ShaderType[i]); GL_CHECK_ERROR();
		glShaderSource(shader_id[i], 1, &shader_code, NULL); GL_CHECK_ERROR();
		glCompileShader(shader_id[i]); GL_CHECK_ERROR();
		if (!GL_CheckShaderSuccess(shader_id[i], glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS))
			shader_id[i] = 0;
		else
			has_valid_shader = true;
	}
	if (!has_valid_shader)
	{
		Debuger::Error("GL_CompileShader error:%s",name.c_str());
		return 0;
	}

	// 着色器程序
	GL_DEBUG_ERROR(glGetError(), "glCreateProgram");
	GLuint program = glCreateProgram();
	for (int i = 0; i < int(ShaderType::Max); ++i)
	{
		if (shader_id[i] > 0)
		{
			GL_ERROR(glAttachShader(program, shader_id[i]));
		}
	}
	GL_ERROR(glLinkProgram(program));
#if defined(DC_DEBUG)
	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if(success != GL_TRUE)
	{
		GLchar infoLog[512];
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		Debuger::Error("glLinkProgram error:%s",infoLog);	
	}
#endif
	// 删除着色器，它们已经链接到我们的程序中了，已经不再需要了
	for (int i = 0; i < int(ShaderType::Max); ++i)
	{
		if (shader_id[i] > 0)
		{
			GL_ERROR(glDeleteShader(shader_id[i]));
		}
	}
	//保存编译后的文件
	if (GetGraphicsCaps()->useProgramBinary)
	{
		GLint binary_size = 0;
		GL_ERROR(glGetProgramiv(program, GL_PROGRAM_BINARY_LENGTH, &binary_size));
		if (binary_size > 0)
		{
			byte* program_binary = Memory::NewArray<byte>(binary_size + sizeof(GLenum));
			GL_ERROR(glGetProgramBinary(program, binary_size, 0, (GLenum*)program_binary, program_binary + sizeof(GLenum)));

			String path = Path::GetDirectoryName(filePath);
			Directory::Create(path);
			File::WriteAllBytes(filePath, program_binary, binary_size + sizeof(GLenum));
			Memory::DeleteArray(program_binary);
		}
	}
	GL_CHECK_ERROR();
	return program;
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(GLProgram, CGProgram);
GLProgram::~GLProgram()
{
	if (_shaderProgram > 0)
	{
		GL_ERROR(glDeleteProgram(_shaderProgram));
		_shaderProgram = 0;
	}
}
bool GLProgram::LoadFromDesc(const ShaderDesc& info)
{
	base::LoadFromDesc(info);

	//获得编译宏
	String defines = "";
	for (const auto& define : info.ShaderDefines)
	{
		defines += "_" + define;
	}

	String name = "";
	VecString codes;
	codes.Resize(int(ShaderType::Max));
	const String& shader_version = Application::GetGraphics()->GetShaderVersion();
	const String& languageName = ShadingLanguageFileName();
	for (int i = 0; i < int(ShaderType::Max); ++i)
	{
		if (info.ShaderFile[i].IsEmpty())continue;

		String pass_name = "";
		if (info.PassIdx > 0)pass_name = String("_pass") + String::ToString(info.PassIdx);

		String dirPath = Path::GetDirectoryName(info.ShaderFile[i]);
		String fileName = Path::GetFileNameWithoutExtension(info.ShaderFile[i]);
		String filePath = dirPath + "/" + fileName + "_" + languageName + "_" + shader_version + defines + pass_name;
#if defined(DC_COLORSPACE_LINEAR)
		filePath = filePath + "_sRGB";
#endif
		if (name.IsEmpty())name = filePath;
		filePath = Resource::GetFullDataPath(filePath + ShaderFileExtEnum[i]);
		codes[i] = File::ReadAllText(filePath);
	}

	return LoadFromMemory(name, codes, info.ShaderDefines);
}
bool GLProgram::LoadFromMemory(const String& name, const VecString& codes, const VecString& defines)
{
	base::LoadFromMemory(name, codes, defines);
	_shaderProgram = GL_CompileShader(name, codes);
	if (_shaderProgram == 0)
		return false;
	
	{//提取uniform信息
		GLint uniform_count = 0;
		GL_ERROR(glGetProgramiv(_shaderProgram, GL_ACTIVE_UNIFORMS, &uniform_count));
		GLint uniform_name_length = 0;
		GL_ERROR(glGetProgramiv(_shaderProgram, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniform_name_length));
		AssertEx(uniform_name_length < 256, "");
		for (GLint i = 0; i < uniform_count; ++i)
		{
			GLuint index = i;
			GLsizei bufSize = 256;
			GLsizei length;
			GLint size;
			GLenum type;
			GLchar name[256] = { 0 };
			GL_ERROR(glGetActiveUniform(_shaderProgram, index, bufSize, &length, &size, &type, name));
			uint registerId = glGetUniformLocation(_shaderProgram, GetVariableFullName(name).c_str());
			String key = String(name, length);
			_uniformIndexMaps.Add(key, registerId);
			if (size > 1)
			{//array
				key = key.Substring(0, key.IndexOf('['));
				_uniformIndexMaps.Add(key, registerId);
			}
		}
	}
	{//提取顶点输入属性
		int attrib_count;
		GL_ERROR(glGetProgramiv(_shaderProgram, GL_ACTIVE_ATTRIBUTES, &attrib_count));
		AssertEx(attrib_count <= GetGraphicsCaps()->maxVertexAttribs, "");
		for (uint i = 0; i < (uint)attrib_count; ++i)
		{
			GLuint index = i;
			GLsizei bufSize = 256;
			GLsizei length;
			GLint size;
			GLenum type;
			GLchar a_name[256] = { 0 };
			GL_ERROR(glGetActiveAttrib(_shaderProgram, index, bufSize, &length, &size, &type, a_name));
			VertexAttrib attrib;
			attrib.index = (byte)i;
			attrib.name = a_name;
			switch (type)
			{
			case GL_FLOAT:		attrib.type = VertexSize::Float1; break;
			case GL_FLOAT_VEC2:	attrib.type = VertexSize::Float2; break;
			case GL_FLOAT_VEC3:	attrib.type = VertexSize::Float3; break;
			case GL_FLOAT_VEC4:	attrib.type = VertexSize::Float4; break;
			case GL_UNSIGNED_INT:attrib.type = VertexSize::Byte4; break;
			case GL_UNSIGNED_BYTE:
				AssertEx(size == 4, "");
				attrib.type = VertexSize::Byte4;
				break;
			case GL_FLOAT_MAT4:
				break;
			default:
				AssertEx(false, "unkown type:%d", type);
				continue;
			}
			_vertexAttribs.Add(attrib);
		}
	}
	return _shaderProgram != 0;
}
void GLProgram::PreRender()
{
	if (_shaderProgram > 0)
	{
		GL_ERROR(glUseProgram(_shaderProgram));
	}
}
void GLProgram::PostRender()
{
}
bool GLProgram::SetVariable(const String& name, const Matrix4& mat)
{
	uint registerId;
	if (!_uniformIndexMaps.TryGet(GetVariableFullName(name), &registerId))
	{
		registerId = glGetUniformLocation(_shaderProgram, GetVariableFullName(name).c_str());
		if (glGetError() > 0 || registerId == -1)
			return false;
	}

	//指明矩阵是列优先(column major)矩阵（GL_FALSE）还是行优先(row major)矩阵（GL_TRUE）
#if defined(DC_GRAPHICS_API_OPENGL) || defined(DC_GRAPHICS_API_OPENGLES3)
	GL_ERROR(glUniformMatrix4fv(registerId, 1, GL_TRUE, (const float*)mat.p));
#else
	//如果设置GL_TRUE, OPENGLES2.0会出错；这里转置下矩阵
	Matrix4 t_mat = mat.Transpose();
	GL_ERROR(glUniformMatrix4fv(registerId, 1, GL_FALSE, (const float*)t_mat.p));
#endif
	return true;
}
bool GLProgram::SetVariable(const String& name, const Matrix4* mats, int count)
{
	uint registerId;
	if (!_uniformIndexMaps.TryGet(GetVariableFullName(name), &registerId))
	{
		registerId = glGetUniformLocation(_shaderProgram, GetVariableFullName(name).c_str());
		if (glGetError() > 0 || registerId == -1)
			return false;
	}

#if defined(DC_GRAPHICS_API_OPENGL) || defined(DC_GRAPHICS_API_OPENGLES3)
	GL_ERROR(glUniformMatrix4fv(registerId, count, GL_TRUE, (const float*)mats));
#else
	//如果设置GL_TRUE, OPENGLES2.0会出错；这里转置下矩阵
	Vector<Matrix4> t_mat(count);
	for (int i = 0; i < count; ++i)
	{
		t_mat[i] = mats->Transpose();
		mats += 1;
	}
	GL_ERROR(glUniformMatrix4fv(registerId, count, GL_FALSE, (const float*)t_mat[0].p));
#endif
	return true;
}
bool GLProgram::SetVariable(const String& name, const float* values, int count)
{
	uint registerId;
	if (!_uniformIndexMaps.TryGet(GetVariableFullName(name), &registerId))
	{
		registerId = glGetUniformLocation(_shaderProgram, GetVariableFullName(name).c_str());
		if (glGetError() > 0 || registerId == -1)
			return false;
	}

	if (count >= 5)
	{
		GL_ERROR(glUniform1fv(registerId, count, values));
	}
	else
	{
		switch (count)
		{
		case 4:GL_ERROR(glUniform4fv(registerId, 1, values)); break;
		case 3:GL_ERROR(glUniform3fv(registerId, 1, values)); break;
		case 2:GL_ERROR(glUniform2fv(registerId, 1, values)); break;
		case 1:GL_ERROR(glUniform1f(registerId, values[0])); break;
		default:AssertEx(false, ""); break;
		}
	}
	return false;
}
bool GLProgram::SetVariable(const String& name, const Vector4* values, int count)
{
	uint registerId;
	if (!_uniformIndexMaps.TryGet(GetVariableFullName(name), &registerId))
	{
		registerId = glGetUniformLocation(_shaderProgram, GetVariableFullName(name).c_str());
		if (glGetError() > 0 || registerId == -1)
			return false;
	}

	GL_ERROR(glUniform4fv(registerId, count, (float*)values));
	return false;
}
bool GLProgram::SetVariable(const String& name, const int* values, int count)
{
	uint registerId;
	if (!_uniformIndexMaps.TryGet(GetVariableFullName(name), &registerId))
	{
		registerId = glGetUniformLocation(_shaderProgram, GetVariableFullName(name).c_str());
		if (glGetError() > 0 || registerId == -1)
			return false;
	}

	int nCount = count / 4;
	if (nCount > 0)
	{
		GL_ERROR(glUniform4iv(registerId, nCount, values));
		values += 4 * nCount;
		count -= 4 * nCount;
		registerId += nCount;
	}
	if (count > 0)
	{
		if (count >= 3)
		{
			GL_ERROR(glUniform3iv(registerId, 1, values));
		}
		else if (count >= 2)
		{
			GL_ERROR(glUniform2iv(registerId, 1, values));
		}
		else
		{
			GL_ERROR(glUniform1i(registerId, values[0]));
		}
	}
	return false;
}
bool GLProgram::SetVariable(const String& name, const bool values)
{
	uint registerId;
	if (!_uniformIndexMaps.TryGet(GetVariableFullName(name), &registerId))
	{
		registerId = glGetUniformLocation(_shaderProgram, GetVariableFullName(name).c_str());
		if (glGetError() > 0 || registerId == -1)
			return false;
	}

	GL_ERROR(glUniform1i(registerId, (int)values));
	return true;
}
bool GLProgram::SetVariable(const String& name, const ShaderLight& light, int index)
{
	if (_shaderProgram == 0)return false;
	String pre_name = GetVariableFullName(name) + "[" + String::ToString(index) + "].";

	String full_name = pre_name + "Dir";
	uint registerId = -1;
	if (_uniformIndexMaps.TryGet(full_name, &registerId) || (registerId = glGetUniformLocation(_shaderProgram, full_name.c_str())) != -1)
	{
		GL_ERROR(glUniform3fv(registerId, 1, light.Dir.ptr()));
	}

	full_name = pre_name + "Type";
	if (_uniformIndexMaps.TryGet(full_name, &registerId) || (registerId = glGetUniformLocation(_shaderProgram, full_name.c_str())) != -1)
	{
		GL_ERROR(glUniform1i(registerId, light.Type));
	}

	full_name = pre_name + "Color";
	if (_uniformIndexMaps.TryGet(full_name, &registerId) || (registerId = glGetUniformLocation(_shaderProgram, full_name.c_str())) != -1)
	{
		GL_ERROR(glUniform3fv(registerId, 1, light.Color.ptr()));
	}

	full_name = pre_name + "Attenuation";
	if (_uniformIndexMaps.TryGet(full_name, &registerId) || (registerId = glGetUniformLocation(_shaderProgram, full_name.c_str())) != -1)
	{
		GL_ERROR(glUniform3fv(registerId, 1, light.Attenuation.ptr()));
	}

	full_name = pre_name + "Range";
	if (_uniformIndexMaps.TryGet(full_name, &registerId) || (registerId = glGetUniformLocation(_shaderProgram, full_name.c_str())) != -1)
	{
		GL_ERROR(glUniform1f(registerId, light.Range));
	}

	full_name = pre_name + "Position";
	if (_uniformIndexMaps.TryGet(full_name, &registerId) || (registerId = glGetUniformLocation(_shaderProgram, full_name.c_str())) != -1)
	{
		GL_ERROR(glUniform3fv(registerId, 1, light.Position.ptr()));
	}

	full_name = pre_name + "SpotAngle";
	if (_uniformIndexMaps.TryGet(full_name, &registerId) || (registerId = glGetUniformLocation(_shaderProgram, full_name.c_str())) != -1)
	{
		GL_ERROR(glUniform1f(registerId, light.SpotAngle));
	}

	GL_DEBUG_ERROR(glGetError(), "SetVariable - light");
	return true;
}
bool GLProgram::SetVariable(byte index, TextureUnit* tex_unit)
{
	if (!tex_unit || !tex_unit->GetTexture())return false;

	const String& name = tex_unit->Name;
	uint registerId;
	if (!_uniformIndexMaps.TryGet(GetVariableFullName(name), &registerId))
	{
		registerId = glGetUniformLocation(_shaderProgram, GetVariableFullName(name).c_str());
		if (glGetError() > 0 || registerId == -1)
			return false;
	}
	//激活纹理
	GL_ERROR(glActiveTexture(GL_TEXTURE0 + index));

	//设置纹理
	Texture* texture = tex_unit->GetTexture();
	if (texture == nullptr)return false;
	GLuint* ogl_texture = (GLuint*)texture->GetTextureView();
	GLuint texture_type = GL_GetTextureType(texture->GetTextureType(), false);
	GL_ERROR(glBindTexture(texture_type, (GLuint)(*ogl_texture)));
	GL_ERROR(glUniform1i(registerId, index));
	
	//UV缩放和偏移
	Vector4 tex_uv(tex_unit->TextureScale.x, tex_unit->TextureScale.y, tex_unit->TextureOffset.x, tex_unit->TextureOffset.y);
	SetVariable(name + "_ST", tex_uv.ptr(), 4);

	//贴图大小
	Vector4 tex_size(1.0f / (float)texture->GetWidth(), 1.0f / (float)texture->GetHeight(), (float)texture->GetWidth(), (float)texture->GetHeight());
	SetVariable(name + "_TexelSize", tex_size.ptr(), 4);

	{//渲染状态
		//纹理寻址模式
		GL_ERROR(glTexParameteri(texture_type, GL_TEXTURE_WRAP_T, (GLuint)GL_GetTextureAddress(tex_unit->AddressMode)));
		GL_ERROR(glTexParameteri(texture_type, GL_TEXTURE_WRAP_S, (GLuint)GL_GetTextureAddress(tex_unit->AddressMode)));
		if (texture->GetTextureType() == TextureType::Cube)GL_ERROR(glTexParameteri(texture_type, GL_TEXTURE_WRAP_R, (GLuint)GL_GetTextureAddress(tex_unit->AddressMode)));
		if (tex_unit->AddressMode == TextureAddress::Border)GL_ERROR(glTexParameterfv(texture_type, GL_TEXTURE_BORDER_COLOR, tex_unit->BorderColor.p));

		//纹理过滤
		GL_ERROR(glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, GL_GetMinTextureFilter(tex_unit->FilterType)));
		GL_ERROR(glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, GL_GetMagTextureFilter(tex_unit->FilterType)));

		//各向异性
		if (GetGLDevice()->GetCaps()->hasAnisoFilter)
		{
#if defined(GL_EXT_texture_filter_anisotropic)
			GL_ERROR(glTexParameteri(texture_type, GL_TEXTURE_MAX_ANISOTROPY_EXT, Math::Min<int>(Application::GetGraphics()->GetCaps()->maxAnisoLevel, tex_unit->Anisotropy)));
#endif
		}
	}

	return true;
}
bool GLProgram::SetVariable(byte index, const String& name, Texture* texture)
{
	if (!texture)return false;

	uint registerId;
	if (!_uniformIndexMaps.TryGet(GetVariableFullName(name), &registerId))
	{
		registerId = glGetUniformLocation(_shaderProgram, GetVariableFullName(name).c_str());
		if (glGetError() > 0 || registerId == -1)
			return false;
	}

	//激活纹理
	GL_ERROR(glActiveTexture(GL_TEXTURE0 + index));

	//设置纹理
	GLuint* ogl_texture = (GLuint*)texture->GetTextureView();
	GLuint texture_type = GL_GetTextureType(texture->GetTextureType(),false);
	GL_ERROR(glBindTexture(texture_type, (GLuint)(*ogl_texture)));
	GL_ERROR(glUniform1i(registerId, index));

	//UV缩放和偏移
	Vector4 tex_uv(1, 1, 0, 0);
	SetVariable(name + "_ST", tex_uv.ptr(), 4);

	//贴图大小
	Vector4 tex_size(1.0f / (float)texture->GetWidth(), 1.0f / (float)texture->GetHeight(), (float)texture->GetWidth(), (float)texture->GetHeight());
	SetVariable(name + "_TexelSize", tex_size.ptr(), 4);

	{//渲染状态
		//纹理寻址模式
		GL_ERROR(glTexParameteri(texture_type, GL_TEXTURE_WRAP_T, (GLuint)GL_GetTextureAddress(texture->GetAddressMode())));
		GL_ERROR(glTexParameteri(texture_type, GL_TEXTURE_WRAP_S, (GLuint)GL_GetTextureAddress(texture->GetAddressMode())));
		if (texture->GetTextureType() == TextureType::Cube)glTexParameteri(texture_type, GL_TEXTURE_WRAP_R, (GLuint)GL_GetTextureAddress(texture->GetAddressMode()));

		//纹理过滤
		GL_ERROR(glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, GL_GetMinTextureFilter(texture->GetFilterType())));
		GL_ERROR(glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, GL_GetMagTextureFilter(texture->GetFilterType())));

		//各向异性
#if defined(GL_EXT_texture_filter_anisotropic)
		if (GetGLDevice()->GetCaps()->hasAnisoFilter)
		{
			int anisotropy = texture->GetFilterType() == TextureFilter::Trilinear ? 4 : 1;
			GL_ERROR(glTexParameteri(texture_type, GL_TEXTURE_MAX_ANISOTROPY_EXT, Math::Min<int>(Application::GetGraphics()->GetCaps()->maxAnisoLevel, anisotropy)));
		}
#endif
	}

	return true;
}
bool GLProgram::SetPassVariable(byte index, const String& name, Texture* texture)
{
	if (!texture)return false;

	uint registerId;
	if (!_uniformIndexMaps.TryGet(GetVariableFullName(name), &registerId))
	{
		registerId = glGetUniformLocation(_shaderProgram, GetVariableFullName(name).c_str());
		if (glGetError() > 0 || registerId == -1)
			return false;
	}

	//激活纹理
	GL_ERROR(glActiveTexture(GL_TEXTURE0 + index));

	//设置纹理
	GLuint* ogl_texture = (GLuint*)texture->GetTextureView();
	GLuint texture_type = GL_GetTextureType(texture->GetTextureType(),false);
	GL_ERROR(glBindTexture(texture_type, (GLuint)(*ogl_texture)));
	GL_ERROR(glUniform1i(registerId, index));

	//UV缩放和偏移
	Vector4 tex_uv(1, 1, 0, 0);
	SetVariable(name + "_ST", tex_uv.ptr(), 4);

	//贴图大小
	Vector4 tex_size(1.0f / (float)texture->GetWidth(), 1.0f / (float)texture->GetHeight(), (float)texture->GetWidth(), (float)texture->GetHeight());
	SetVariable(name + "_TexelSize", tex_size.ptr(), 4);

	return true;
}
uint GLProgram::GetUniformIndex(const String& name)const
{
	if (_shaderProgram == 0)return -1;

	uint registerId = glGetUniformLocation(_shaderProgram, GetVariableFullName(name).c_str());
	if (glGetError() > 0 || registerId == -1)
	{
		return -1;
	}
	return registerId;
}
DC_END_NAMESPACE