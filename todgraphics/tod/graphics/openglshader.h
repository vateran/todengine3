#pragma once
#include <GL/glew.h>
#include "tod/graphics/shader.h"
namespace tod::graphics
{
    
class OpenGlShader : public Derive<OpenGlShader, Shader>
{
public:
    OpenGlShader():vshader(0), fshader(0), program(0)
    {
    }
    virtual~OpenGlShader()
    {
        this->unload();
    }
    
    bool begin() override
    {
        if (0 == this->program) return false;
        glUseProgram(this->program);
        return true;
    }
    
    void end() override
    {
        glUseProgram(0);
    }
    
    bool load(const String& vshader_fname, const String& fshader_fname) override
    {
        this->unload();
        
        //Load Vertext Shader
        if (!this->load_shader(
           vshader_fname,
           this->vshader,
           GL_VERTEX_SHADER)) return false;
        
        //Load Fragment Shader
        if (!this->load_shader(
           fshader_fname,
           this->fshader,
           GL_FRAGMENT_SHADER)) return false;
        
        this->program = glCreateProgram();
        glAttachShader(this->program, this->vshader);
        glAttachShader(this->program, this->fshader);
        glLinkProgram(this->program);
        if (!this->check_program(this->program))
        {
            this->unload();
            return false;
        }
        
        return true;
    }
    
    void unload() override
    {
        if (0 != this->fshader)
        {
            glDetachShader(this->program, this->fshader);
            glDeleteShader(this->fshader);
        }
        if (0 != this->vshader)
        {
            glDetachShader(this->program, this->vshader);
            glDeleteShader(this->vshader);
        }
        if (0 != this->program)
            glDeleteProgram(this->program);
        this->fshader = 0;
        this->vshader = 0;
        this->program = 0;
    }
    
private:
    bool load_shader(const String& file_name, GLuint& shader, GLenum shader_type)
    {
        if (!FileSystem::instance()->load(
        file_name, [this, &shader, shader_type](FileSystem::Data& data)
        {
            const char* shader_data = &data[0];
            shader = glCreateShader(shader_type);
            glShaderSource(shader, 1, &shader_data, nullptr);
            glCompileShader(shader);
            if (!this->check_shader(shader))
            {
                this->unload();
                return false;
            }
            return true;
        }, FileSystem::LoadOption().string())) return false;
        return true;
    }
    bool check_shader(GLuint shader)
    {
        GLint state;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &state);
        if (GL_FALSE == state)
        {
            int infolog_len = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infolog_len);
            if (infolog_len > 1)
            {
                int chars = 0;
                std::string error_log;
                error_log.resize(infolog_len);
                glGetShaderInfoLog(shader, infolog_len, &chars, &error_log[0]);
                TOD_THROW_EXCEPTION(error_log.c_str());
            }
            return false;
        }
        return true;
    }
    bool check_program(GLuint program)
    {
        GLint state;
        glGetProgramiv(program, GL_LINK_STATUS, &state);
        if (GL_FALSE == state)
        {
            int infolog_len = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infolog_len);
            if (infolog_len > 1)
            {
                int chars = 0;
                std::string error_log;
                error_log.resize(infolog_len);
                glGetProgramInfoLog(program, infolog_len, &chars, &error_log[0]);
                TOD_THROW_EXCEPTION(error_log.c_str());
            }
            return false;
        }
        return true;
    }
    
private:
    GLuint vshader;
    GLuint fshader;
    GLuint program;
};

}
