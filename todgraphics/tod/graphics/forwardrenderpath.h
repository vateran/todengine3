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

                //�ǳ����� �ö���鼭 update Bounding Volume
                //�� BoundingVolume ������Ʈ�� Node �ȿ� �ִ� ��� Bounding Volume �� ���ε��� ����
                //���߿� Hierarchical Frustum Culling �Ҷ� ���
            }
        }

        void render_cameras(Viewport* viewport)
        {
            //���⼭ CommandBuffer �� ���ڵ� ����


            //Update per Frame Constant Buffer



            //render CameraNodes
            for (auto& camera_node : this->cameraNodes)
            {
                if (nullptr == camera_node.shader) continue;

                camera_node.camera->setWidth(static_cast<float>(viewport->getWidth()));
                camera_node.camera->setHeight(static_cast<float>(viewport->getHeight()));

                //camera �� frustum �� �������� RenderNode, LightNode �鿡 ���� frustum culling ����
                Frustum frustum;
                Matrix44 viewProjMatrix = camera_node.transform->getWorldTransformMatrix();
                viewProjMatrix.inverse();
                viewProjMatrix *= camera_node.camera->getProjectionMatrix();
                frustum.makeFrustum(viewProjMatrix);

                //Frustum Culling �� RenderNode vector �� output ��
                for (auto& scene_node : this->sceneNodes)
                {
                    auto aabb = scene_node.render->getAABB();
                    aabb.transform(scene_node.transform->getWorldTransformMatrix());
                    Frustum::Cull result;
                    frustum.checkCullSphere(aabb, result);
                    scene_node.culled = (result == Frustum::CULL_OUTSIDE);
                }


                //camera ���� look vector �������� ����� RenderNode(Opaque�鸸), LightNode �� ����


                uint32 passes = 0;
                camera_node.shader->begin(passes);


                //Light �� ���⼭ ����
                for (auto& light_node : this->lightNodes)
                {
                }


                //Update per Camera Constant Buffer


                //���⼭ RenderTarget �� �����ؾ� �ҵ�?


                //���⼭ Hierarchical Frustum Culling, Hiz Occlusion Culling ����


                //Light �� ���⼭ Culling �� ������ ����


                //RenderNode ���� camera ���� �Ÿ������� sort


                //opaque ���� early-z ���� �Ͽ� ��->�� rendering


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


                //���� ��ü�� ��->�� rendering
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



            //���⼭ CommandBuffer ���ڵ� ������,
            //CommandBuffer 1/2 swap ��
            //thread pool �� CommandBuffer ��� �ϰ�
            //��ٷ� render_cameras �Լ� ��
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
            //���߿� �ܼ��� clear �������� buffer �� �����ϴ� ����� ������ ��
            this->cameraNodes.clear();
            this->lightNodes.clear();
            this->sceneNodes.clear();
            this->behaviors.clear();

            //SceneNode ���� ������ 
            Matrix44 rootTransform;
            rootTransform.identity();
            this->gather(scene_node, rootTransform, false);

            //��� behavior ���� ������Ʈ
            for (auto& behavior : this->behaviors)
            {
                behavior->update();
            }

            //Camera �� �������� ������ ����
            this->render_cameras(viewport);
        }
    };
}