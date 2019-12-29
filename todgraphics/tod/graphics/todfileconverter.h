#pragma once
namespace tod::graphics
{

class Mesh;
class Bone;
class BoneAnimator;
class Skeleton;

class TodFileConverter
{
public:
    typedef int32 BoneNameHash;
    typedef int32 BoneIndex;
    typedef std::unordered_map<BoneNameHash, BoneIndex> BoneIndexer;
    BoneIndexer boneIndexer;
    BoneIndex boneIndexCounter;

    struct TodMeshFileHeader
    {
        int32 version;
    };

    struct TodAnimFileHeader
    {
        int32 version;
    };

    TodFileConverter();

    int32 allocBoneIndex(const String& bone_name);
    void setTextureRootPath(const String& path);

    bool exportMesh(const String& uri, const String& output_uri);
    bool importMesh(const String& uri, Mesh** mesh_out);
    bool exportAnim(const String& uri, const String& output_uri);
    bool importAnim(const String& uri, BoneAnimator** anim_out);
    bool exportSkeleton(const String& uri, const String& output_uri, const String& root_name);
    bool importSkeleton(const String& uri, Skeleton** skeleton_out);

private:
    //Texture 를 찾을 폴더
    String textureRootPath;
    struct Material
    {
        String texDiffuse;
        String texSpecular;
        String texNormal;
    };
    typedef std::vector<Material> materials;
};

}