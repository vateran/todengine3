#include <regex>
#include <sstream>
#include "tod/graphics/vector2.h"
#include "tod/graphics/vector3.h"
#include "tod/graphics/vector4.h"
#include "tod/graphics/color.h"
#include "tod/graphics/colorf.h"
#include "tod/graphics/matrix44.h"
#include "tod/graphics/openglfunc.h"
#include "tod/graphics/todmath.h"
#include "tod/graphics/openglshader.h"
#include "tod/graphics/opengltexture.h"
namespace tod::graphics
{
    
//-----------------------------------------------------------------------------
OpenGlShader::OpenGlShader():
  vshader(0)
, fshader(0)
, program(0)
, dirty(false)
{
}


//-----------------------------------------------------------------------------
OpenGlShader::~OpenGlShader()
{
    this->unload();
}


//-----------------------------------------------------------------------------
bool OpenGlShader::begin()
{
    if (0 == this->program) TOD_RETURN_TRACE(false);
    OpenGl::funcs->glUseProgram(this->program);
    return true;
}


//-----------------------------------------------------------------------------
void OpenGlShader::end()
{
    if (this->dirty)
    {
        this->dirty = true;
        this->commitParams();
    }
    
    OpenGl::funcs->glUseProgram(0);
}


//-----------------------------------------------------------------------------
bool OpenGlShader::load
(const String& vshader_fname, const String& fshader_fname)
{
    this->unload();
    
    //Load Vertext Shader
    if (!this->load_shader(
       vshader_fname,
       this->vshader,
       GL_VERTEX_SHADER)) TOD_RETURN_TRACE(false);
    
    //Load Fragment Shader
    if (!this->load_shader(
       fshader_fname,
       this->fshader,
       GL_FRAGMENT_SHADER)) TOD_RETURN_TRACE(false);
    
    this->program = OpenGl::funcs->glCreateProgram();
    OpenGl::funcs->glAttachShader(this->program, this->vshader);
    OpenGl::funcs->glAttachShader(this->program, this->fshader);
    OpenGl::funcs->glLinkProgram(this->program);
    
    String error_log;
    if (!this->check_program(this->program, error_log))
    {
        this->unload();
        TOD_THROW_EXCEPTION(S("%s,%s:%s"),
            &vshader_fname[0],
            &fshader_fname[0],
            &error_log[0]);
        TOD_RETURN_TRACE(false);
    }
    
    GLint attr_count;
    OpenGl::funcs->glGetProgramiv(this->program, GL_ACTIVE_ATTRIBUTES, &attr_count);
    GLchar name_buf[1024];
    GLsizei name_len;
    GLint attr_var_size;
    GLenum attr_type;
    for (int index=0;index<attr_count;++index)
    {
        OpenGl::funcs->glGetActiveAttrib(this->program, index,
            sizeof(name_buf), &name_len,
            &attr_var_size, &attr_type, name_buf);
        printf("%s\n", name_buf);
    }
    
    return true;
}


//-----------------------------------------------------------------------------
void OpenGlShader::unload()
{
    if (0 != this->fshader)
    {
        OpenGl::funcs->glDetachShader(this->program, this->fshader);
        OpenGl::funcs->glDeleteShader(this->fshader);
    }
    if (0 != this->vshader)
    {
        OpenGl::funcs->glDetachShader(this->program, this->vshader);
        OpenGl::funcs->glDeleteShader(this->vshader);
    }
    if (0 != this->program)
        OpenGl::funcs->glDeleteProgram(this->program);
    this->fshader = 0;
    this->vshader = 0;
    this->program = 0;
}


//-----------------------------------------------------------------------------
bool OpenGlShader::setParam(const String& name, const std::any& value)
{
    auto i = this->shaderParams.find(name.hash());
    if (this->shaderParams.end() == i)
    {
        auto location = OpenGl::funcs->glGetUniformLocation(
            this->program, name.c_str());
        if (-1 == location)
        {
            TOD_LOG("error", "Not found shader param[%s]\n", name.c_str());
            return false;
        }
        
        auto shader_param = ShaderParamPtr(
            new ShaderParam(name, location, value));
        this->shaderParams[name.hash()] = shader_param;
        return true;
    }
    
    i->second->value = value;
    i->second->dirty = true;
    this->dirty = true;
    
    return true;
}


//-----------------------------------------------------------------------------
bool OpenGlShader::commitParams()
{
    for (auto i : this->shaderParams)
    {
        auto& shader_param = i.second;
        if (!shader_param->dirty) continue;
        
        const auto& shader_value = shader_param->value;
        if (typeid(bool)==shader_value.type())
        {
            const auto& v = std::any_cast<bool>(shader_value);
            OpenGl::funcs->glUniform1i(shader_param->location, v);
            shader_param->dirty = false;
        }
        else if (typeid(int)==shader_value.type())
        {
            const auto& v = std::any_cast<int>(shader_value);
            OpenGl::funcs->glUniform1i(shader_param->location, v);
            shader_param->dirty = false;
        }
        else if (typeid(float)==shader_value.type())
        {
            const auto& v = std::any_cast<float>(shader_value);
            OpenGl::funcs->glUniform1f(shader_param->location, v);
            shader_param->dirty = false;
        }
        else if (typeid(Vector2)==shader_value.type())
        {
            const auto& v = std::any_cast<Vector2>(shader_value);
            OpenGl::funcs->glUniform2f(
                shader_param->location, v.x, v.y);
            shader_param->dirty = false;
        }
        else if (typeid(Vector3)==shader_value.type())
        {
            const auto& v = std::any_cast<Vector3>(shader_value);
            OpenGl::funcs->glUniform3f(
                shader_param->location, v.x, v.y, v.z);
            shader_param->dirty = false;
        }
        else if (typeid(Vector4)==shader_value.type())
        {
            const auto& v = std::any_cast<Vector4>(shader_value);
            OpenGl::funcs->glUniform4f(
                shader_param->location, v.x, v.y, v.z, v.w);
            shader_param->dirty = false;
        }
        else if (typeid(Color)==shader_value.type())
        {
            const auto& v = std::any_cast<Color>(shader_value);
            ColorF c(v);
            OpenGl::funcs->glUniform3f(
                shader_param->location, c.r, c.g, c.b);
            shader_param->dirty = false;
        }
        else if (typeid(Matrix44)==shader_value.type())
        {
            const auto& v = std::any_cast<Matrix44>(shader_value);
            OpenGl::funcs->glUniformMatrix4fv(
                shader_param->location, 1, GL_FALSE, &v.array[0]);
            shader_param->dirty = false;
        }
        else if (typeid(Texture*)==shader_value.type())
        {
            const auto& v = std::any_cast<Texture*>(shader_value);
            auto ogltexture = static_cast<OpenGlTexture*>(v);
            OpenGl::funcs->glUniform1i(
                shader_param->location, ogltexture->getSamplerIndex());
            shader_param->dirty = false;
        }
        else
        {
            TOD_LOG("warning", "%s type is not supported\n",
                shader_param->name.c_str());
            TOD_RETURN_TRACE(false);
        }
    }
    
    return true;
}


//-----------------------------------------------------------------------------
bool OpenGlShader::load_shader
(const String& file_name, GLuint& shader, GLenum shader_type)
{
    if (!FileSystem::instance()->load(
    file_name, [this, &shader, file_name, shader_type](FileSystem::Data& data)
    {
        std::string output;
        if (!this->preprocess(data, output))
        {
            this->unload();
            TOD_RETURN_TRACE(false);
        }
        const char* shader_data[] = { &output[0] };
        shader = OpenGl::funcs->glCreateShader(shader_type);
        OpenGl::funcs->glShaderSource(shader, 1, shader_data, nullptr);
        OpenGl::funcs->glCompileShader(shader);
        
        String error_log;
        if (!this->check_shader(shader, error_log))
        {
            this->unload();
            TOD_THROW_EXCEPTION(S("%s:%s"), &file_name[0], &error_log[0]);
            TOD_RETURN_TRACE(false);
        }
        return true;
    }, FileSystem::LoadOption().string())) TOD_RETURN_TRACE(false);
    return true;
}


//-----------------------------------------------------------------------------
bool OpenGlShader::preprocess(FileSystem::Data& data, std::string& output)
{
    std::regex re("^[ \t]*#include[ \t]+[\"<](.*)[\">].*");
    std::smatch matches;
    std::stringstream stream(&data[0]);
    std::string line;
    output.reserve(data.size());
    while (std::getline(stream, line))
    {
        output += "\n";
        if (std::regex_search(line, matches, re))
        {
            std::string include_file = matches[1];
            
            FileSystem::Data include_data;
            if (!FileSystem::instance()->load(include_file, include_data,
                FileSystem::LoadOption().string()))
                TOD_RETURN_TRACE(false);
            output += &include_data[0];
        }
        else
        {
            output += line;
        }
    }

    return true;
}


//-----------------------------------------------------------------------------
bool OpenGlShader::check_shader(GLuint shader, String& error_log)
{
    GLint state;
    OpenGl::funcs->glGetShaderiv(shader, GL_COMPILE_STATUS, &state);
    if (GL_FALSE == state)
    {
        int infolog_len = 0;
        OpenGl::funcs->glGetShaderiv(
            shader, GL_INFO_LOG_LENGTH, &infolog_len);
        if (infolog_len > 1)
        {
            int chars = 0;
            error_log.resize(infolog_len);
            OpenGl::funcs->glGetShaderInfoLog(
                shader, infolog_len, &chars, &error_log[0]);
        }
        TOD_RETURN_TRACE(false);
    }
    return true;
}


//-----------------------------------------------------------------------------
bool OpenGlShader::check_program(GLuint program, String& error_log)
{
    GLint state;
    OpenGl::funcs->glGetProgramiv(program, GL_LINK_STATUS, &state);
    if (GL_FALSE == state)
    {
        int infolog_len = 0;
        OpenGl::funcs->glGetProgramiv(
            program, GL_INFO_LOG_LENGTH, &infolog_len);
        if (infolog_len > 1)
        {
            int chars = 0;
            error_log.resize(infolog_len);
            OpenGl::funcs->glGetProgramInfoLog(
                program, infolog_len, &chars, &error_log[0]);
        }
        TOD_RETURN_TRACE(false);
    }
    return true;
}


}
