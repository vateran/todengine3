#pragma once
#include "tod/node.h"
namespace tod::graphics
{
    
class Config : public SingletonDerive<Config, Node>
{
public:
    Config() :
    shaderDebug(false),
    shaderOptimization(false)
    {}
    
    void setShaderDebug(bool value) { this->shaderDebug = value; }
    bool isShaderDebug() { return this->shaderDebug; }
    void setShaderOptimization(bool value) { this->shaderDebug = value; }
    bool isShaderOptimization() { return this->shaderOptimization; }
    
    static void bindProperty()
    {
        BIND_PROPERTY(bool, "shader_debug", "Shader µπˆ±Î »∞º∫»≠", setShaderDebug, isShaderDebug, false, PropertyAttr::DEFAULT);
        BIND_PROPERTY(bool, "shader_optimization", "Shader √÷¿˚»≠ »∞º∫»≠", setShaderOptimization, isShaderOptimization, false, PropertyAttr::DEFAULT);
    }
    
private:
    bool shaderDebug;
    bool shaderOptimization;
};
    
}
