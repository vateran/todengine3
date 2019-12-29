#include "tod/filesystem.h"
#include "tod/graphics/renderer.h"
#include "tod/graphics/renderinterface.h"
#include "tod/graphics/mesh.h"
#include "tod/graphics/inputlayout.h"
#include "tod/graphics/vertexbuffer.h"
#include "tod/graphics/indexbuffer.h"
#include "tod/graphics/texture.h"
#include "tod/graphics/material.h"
#include "tod/graphics/gltfloader.h"
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tinygltf/tiny_gltf.h"
namespace tod::graphics
{

class GLTFLoader::GLTFLoaderPrivate
{
public:
    void bindMesh(tinygltf::Model& gltf_model, tinygltf::Mesh& gltf_mesh, Mesh* out_mesh)
    {
        for (size_t i = 0; i < gltf_model.bufferViews.size(); ++i)
        {
            const tinygltf::BufferView& bufferView = gltf_model.bufferViews[i];
            if (bufferView.target == 0)
            {
                continue;  // Unsupported bufferView.
            }

            const tinygltf::Buffer& buffer = gltf_model.buffers[bufferView.buffer];

            Mesh::SubMesh* sub_mesh = out_mesh->newSubMesh(buffer.name.c_str());

            tinygltf::Primitive& prim = gltf_mesh.primitives[i];
            tinygltf::Accessor& pos = gltf_model.accessors[prim.attributes["POSITION"]];
            


            //****** 여기서 SubMesh 생성해야 하나?

            //glBufferData(bufferView.target, bufferView.byteLength,
            //    &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
        }

        for (size_t i = 0; i < gltf_mesh.primitives.size(); ++i)
        {
            tinygltf::Primitive primitive = gltf_mesh.primitives[i];
            tinygltf::Accessor indexAccessor = gltf_model.accessors[primitive.indices];

            for (auto& attrib : primitive.attributes)
            {
                tinygltf::Accessor accessor = gltf_model.accessors[attrib.second];
                int byteStride =
                    accessor.ByteStride(gltf_model.bufferViews[accessor.bufferView]);

                //glBindBuffer(GL_ARRAY_BUFFER, vbos[accessor.bufferView]);

                int size = 1;
                if (accessor.type != TINYGLTF_TYPE_SCALAR) {
                    size = accessor.type;
                }

                int vaa = -1;
                if (attrib.first.compare("POSITION") == 0) vaa = 0;
                if (attrib.first.compare("NORMAL") == 0) vaa = 1;
                if (attrib.first.compare("TEXCOORD_0") == 0) vaa = 2;
                if (vaa > -1)
                {
                    //glEnableVertexAttribArray(vaa);
                    //glVertexAttribPointer(vaa, size, accessor.componentType,
                    //    accessor.normalized ? GL_TRUE : GL_FALSE,
                    //    byteStride, BUFFER_OFFSET(accessor.byteOffset));
                }
                else
                {
                    //std::cout << "vaa missing: " << attrib.first << std::endl;
                }
            }

            //GLuint texid;
            //glGenTextures(1, &texid);

            tinygltf::Texture& tex = gltf_model.textures[0];
            tinygltf::Image& image = gltf_model.images[tex.source];

            //glBindTexture(GL_TEXTURE_2D, texid);
            //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            //GLenum format = GL_RGBA;

            /*if (image.component == 1)
            {
                format = GL_RED;
            }
            else if (image.component == 2)
            {
                format = GL_RG;
            }
            else if (image.component == 3)
            {
                format = GL_RGB;
            }
            else {
                // ???
            }

            GLenum type = GL_UNSIGNED_BYTE;
            if (image.bits == 8) {
                // ok
            }
            else if (image.bits == 16) {
                type = GL_UNSIGNED_SHORT;
            }
            else {
                // ???
            }

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0,
                format, type, &image.image.at(0));*/
        }
    }

    void bindModelNodes(
          tinygltf::Model& gltf_model
        , tinygltf::Node& gltf_node
        , Mesh* out_mesh)
    {
        bindMesh(gltf_model, gltf_model.meshes[gltf_node.mesh], out_mesh);
        for (size_t i = 0; i < gltf_node.children.size(); i++)
        {
            bindModelNodes(gltf_model, gltf_model.nodes[gltf_node.children[i]], out_mesh);
        }
    }

    bool importMesh(const String& uri, OUT Mesh** mesh)
    {
        Buffer raw_data;
        if (false == FileSystem::instance()->load(uri, &raw_data))
        {
            return false;
        }

        tinygltf::TinyGLTF loader;
        std::string err;
        std::string warn;

        tinygltf::Model model;
        if (false == loader.LoadBinaryFromMemory(
            &model, &err, &warn
            , reinterpret_cast<unsigned char*>(raw_data.data())
            , static_cast<uint32>(raw_data.size())))
        {
            TOD_RETURN_TRACE(false);
        }

        Mesh* out_mesh = new Mesh();
        const tinygltf::Scene& scene = model.scenes[model.defaultScene];
        for (size_t i = 0; i < scene.nodes.size(); ++i)
        {
            bindModelNodes(model, model.nodes[scene.nodes[i]], out_mesh);
        }

        *mesh = out_mesh;

        return true;
    }
};

//-----------------------------------------------------------------------------
GLTFLoader::GLTFLoader()
: inner(new GLTFLoaderPrivate)
{

}


//-----------------------------------------------------------------------------
GLTFLoader::~GLTFLoader()
{

}


//-----------------------------------------------------------------------------
bool GLTFLoader::importMesh(const String& uri, OUT Mesh** mesh)
{
    return this->inner->importMesh(uri, mesh);
}

}