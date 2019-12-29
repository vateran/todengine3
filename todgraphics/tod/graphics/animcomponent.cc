#include "tod/graphics/animcomponent.h"
#include "tod/node.h"
#include "tod/timemgr.h"
#include "tod/graphics/shadercomponent.h"
#include "tod/graphics/shader.h"
#include "tod/graphics/bone.h"
#include "tod/graphics/boneanimator.h"
#include "tod/graphics/skeleton.h"
#include "tod/graphics/constantbuffer.h"
#include "tod/graphics/renderer.h"
#include "tod/graphics/renderinterface.h"
#include "tod/graphics/meshloader.h"
#include "tod/graphics/todfileconverter.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
AnimComponent::AnimComponent()
: boneAnimator(nullptr)
, skeleton(nullptr)
{
}


//-----------------------------------------------------------------------------
AnimComponent::~AnimComponent()
{
}


//-----------------------------------------------------------------------------
void AnimComponent::update()
{
    if ((nullptr == this->boneAnimator) || (nullptr == this->skeleton))
    {
        return;
    }

    ShaderComponent* shader_component
        = this->getNode()->findComponent<ShaderComponent>();
    if (nullptr == shader_component)
    {
        return;
    }

    Shader* shader = shader_component->getShader();
    if (nullptr == shader)
    {
        return;
    }


    struct BoneTransformCBuffer
    {
        tod::graphics::Matrix44 boneTransform[256];
    };
    static tod::graphics::ConstantBuffer* boneTransformCB = nullptr;
    if (nullptr == boneTransformCB)
    {
        boneTransformCB = Renderer::instance()->getRenderInterface()->createConstantBuffer("BoneTransformCBuffer");
        boneTransformCB->create(sizeof(BoneTransformCBuffer));
        this->boneAnimator->play(this->animName);
    }

    {
        //여기서는 Constant Buffer 를 사용하지만 최종적으로는
        //this->skeleton 끼리 모아서 VTF(Vertex Texture Fetch) 로 만들꺼다
        //Texture 에서 직접 가져온 ptr 에를 this->skeleton 에서 가지고 있을테니까
        //그때는 사실 memcpy 도 필요없고 Texture UpdateSubResource 만(GPU로 전송)
        //만 해주면 끝날것이다. (boneTransformCB->commit() 대체될듯)

        
        //BoneAnimator 가 this->skeleton 의 FinalBoneTransforms 을 업데이트 시킨다.
        this->boneAnimator->update(static_cast<float>(tod::TimeMgr::instance()->now()), this->skeleton);

        //ConstantBuffer 업데이트
        BoneTransformCBuffer* ptr = nullptr;
        boneTransformCB->lock<BoneTransformCBuffer>(&ptr);
        memcpy(
            ptr
            , &this->skeleton->getFinalBoneTransforms()[0]
            , sizeof(tod::graphics::Matrix44) * this->skeleton->getFinalBoneTransforms().size());
        boneTransformCB->unlock();
        shader->setParam("BoneTransformCBuffer", boneTransformCB);
    }
}


//-----------------------------------------------------------------------------
void AnimComponent::setAnimUri(const String& uri)
{
    this->animUri = uri;
    if (this->animUri.empty()) return;

    BoneAnimator* loaded_bone_animator = nullptr;
    if (!AnimLoader::instance()->load(uri, &loaded_bone_animator))
    {
        TOD_RETURN_TRACE();
    }

    this->boneAnimator = loaded_bone_animator;
}


//-----------------------------------------------------------------------------
const String& AnimComponent::getAnimUri()
{
    return this->animUri;
}


