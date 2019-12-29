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
    arg_parser.addOption<bool>("binary", "���̳ʸ� �������� export", true);
    arg_parser.addOption<bool>("recursive", "���� ��� ���丮�� ��ȸ�ϸ� ��ȯ", true);
    arg_parser.addOption<tod::String>("input", "�Է� ����. wildcard ���ڻ�밡��", "");
    arg_parser.addOption<tod::String>("texture_root", "Texture ������ �����ϴ� root ������ ����. ex) res/texture", "");
    arg_parser.addCommand("anim", "Animation �����͸� �����Ͽ� export",
        []()
        {
            return true;
        });
    arg_parser.addCommand("mesh", "Mesh �����͸� �����Ͽ� export",
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
    arg_parser.addCommand("bone", "Bone �����͸� �����Ͽ� export", 
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
