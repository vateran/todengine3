#pragma once
#include "tod/string.h"
#include "tod/object.h"
namespace tod::graphics
{
    class Texture;

    class Material
    {
    public:
        String name;
        String textureUri;
        Texture* texture;
    };

    class MaterialCache : public SingletonDerive<MaterialCache, Object>
    {
    public:
        void addMaterial(const Material& material)
        {
            this->materials[material.name.hash()] = material;
        }

        Material* getMaterial(const String& name)
        {
            auto iter = this->materials.find(name.hash());
            if (this->materials.end() == iter)
            {
                return nullptr;
            }

            return &iter->second;
        }

    private:
        typedef std::unordered_map<int32, Material> Materials;
        Materials materials;
    };
}