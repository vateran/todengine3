#pragma once
namespace tod::graphics
{

enum class PixelFormat : char
{
    NONE,
    L8F,
    R8G8B8,
    R8G8B8A8,
    R16G16B16F,
    R16G16B16A16F,
    DEPTH16,
    DEPTH32F,
    
    MAX
};

enum class TextureWrap : char
{
    REPEAT,
    MIRRORED_REPEAT,
    CLAMP_TO_EDGE,
    CLAMP_TO_BORDER,
};
enum class TextureFilter : char
{
    NEAREST,
    LINEAR,
};

}