//-----------------------------------------------------------------------------
void AnimComponent::setSkeletonUri(const String& uri)
{
    this->skeletonUri = uri;

    TodFileConverter t;
    t.importSkeleton(this->skeletonUri, &this->skeleton);


    //t.importSkeleton("resources/mesh/character/PC_TP_Female_Red02_Lobby.todskel", &this->skeleton);
    //t.importSkeleton("resources/mesh/character/castanicgirl.todskel", &this->skeleton);    
    //t.importSkeleton("resources/mesh/character/W_Daisy_Red.todskel", &this->skeleton);
    std::map<String, String> m;

    //m["Bip001_Pelvis001"] = "Bip01_Pelvis";
    /*m["Bip001_Spine001"] = "Bip01_Spine";
    m["Bip001_Spine002"] = "Bip01_Spine1";
    m["Bip001_Spine003"] = "Bip01_Spine2";

    m["Bip001_Neck001"] = "Bip01_Neck";
    m["Bip001_Head001"] = "Bip01_Head";

    m["Bip001_L_Clavicle001"] = "Bip01_L_Clavicle";
    m["Bip001_L_UpperArm001"] = "Bip01_L_UpperArm";
    m["Bip001_L_Forearm001"] = "Bip01_L_Forearm";
    m["Bip001_L_Hand001"] = "Bip01_L_Hand";


    m["Bip001_R_Clavicle001"] = "Bip01_R_Clavicle";
    m["Bip001_R_UpperArm001"] = "Bip01_R_UpperArm";
    m["Bip001_R_Forearm001"] = "Bip01_R_Forearm";
    m["Bip001_R_Hand001"] = "Bip01_R_Hand";


    m["Bip001_L_Thigh001"] = "Bip01_L_Thigh";
    m["Bip001_L_Calf001"] = "Bip01_L_Calf";
    m["Bip001_L_Foot001"] = "Bip01_L_Foot";
    m["Bip001_L_Toe001"] = "Bip01_L_Toe0";
    m["Bip001_R_Thigh001"] = "Bip01_R_Thigh";
    m["Bip001_R_Calf001"] = "Bip01_R_Calf";
    m["Bip001_R_Foot001"] = "Bip01_R_Foot";
    m["Bip001_R_Toe001"] = "Bip01_R_Toe0";*/

    /*m["Bip001_R_Finger001"] = "Bip01_R_Finger0";
    m["Bip001_R_Finger004"] = "Bip01_R_Finger01";
    m["Bip001_R_Finger007"] = "Bip01_R_Finger02";
    m["Bip001_R_Finger010"] = "Bip01_R_Finger1";
    m["Bip001_R_Finger013"] = "Bip01_R_Finger11";
    m["Bip001_R_Finger014"] = "Bip01_R_Finger12";*/

    /*m["Bip001_L_Finger001"] = "Bip01_L_Finger0";
    m["Bip001_L_Finger004"] = "Bip01_L_Finger01";
    m["Bip001_L_Finger007"] = "Bip01_L_Finger02";
    m["Bip001_L_Finger010"] = "Bip01_L_Finger1";
    m["Bip001_L_Finger013"] = "Bip01_L_Finger11";
    m["Bip001_L_Finger014"] = "Bip01_L_Finger12";*/

    //this->skeleton->retargeting(this->boneAnimator, m);
}


//-----------------------------------------------------------------------------
const String& AnimComponent::getSkeletonUri()
{
    return this->skeletonUri;
}


//-----------------------------------------------------------------------------
void AnimComponent::setAnimName(const String& name)
{
    this->animName = name;
}


//-----------------------------------------------------------------------------
const String& AnimComponent::getAnimName()
{
    return this->animName;
}


//-----------------------------------------------------------------------------
void AnimComponent::bindProperty()
{
    BIND_PROPERTY(const String&, "animUri", "", setAnimUri, getAnimUri, "", PropertyAttr::DEFAULT);
    BIND_PROPERTY(const String&, "skeletonUri", "", setSkeletonUri, getSkeletonUri, "", PropertyAttr::DEFAULT);
    BIND_PROPERTY(const String&, "animName", "", setAnimName, getAnimName, "", PropertyAttr::DEFAULT);
}

}