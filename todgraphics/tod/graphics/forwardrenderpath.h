#pragma once
#include "tod/node.h"
#include "tod/timemgr.h"
#include "tod/graphics/renderinterface.h"
#include "tod/graphics/cameracomponent.h"
#include "tod/graphics/transformcomponent.h"
#include "tod/graphics/shadercomponent.h"
#include "tod/graphics/rendercomponent.h"
#include "tod/graphics/behaviorcomponent.h"
#include "tod/graphics/lightcomponent.h"
#include "tod/graphics/constantbuffer.h"
#include "tod/graphics/shader.h"
#include "tod/graphics/frustum.h"
namespace tod::graphics
{
    struct TodBaseFrameCBuffer
    {
        tod::graphics::Matrix44 matWVP;
        tod::graphics::Matrix44 matWorld;
        tod::graphics::Matrix44 matInvWorld;
        tod::graphics::Matrix44 matView;
        tod::graphics::Matrix44 matInvView;
        tod::graphics::Matrix44 matProj;
        tod::graphics::Vector3 vecViewPos;        
        float fTime;
        uint32 iLightCount;
        float padding[3];
    };


    struct TodBaseLight
    {
        Light lights[8];
    };

    class RenderPath
    {
    public:
        RenderPath()
        : renderInterface(nullptr)
        {
        }

        bool create(RenderInterface* render_interface)
        {
            this->renderInterface = render_interface;

            this->todBaseFrameCBuffer = render_interface->createConstantBuffer("TodBaseFrameCBuffer");
            this->todBaseFrameCBuffer->create(sizeof(TodBaseFrameCBuffer));

            this->todBaseLightCBuffer = render_interface->createConstantBuffer("TodBaseFrameCBuffer");
            this->todBaseLightCBuffer->create(sizeof(TodBaseFrameCBuffer));

            return true;
        }

        void destroy()
        {

        }

        virtual void render(Node* scene_node, Viewport* viewport) = 0;

    protected:
        void gather(Node* node, const Matrix44& parent_transform, bool parent_transform_dirty)
        {
            if (false == node->isVisible())
            {
                return;
            }

            Matrix44 world_transform{ parent_transform };
            SceneNode* scene_node = nullptr;

            auto transform_compo = node->findComponent<TransformComponent>();
            if (nullptr != transform_compo)
            {
                if (node->hasComponent<CameraComponent>())
                {
                    this->cameraNodes.push_back(CameraNode());
                    CameraNode* camera_node = &this->cameraNodes.back();
                    scene_node = camera_node;
                    camera_node->camera = node->findComponent<CameraComponent>();
                }
                else if (node->hasComponent<LightComponent>())
                {
                    this->lightNodes.push_back(LightNode());
                    LightNode* light_node = &this->lightNodes.back();
                    scene_node = light_node;
                    light_node->light = node->findComponent<LightComponent>();
                }
                else if (node->hasComponent<RenderComponent>())
                {   
                    this->sceneNodes.push_back(SceneNode());
                    scene_node = &this->sceneNodes.back();
                    scene_node->render = node->findComponent<RenderComponent>();
                }

                if (nullptr != scene_node)
                {
                    scene_node->transform = transform_compo;
                    scene_node->shader = node->findComponent<ShaderComponent>();
                    for (auto compo : node->getComponents())
                    {
                        if (false == compo->getType()->isKindOf<BehaviorComponent>())
                        {
                            continue;
                        }
                        this->behaviors.push_back(compo);
                    }
                }
               
                parent_transform_dirty
                    = transform_compo->updateWorldTransform(parent_transform, parent_transform_dirty)
                   || parent_transform_dirty;
                world_transform = transform_compo->getWorldTransformMatrix();
            }

            for (auto& child : node->getChildren())
            {
                this->gather(child, world_transform, parent_transform_dirty);

                //맨끝에서 올라오면서 update Bounding Volume
                //이 BoundingVolume 업데이트는 Node 안에 있는 모든 Bounding Volume 을 감싸도록 만들어서
                //나중에 Hierarchical Frustum Culling 할때 사용
            }
        }

