#define NOMINMAX
#include "tod/string.h"
#include "tod/argparser.h"
#include "tod/filesystem.h"
#include "tod/binarywriter.h"
#include "tod/graphics/mesh.h"
#include "tod/graphics/vector2.h"
#include "tod/graphics/vector3.h"
#include "tod/graphics/vector4.h"
#include "tod/graphics/inputlayout.h"
#include "tod/graphics/mesh.h"
#include "tod/graphics/vertexbuffer.h"
#include "tod/graphics/indexbuffer.h"
#include "tod/graphics/todfileconverter.h"
#include <array>
#include <unordered_map>

namespace tod::graphics
{

}



bool convertToTodMeshFile(
  tod::ArgParser& arg_parser
, const tod::FileSystem::DirName& dir_name
, const tod::FileSystem::FileName& file_name)
{
    tod::String path = dir_name + S("/") + file_name;
    path = path.normalizePath();

    tod::String only_file(file_name.extractFileName());
    tod::String output_path = dir_name + S("/") + only_file.substr(0, only_file.rfind(S("."))) + ".todmesh";
    output_path = output_path.normalizePath();

    tod::String texture_root;
    arg_parser.getOption<tod::String>("texture_root", texture_root);
    tod::graphics::TodFileConverter tod_mesh_file;
    tod_mesh_file.setTextureRootPath(texture_root);
    return tod_mesh_file.exportMesh(path, output_path);
}




int main(int argc, char* argv[])
{
    tod::ArgParser arg_parser("exe --mesh --input girlmodel/ANIME+GIRL+001.fbx");
    arg_parser.addOption<bool>("binary", "바이너리 포맷으로 export", true);
    arg_parser.addOption<bool>("recursive", "하위 모든 디렉토리를 순회하며 변환", true);
    arg_parser.addOption<tod::String>("input", "입력 파일. wildcard 문자사용가능", "");
    arg_parser.addOption<tod::String>("texture_root", "Texture 파일이 존재하는 root 폴더를 지정. ex) res/texture", "");
    arg_parser.addCommand("anim", "Animation 데이터를 추출하여 export",
        []()
        {
            return true;
        });
    arg_parser.addCommand("mesh", "Mesh 데이터를 추출하여 export",
        [&arg_parser]()
        {
            bool recursive = false;
            arg_parser.getOption<bool>("recursive", recursive);
            tod::String input_files;
            arg_parser.getOption<tod::String>("input", input_files);

            if (tod::String::npos != input_files.rfind(S("*")))
            {
                tod::FileSystem::instance()->iterateFiles(
                      input_files
                    , [&arg_parser](
                      const tod::FileSystem::DirName& dir_name
                    , const tod::FileSystem::FileName& file_name)
                    {
                        return convertToTodMeshFile(arg_parser, dir_name, file_name);
                    }, recursive);
            }
            else
            {   
                return convertToTodMeshFile(
                      arg_parser
                    , tod::FileSystem::instance()->getCurrentWorkingDirectory()
                    , input_files);
            }
            
            return true;
        });
    arg_parser.addCommand("bone", "Bone 데이터를 추출하여 export", 
        []()
        {
            return true;
        });

    if (!arg_parser.parse())
    {
        printf("%s", arg_parser.getErrorMsg().c_str());
        return 0;
    }

    if (!arg_parser.execute())
    {
        printf("%s", arg_parser.getErrorMsg().c_str());
        return 0;
    }

    return 0;
}
