#include "tod/graphics/graphicsdef.h"
#include "tod/exception.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
FormatDesc FormatDesc::repository[static_cast<uint32>(Format::MAX)] =
{
    { S("NONE"),            Format::NONE,               0   },
    { S("L8F"),             Format::L8F,                1   },
    { S("R8G8B8A8"),        Format::R8G8B8A8,           4   },
    { S("R8G8B8A8U"),       Format::R8G8B8A8U,          4   },  //unsigned int
    { S("R8G8B8A8UN"),      Format::R8G8B8A8UN,         4   },  //unsigned normalized int
    { S("R8G8B8A8UN_SRGB"), Format::R8G8B8A8UN_SRGB,    4   },  //unsigned normalized int
    { S("R16G16B16A16F"),   Format::R16G16B16A16F,      8   },
    { S("R32F"),            Format::R32F,               4   },
    { S("R32G32F"),         Format::R32G32F,            8   },
    { S("R32G32B32F"),      Format::R32G32B32F,         12  },
    { S("R32G32B32A32F"),   Format::R32G32B32A32F,      16  },
    { S("DEPTH16UN"),       Format::DEPTH16UN,          2   },
    { S("DEPTH32F"),        Format::DEPTH32F,           4   },
};


//-----------------------------------------------------------------------------
TextureTypeDesc TextureTypeDesc::repository[static_cast<uint32>(TextureType::MAX)] =
{
    { S("DIFFUSE"),         S("texDiffuse"),        TextureType::DIFFUSE      },
    { S("NORMAL"),          S("texNormal"),         TextureType::NORMAL       },
    { S("SPECULAR"),        S("texSpecular"),       TextureType::SPECULAR     },
    { S("LIGHT"),           S("texLight"),          TextureType::LIGHT        },
    { S("BUMP"),            S("texBump"),           TextureType::BUMP         },
    { S("OPACITY"),         S("texOpacity"),        TextureType::OPACITY      },
    { S("EMISSIVE"),        S("texEmissive"),       TextureType::EMISSIVE     },
    { S("AMBIENT"),         S("texAmbient"),        TextureType::AMBIENT      },
    { S("SHININESS"),       S("texShininess"),      TextureType::SHININESS    },
    { S("DISPLACEMENT"),    S("texDisplacement"),   TextureType::DISPLACEMENT },
    { S("REFLECTION"),      S("texRelfection"),     TextureType::REFLECTION   },
    { S("HEIGHT"),          S("texHeight"),         TextureType::HEIGHT,      },
};


}