        void render_cameras(Viewport* viewport)
        {
            //여기서 CommandBuffer 에 레코딩 시작


            //Update per Frame Constant Buffer



            //render CameraNodes
            for (auto& camera_node : this->cameraNodes)
            {
                if (nullptr == camera_node.shader) continue;

                camera_node.camera->setWidth(static_cast<float>(viewport->getWidth()));
                camera_node.camera->setHeight(static_cast<float>(viewport->getHeight()));

                //camera 의 frustum 을 기준으로 RenderNode, LightNode 들에 대해 frustum culling 수행
                Frustum frustum;
                Matrix44 viewProjMatrix = camera_node.transform->getWorldTransformMatrix();
                viewProjMatrix.inverse();
                viewProjMatrix *= camera_node.camera->getProjectionMatrix();
                frustum.makeFrustum(viewProjMatrix);

                //Frustum Culling 된 RenderNode vector 가 output 됨
                for (auto& scene_node : this->sceneNodes)
                {
                    auto aabb = scene_node.render->getAABB();
                    aabb.transform(scene_node.transform->getWorldTransformMatrix());
                    Frustum::Cull result;
                    frustum.checkCullSphere(aabb, result);
                    scene_node.culled = (result == Frustum::CULL_OUTSIDE);
                }


                //camera 기준 look vector 기준으로 가까운 RenderNode(Opaque들만), LightNode 를 정렬


                uint32 passes = 0;
                camera_node.shader->begin(passes);


                //Light 도 여기서 설정
                for (auto& light_node : this->lightNodes)
                {
                }


                //Update per Camera Constant Buffer


                //여기서 RenderTarget 을 셋팅해야 할듯?


                //여기서 Hierarchical Frustum Culling, Hiz Occlusion Culling 수행


                //Light 도 여기서 Culling 에 영향을 받음


                //RenderNode 들을 camera 와의 거리순으로 sort


                //opaque 먼저 early-z 적용 하여 앞->뒤 rendering


                //render Opaque RenderNodes
                int render_count = 0;
                for (auto& scene_node : this->sceneNodes)
                {
                    if (true == scene_node.culled)
                    {
                    //    continue;
                    }
                    
                    scene_node.shader->begin(passes);

                    //Update per Object Constant Buffer
                    TodBaseFrameCBuffer* ptr = nullptr;
                    this->todBaseFrameCBuffer->lock<TodBaseFrameCBuffer>(&ptr);
                    ptr->fTime = static_cast<float>(tod::TimeMgr::instance()->now());
                    ptr->matWorld = scene_node.transform->getWorldTransformMatrix();
                    ptr->matWVP = ptr->matWorld;
                    ptr->matWVP *= viewProjMatrix;
                    ptr->matWVP.transpose();
                    ptr->matWorld.transpose();
                    ptr->matWorld.inverseTo(ptr->matInvWorld);
                    Matrix44 view, proj;
                    view = camera_node.transform->getWorldTransformMatrix();
                    view.inverse();
                    view.transpose();
                    ptr->matView = view;
                    view.inverseTo(ptr->matInvView);
                    ptr->matProj = camera_node.camera->getProjectionMatrix();
                    ptr->matProj.transpose();
                    ptr->vecViewPos = camera_node.transform->getWorldTransformMatrix().getTranslation();
                    this->todBaseFrameCBuffer->unlock();
                    scene_node.shader->setParam("TodBaseFrameCBuffer", this->todBaseFrameCBuffer);

                    if (nullptr != scene_node.render)
                    {
                        scene_node.render->render(scene_node.shader->getShader(), passes);
                    }

                    scene_node.shader->end();




                    /*{
                        auto prevTechnique = render_node.shader->getTechnique();
                        render_node.shader->setTechnique("Outline");
                        render_node.shader->begin(passes);
                        for (auto& render : render_node.renders)
                        {
                            if (nullptr == render) continue;

                            render->render(shader, passes);
                        }
                        render_node.shader->end();
                        render_node.shader->setTechnique(prevTechnique);
                    }*/








                    






                    ++render_count;
                }

                static float elapsed = 0;
                static int fps = 0;
                elapsed += TimeMgr::instance()->delta();
                ++fps;
                if (1 <= elapsed)
                {   
                    TOD_LOG("INFO", "FPS[%d] RenderCount[%d]", fps, render_count);
                    elapsed -= 1;
                    fps = 0;
                }


                //투명 객체들 뒤->앞 rendering
                //render Transpaent RenderNodes
                /*for (auto& render_node : this->renderNodes)
                {
                    //Update per Object Constant Buffer

                    uint32 passes = 0;
                    render_node.shader->begin(passes);

                    //render_node.transform->getWorldTransformMatrix(); -> ConstantBuffer

                    for (uint32 i = 0; i < passes; ++i)
                    {
                        render_node.render->render(render_node.shader->getShader());
                    }

                    render_node.shader->end();
                }*/


                camera_node.shader->end();
            }



            //여기서 CommandBuffer 레코딩 끝내고,
            //CommandBuffer 1/2 swap 후
            //thread pool 에 CommandBuffer 재생 하고
            //곧바로 render_cameras 함수 끝
        }

    protected:
        struct SceneNode
        {
            SceneNode():culled(false) {}

            bool culled;
            TransformComponent* transform;
            ShaderComponent* shader;
            RenderComponent* render;
        };
        typedef std::vector<SceneNode> SceneNodes;
        SceneNodes sceneNodes;

        //CameraNode list
        struct CameraNode : public SceneNode
        {
            CameraComponent* camera;
        };
        typedef std::list<CameraNode> CameraNodes;
        CameraNodes cameraNodes;

        //LightNode list
        struct LightNode : public SceneNode
        {
            LightComponent* light;
        };
        typedef std::list<LightNode> LightNodes;
        LightNodes lightNodes;

        std::vector<BehaviorComponent*> behaviors;

        RenderInterface* renderInterface;
        ConstantBuffer* todBaseFrameCBuffer;
        ConstantBuffer* todBaseLightCBuffer;
    };


    class ForwardRenderPath : public RenderPath
    {
    public:
        void render(Node* scene_node, Viewport* viewport) override
        {
            //나중에 단순히 clear 하지만고 buffer 를 유지하는 방법을 적용할 것
            this->cameraNodes.clear();
            this->lightNodes.clear();
            this->sceneNodes.clear();
            this->behaviors.clear();

            //SceneNode 들을 모은다 
            Matrix44 rootTransform;
            rootTransform.identity();
            this->gather(scene_node, rootTransform, false);

            //모든 behavior 들을 업데이트
            for (auto& behavior : this->behaviors)
            {
                behavior->update();
            }

            //Camera 를 기준으로 렌더링 시작
            this->render_cameras(viewport);
        }
    };
}