#pragma once
#include "tod/filesystem.h"
#include "tod/graphics/shader.h"
namespace tod::graphics
{
    
class OpenGlShader : public Derive<OpenGlShader, Shader>
{
public:
    struct ShaderParam
    {
        ShaderParam(const String& name, GLint location, const std::any& value):
        name(name), location(location), value(value), dirty(true) {}
        String name;
        GLint location;
        std::any value;
        bool dirty;
    };
    typedef std::shared_ptr<ShaderParam> ShaderParamPtr;
    typedef std::unordered_map<int, ShaderParamPtr> ShaderParams;

public:
    OpenGlShader();
    virtual~OpenGlShader();
    
    bool begin() override;
    void end() override;
    bool load(const String& vshader_fname, const String& fshader_fname) override;
    void unload() override;
    bool setParam(const String& name, const std::any& value) override;
    bool commitParams() override;
    
private:
    bool load_shader(const String& file_name, GLuint& shader, GLenum shader_type);
    bool preprocess(FileSystem::Data& data, std::string& output);
    bool check_shader(GLuint shader, String& error_log);
    bool check_program(GLuint program, String& error_log);
    
private:
    GLuint vshader;
    GLuint fshader;
    GLuint program;
    ShaderParams shaderParams;
    bool dirty;
};

}
