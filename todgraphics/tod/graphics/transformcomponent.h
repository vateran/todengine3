#pragma once
#include "tod/component.h"
#include "tod/graphics/matrix44.h"
#include "tod/graphics/transform44.h"
namespace tod::graphics
{

class TransformComponent : public Derive<TransformComponent, Component>
{
public:
    enum
    {
        TRANSFORM_DIRTY,
        
        FLAG_MAX,
    };

public:
    TransformComponent()
    {
        this->flags[TRANSFORM_DIRTY] = true;
    }

    void reset()
    {
        this->flags[TRANSFORM_DIRTY] = true;
        this->localTransform.identity();
    }

    bool updateWorldTransform(
          const Matrix44& parent_transform
        , bool parent_transform_dirty)
    {
        if ((false == this->flags[TRANSFORM_DIRTY])
         && (false == parent_transform_dirty))
        {
            return false;
        }

        this->worldTransform = this->localTransform.getMatrix();
        this->worldTransform *= parent_transform;
        this->flags[TRANSFORM_DIRTY] = false;

        return true;
    }
    inline void setTranslation(const Vector3& t)
    {
        this->localTransform.setTranslation(t);
        this->flags[TRANSFORM_DIRTY] = true;
    }
    inline bool isTransformDirty() const { return this->flags[TRANSFORM_DIRTY]; }
    inline Transform44* getLocalTransform() { return &this->localTransform; }
    inline const Matrix44& getLocalTransformMatrix()
    { return this->localTransform.getMatrix(); }
    //TODO : 아직 이번 프레임이 렌더링 되지 않았을 경우 worldTransform 이 업데이트 안된 경우가 있다
    //아직 렌더링 되지 않았을때도 올바른 WorldTransform을 얻도록 해야함
    inline const Matrix44& getWorldTransformMatrix() const
    { return this->worldTransform; }
    
    inline void moveForward(float speed)
    {
        this->localTransform.moveForward(speed);
        this->flags[TRANSFORM_DIRTY] = true;
    }
    
    inline void moveSideward(float speed)
    {
        this->localTransform.moveSideward(speed);
        this->flags[TRANSFORM_DIRTY] = true;
    }
    
    inline void moveUpward(float speed)
    {
        this->localTransform.moveUpward(speed);
        this->flags[TRANSFORM_DIRTY] = true;
    }
    
    inline void setX(float value)
    {
        this->localTransform.setX(value);
        this->flags[TRANSFORM_DIRTY] = true;
    }
    inline float getX()
    {
        return this->localTransform.getX();
    }
    inline void setY(float value)
    {
        this->localTransform.setY(value);
        this->flags[TRANSFORM_DIRTY] = true;
    }
    inline float getY()
    {
        return this->localTransform.getY();
    }
    inline void setZ(float value)
    {
        this->localTransform.setZ(value);
        this->flags[TRANSFORM_DIRTY] = true;
    }
    inline float getZ()
    {
        return this->localTransform.getZ();
    }
    inline void setPosition(const Vector3& value)
    {
        this->localTransform.setTranslation(value);
        this->flags[TRANSFORM_DIRTY] = true;
    }
    inline const Vector3& getPosition()
    {
        return this->localTransform.getTranslation();
    }
    inline void setScaling(const Vector3& value)
    {
        this->localTransform.setScaling(value);
        this->flags[TRANSFORM_DIRTY] = true;
    }
    inline const Vector3& getScaling()
    {
        return this->localTransform.getScaling();
    }
    inline void setRotation(const Vector3& value)
    {
        this->localTransform.setRotation(value);
        this->flags[TRANSFORM_DIRTY] = true;
    }
    inline const Vector3& getRotation()
    {
        return this->localTransform.getRotation();
    }
    
    static void bindProperty();
    
private:
    Transform44 localTransform;
    Matrix44 worldTransform;
    std::bitset<FLAG_MAX> flags;
};

}
