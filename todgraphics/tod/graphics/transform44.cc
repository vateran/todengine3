#include "tod/graphics/transform44.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
Transform44::Transform44()
: dirty(false)
{
    this->identity();
}


//-----------------------------------------------------------------------------
void Transform44::clear()
{
    this->m.clear();
}


//-----------------------------------------------------------------------------
void Transform44::identity()
{
    this->m.identity();
    this->t = { 0, 0, 0 };
    this->r = { 0, 0, 0 };
    this->s = { 1, 1, 1 };
    this->dirty = true;
}


//-----------------------------------------------------------------------------
void Transform44::setX(float value)
{
    this->t.x = value;
    this->dirty = true;
}


//-----------------------------------------------------------------------------
float Transform44::getX() const
{
    return this->t.x;
}


//-----------------------------------------------------------------------------
void Transform44::setY(float value)
{
    this->t.y = value;
    this->dirty = true;
}


//-----------------------------------------------------------------------------
float Transform44::getY() const
{
    return this->t.y;
}


//-----------------------------------------------------------------------------
void Transform44::setZ(float value)
{
    this->t.z = value;
    this->dirty = true;
}


//-----------------------------------------------------------------------------
float Transform44::getZ() const
{
    return this->t.z;
}


//-----------------------------------------------------------------------------
void Transform44::setTranslation(const Vector3& value)
{
    this->t = value;
    this->dirty = true;
}


//-----------------------------------------------------------------------------
const Vector3& Transform44::getTranslation() const
{
    return this->t;
}


//-----------------------------------------------------------------------------
void Transform44::setRotation(const Vector3& value)
{
    this->r = value;
    this->dirty = true;
}


//-----------------------------------------------------------------------------
const Vector3& Transform44::getRotation() const
{
    return this->r;
}


//-----------------------------------------------------------------------------
void Transform44::setScaling(const Vector3& value)
{
    this->s = value;
    this->dirty = true;
}


//-----------------------------------------------------------------------------
const Vector3& Transform44::getScaling() const
{
    return this->s;
}


//-----------------------------------------------------------------------------
const Matrix44& Transform44::getMatrix()
{
    if (this->dirty)
    {
        this->dirty = false;
        this->m.identity();
        this->m.scale(this->s);
        this->m.eulerRotate(
            Vector3(
                Math::deg2rad(this->r.x),
                Math::deg2rad(this->r.y),
                Math::deg2rad(this->r.z)));
        this->m.translate(this->t);
    }
    return this->m;
}


//-----------------------------------------------------------------------------
void Transform44::moveForward(float speed)
{
    auto forward = this->m.getForwardVector();
    forward.normalize();
    forward *= speed;
    this->t += forward;
    this->dirty = true;
}


//-----------------------------------------------------------------------------
void Transform44::moveSideward(float speed)
{
    auto right = this->m.getRightVector();
    right.normalize();
    right *= speed;
    this->t += right;
    this->dirty = true;
}


//-----------------------------------------------------------------------------
void Transform44::moveUpward(float speed)
{
    auto up = this->m.getUpVector();
    up.normalize();
    up *= speed;
    this->t += up;
    this->dirty = true;
}

}
