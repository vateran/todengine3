#pragma once
#include "tod/graphics/matrix44.h"
namespace tod::graphics
{

class Transform44
{
public:
    Transform44();
    
    void clear();
    void identity();
    void setX(float value);
    float getX() const;
    void setY(float value);
    float getY() const;
    void setZ(float value);
    float getZ() const;
    void setTranslation(const Vector3& value);
    const Vector3& getTranslation() const;
    void setRotation(const Vector3& value);
    const Vector3& getRotation() const;
    void setScaling(const Vector3& value);
    const Vector3& getScaling() const;
    const Matrix44& getMatrix();
    void moveForward(float speed);
    void moveSideward(float speed);
    void moveUpward(float speed);

private:
    bool dirty;
    Matrix44 m;
    Vector3 t;
    Vector3 s;
    Vector3 r;
};

}
