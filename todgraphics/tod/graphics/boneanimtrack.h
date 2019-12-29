#pragma once
#include <map>
#include "tod/graphics/matrix44.h"
namespace tod::graphics
{

/**
    @class BoneAnimTrack
    @brief Bone 1���� Animation �����͸� ����
    @ref Skeletal Animation ���� �������� Bone �� ����Ǿ� �ִµ�,
    BoneAnimTrack �� �� Bone 1���� Animation �����͸� �����ϰ� �ִ�.
    @see BoneAnim
*/
class BoneAnimTrack
{
public:
    typedef std::map<float, Vector3> Keys;
    typedef std::map<float, Quaternion> QuatKeys;

public:
    void update(float elapsed);

    Matrix44& getLocalBoneTransform() { return this->localBoneTransform; }

public:
    template <typename KEYS, typename OUT_VALUE>
    void find_key(float elapsed, KEYS& keys, OUT_VALUE& out);

public:
    Keys positionKeys;
    QuatKeys rotateKeys;
    Keys scaleKeys;
    Matrix44 localBoneTransform;
};


//-----------------------------------------------------------------------------
template <typename KEYS, typename OUT_VALUE>
void BoneAnimTrack::find_key(float elapsed, KEYS& keys, OUT_VALUE& out)
{
    auto cur_key_iter = keys.lower_bound(elapsed);
    if (keys.end() == cur_key_iter)
    {
        if (false == keys.empty())
        {
            auto rbegin_iter = keys.rbegin();
            out = rbegin_iter->second;
        }
        return;
    }
    if (keys.begin() != cur_key_iter)
    {
        --cur_key_iter;
    }

    auto next_key_iter = cur_key_iter;
    ++next_key_iter;
    if (keys.end() == next_key_iter)
    {
        out = cur_key_iter->second;
        return;
    }

    float duration = next_key_iter->first - cur_key_iter->first;
    float ratio = (elapsed - cur_key_iter->first) / duration;
    OUT_VALUE::interpolate(
        ratio
        , cur_key_iter->second
        , next_key_iter->second
        , out);
}

}