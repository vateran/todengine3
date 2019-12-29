#include <stdio.h>
#include <cassert>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <unordered_map>
#include <set>
#include <atomic>
#include <limits>
#include <curl/curl.h>
#include "tod/promise.h"
#include "tod/promise.inl"
#include "tod/http.h"
#include "tod/kernel.h"
#include "tod/uuid.h"
#include "tod/type.h"
#include "tod/property.h"
#include "tod/method.h"
#include "tod/object.h"
#include "tod/any.h"
#include "tod/eventdispatcher.h"
#include "tod/pool.h"
#include "tod/filesystem.h"
#include "tod/serializer.h"
#include "tod/timemgr.h"
#include "tod/random.h"
#include "tod/interpolation.h"
#include "tod/argparser.h"
#include "tod/staticstring.h"
#include "tod/binarystreamwriter.h"
#include "tod/log.h"
#include "tod/graphics/compactfloat.h"

using namespace tod;




/*
 1. 가비지 콜렉션 같은 개념이 있어서 해제 안된 객체를 추적 할 수 있는 기능, ref count 가 없는 객체는 가비지 콜렉션이 가능
 -> 순환 참조도 자동으로 해결
 2. 루아/파이썬을 공용으로 사용 가능한 기반 기능. 모노도 연결해 볼까?
 3. C++17 적극 사용
 4. 예외 적극 사용. 에러를 리더블하게 보여줌
 5. AOP 를 넣을 수 있는 지 알아보기. C++17 의 operator . 의 가능성을 알아보자
 6. 병렬처리가 가능한지 알아보자
 7. 객체간 효율적인 알림 처리
 8. Component 기반 객체 조합
 9. 루아/파이썬을 사용한 툴 확장 가능 -> 노출하는 라이브러리를 모듈별로 정리하자.  C#처럼 잘 정리하자
 10. Prefab 기능을 구현
 11. 데이터 로더를 구현 및 확장 가능하도록 -> 이것들을 레이어에 맞춰서 잘 나눠보자 -> 사실상 툴 라이브러리를 C++말고 구성 가능하도록
 12. 기존보다 더 풍부한 reflection 아키텍쳐 적용
 - 객체 도움말
 - Property, Method 도움말
 - 제약조건 설정
 13. bson 같은걸로 serialization
 14. 툴을 기반으로 시작하는 것으로 만들자. 실행할때 컴파일 안하게끔
 15. Asset Pipeline 정리
 - JSON 기반 Serialize
 - BSON 기반 Serialize
 - BSON + zlib 아카이브로 어셋 엑세스 가능하도록
 - 각 Asset 을 플랫폼별로 최적화 할 수 있도록 중간 처리 할 수 있는 방법을 제공해야 함
 - Pipeline 의 stage 별로 커스텀화 할 수 있게 <- 이게 Script layer 에서 가능해야 함
 16. std::any 에 data pool 을 적용할 필요성이 있음
 */


//-----------------------------------------------------------------------------
void test_Interpolation()
{
    for (float i=0.0f;i<=1.01f;i+=0.1f)
    {
        printf("%f -> %f\n", i, Interpolation::linear(i, 23.0f, 50.223f));
    }
}


//-----------------------------------------------------------------------------
void test_Random()
{
    Random r;
    for (int i=0;i<100;++i)
    {
        printf("%f\n", r.uniformFloat(-1, 1));
    }
    for (int i=0;i<1000;++i)
    {
        auto ret = r.uniformInt(-10, 10);
        assert(-10 <= ret);
        assert(ret <= 10);
        printf("%d\n", ret);
    }
}


//-----------------------------------------------------------------------------
void test_TimeMgr()
{
    for (int i=0;i<10;++i)
    {
        TimeMgr::instance()->update();
        TimeMgr::instance()->now();
        printf("%f %f\n", TimeMgr::instance()->now(), TimeMgr::instance()->delta());
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}


//-----------------------------------------------------------------------------
void test_NodeRelease()
{
    Node* node1 = new Node();
    node1->setName("node1");
    Node* node2 = new Node();
    node2->setName("node2");
    
    ObjRef<Node> node_ref2(node2);
    
    node1->addChild(node2);
    node1->release();
    
    Node* node3 = new Node();
    Node* node4 = new Node();
    node3->addChild(node4);
    node4->removeFromParent();
    node3->release();
    
    Node* node5 = new Node();
    Node* node6 = new Node();
    node5->addChild(node6);
    node6->release();
    node5->release();
}


//-----------------------------------------------------------------------------
void test_Utilites()
{
    Uuid u1, u2;
    
    //string 으로 초기화
    u1 = "084cb2b0-bcad-4932-9d71-28fb6e03ea27";
    u2 = "084cb2b0-bcad-4932-9d71-28fb6e03ea27";
    
    
    //기본 비교
    assert(u1 == u2);
    assert(u1 == "084cb2b0-bcad-4932-9d71-28fb6e03ea27");
    
    
    //랜덤 생성
    u1.generate();
    assert(u1 != u2);
    printf("generated uuid [%s]\n", u1.toString().c_str());
    
    
    //string split
    String split_test("/usr/scene/test");
    std::vector<String> splited_strings;
    split_test.split("/", splited_strings);
    assert(splited_strings.size() == 3);
    assert(splited_strings[0] == "usr");
    assert(splited_strings[1] == "scene");
    assert(splited_strings[2] == "test");
    
    split_test = "/usr//test";
    splited_strings.clear();
    split_test.split("/", splited_strings);
    assert(splited_strings.size() == 2);
    assert(splited_strings[0] == "usr");
    assert(splited_strings[1] == "test");
    
    
    //빈 token 을 drop 안함
    split_test = "/usr//test";
    splited_strings.clear();
    split_test.split("/", splited_strings, false);
                                         //~~~~~
    assert(splited_strings.size() == 3);
    assert(splited_strings[0] == "usr");
    assert(splited_strings[1] == "");
    assert(splited_strings[2] == "test");

    //빈 token 을 drop함
    split_test = "/usr//test";
    splited_strings.clear();
    split_test.split("/", splited_strings, true);
                                        //~~~~~
    assert(splited_strings.size() == 2);
    assert(splited_strings[0] == "usr");
    assert(splited_strings[1] == "test");

    
    String trim_test = "   trim test   \t ";
    trim_test.trim();
    assert(trim_test == "trim test");
    
    
    assert(String::fromFormat("%s %d", "asd", 10) == "asd 10");
}


//-----------------------------------------------------------------------------
void test_Pool()
{
    //Params 는 new 를 사용해서 새로운 객체 생성시, 자체 Pool 에서 할당 받음
    auto p = new Params;
    p->push_back(10);
    delete p;
    //delete 된 직후, 바로 new 하면 전에 있던 객체가 반환됨
    
    auto p2 = new Params;
    assert(p == p2);
    assert(p2->empty());
    
    auto p3 = Params::newParam(123, String("test"), false);
    
    delete p2;
    delete p3;
}


//-----------------------------------------------------------------------------
void test_any()
{
    std::any any_value(1);
    assert(std::any_cast<int>(any_value) == 1);
    any_value = true;
    assert(std::any_cast<bool>(any_value) == true);
    any_value = 3.14f;
    assert(std::any_cast<float>(any_value) == 3.14f);
}


//-----------------------------------------------------------------------------
class Fighter : public Derive<Fighter, Node>
{
public:
    Fighter():boolValue(false)
    {
        
    }
    void setHp(int value)
    {
        this->hp = value;
    }
    int getHp()
    {
        return this->hp;
    }
    void setMp(int value)
    {
        this->mp = value;
    }
    int getMp()
    {
        return this->mp;
    }
    void method_call(Params& in, Params& out)
    {
        printf("method called\n");
        out.clear();
        out.push_back(122);
    }
    
    enum Enum
    {
        ENUM_0,
        ENUM_1,
        ENUM_2,
        MAX
    };
    
    void setEnum(int value)
    {
        this->enumValue = value;
    }
    int getEnum()
    {
        return this->enumValue;
    }
    static EnumList<int>& getEnumEnumerator()
    {
        static EnumList<int> s_enum
        {
            std::make_tuple("ENUM_0", ENUM_0),
            std::make_tuple("ENUM_1", ENUM_1),
            std::make_tuple("ENUM_2", ENUM_2),
        };
        return s_enum;
    }
    void setString(const String& value)
    {
        this->string = value;
    }
    const String& getString()
    {
        return this->string;
    }
    void setBool(bool value)
    {
        this->boolValue = value;
    }
    bool isBool()
    {
        return this->boolValue;
    }
    
    static void bindProperty()
    {
        BIND_PROPERTY(bool, "bool", "bool", setBool, isBool, false, PropertyAttr::DEFAULT);
        BIND_PROPERTY(int32, "hp", "Fighter의 HP", setHp, getHp, 0, PropertyAttr::DEFAULT);
        BIND_PROPERTY(int32, "mp", "Fighter의 MP", setMp, getMp, 0, PropertyAttr::DEFAULT);
        BIND_ENUM_PROPERTY(int32, "enum", "Enum테스트", setEnum, getEnum, getEnumEnumerator, 0, PropertyAttr::DEFAULT);
        BIND_PROPERTY(const String&, "string", "string", setString, getString, "", PropertyAttr::DEFAULT);
    }
    
    static void bindMethod()
    {
        BIND_METHOD("method_call", "Method 호출 테스트", { self->method_call(in, out); }, 0, String(""), 0.0f, false);
    }
    
private:
    bool boolValue;
    int32 hp;
    int32 mp;
    int32 enumValue;
    String string;
};


//-----------------------------------------------------------------------------
void test_Reflection()
{
    auto test_fighter = newInstance<Fighter>();
    test_fighter->setHp(10);
    auto fighter_hp_prop = test_fighter->findProperty("hp");
    auto fighter_string_prop = test_fighter->findProperty("string");
    auto fighter_bool_prop = test_fighter->findProperty("bool");
    auto fighter_enum_prop = test_fighter->findProperty("enum");
    
    String fighter_hp_prop_value;
    fighter_hp_prop->toString(test_fighter, fighter_hp_prop_value);
    assert(fighter_hp_prop_value == "10");
    fighter_hp_prop->fromString(test_fighter, "22");
    assert(test_fighter->getHp() == 22);
    fighter_string_prop->fromString(test_fighter, "aaa");
    fighter_bool_prop->fromString(test_fighter, "true");
    String bool_value;
    fighter_bool_prop->toString(test_fighter, bool_value);
    assert(bool_value == "true");
    test_fighter->setEnum(Fighter::ENUM_2);
    String enum_value;
    fighter_enum_prop->toString(test_fighter, enum_value);
    assert(enum_value == "ENUM_2");
    
    auto test_method = test_fighter->findMethod("method_call");
    test_method->invoke(test_fighter);
    assert(std::any_cast<int>(test_method->out()[0]) == 122);
}


//-----------------------------------------------------------------------------
void test_EventDispatcher()
{
    static const char* EVENT_TEST = "EVENT_TEST";
    
    class EventSource : public EventDispatcher<>
    {
    public:
        virtual~EventSource() {}
        
        void emit()
        {
            this->dispatchEvent(EVENT_TEST,
                Params::newParam(123, String("test"), false));
        }
    };
    
    class EventSinker {};
    
    EventSinker sinker;
    EventSource source;
    source.addEventHandler(EVENT_TEST, &sinker, [](const Params* params)
   {
       printf("Event recieved %d\n",
              std::any_cast<int>(params->at(0)));
   });
    source.emit();
}



//-----------------------------------------------------------------------------
void test_Kernel()
{
    //String 으로 객체 생성
    auto obj = Kernel::instance()->create("Fighter");
    assert(obj != nullptr);
}


//-----------------------------------------------------------------------------
void test_NodeHierarchy()
{
    auto obj = Kernel::instance()->create("Fighter", "/node1/node2/fighter");
    assert(obj->getName() == "fighter");
    assert(obj->getType()->getName() == "Fighter");
    assert(obj->getAbsolutePath() == "/node1/node2/fighter");
    assert(obj == Kernel::instance()->lookup("/node1/node2/fighter"));
    assert(Kernel::instance()->lookup("/node1/node2") == obj->getRelativeNode(".."));
    assert(obj == obj->getRelativeNode("../.././node2/fighter"));
    assert(nullptr != obj->findProperty("hp"));
    assert(nullptr != obj->findProperty("visible"));
}


//-----------------------------------------------------------------------------
void test_Serializer()
{
    auto root = Kernel::instance()->lookup("/");
    root->setVisible(false);
    auto fighter = Kernel::instance()->lookup<Fighter>("/node1/node2/fighter");
    fighter->setHp(22);
    Serializer s;
    String json_str;
    s.serializeToJson(root, json_str);
    
    auto deserialized_node = s.deserializeFromJson(json_str);
    assert(deserialized_node != nullptr);


    {
        FileSystem::instance()->save("file://test_node.json", json_str.data(), json_str.size());
        auto deserialized_from_file = s.deserializeFromJsonFile("file://test_node.json");
        assert(deserialized_from_file);
        auto fighter = Kernel::instance()->lookup<Fighter>("/node1/node2/fighter");
        assert(fighter->getHp() == 22);
    }
}



//-----------------------------------------------------------------------------
void test_ThreadQueue()
{
    /*
    
    moodycamel::ConcurrentQueue<std::any> q;
    
    for (int i=0;i<8;++i)
    {
        std::thread([&q, i]()
        {
            std::any value;
            while (1)
            {
                if (!q.try_dequeue(value)) continue;
                //printf("thread %d %d\n", i, std::any_cast<int>(value));
                usleep(10);
            }
        }).detach();
    }
    
    for (int i=0;i<4;++i)
    {
        std::thread([&q, i]()
        {
            while (1)
            {
                q.enqueue((int)(rand() % 255));
                usleep(10);
            }
        }).detach();
    }
    
    while (1)
    {
        q.enqueue((int)(rand() % 255));
        printf("%d\n", q.size_approx());
        usleep(10000);
        
    }*/
}


//-----------------------------------------------------------------------------
void test_FileSystem()
{
    //MemMappedBuffer 읽기
    MemMappedBuffer buffer;
    FileSystem::instance()->load("file://test.txt", &buffer);    

    //비동기 방식 파일 읽기
    FileSystem::instance()->load("file://test.txt", [](Buffer* data)
    {
        printf("[%s]\n", data->data());
        return true;
    }, FileSystem::LoadOption().aync().string());
    printf("바로 떨어짐\n");
    
    //비동기 방식 Http 요청(지하철에서는 인터넷이 안되어 예외남)
    try
    {
        /*FileSystem::instance()->load("http://naver.com", [](FileSystem::Data& data)
        {
            printf("[%s]\n", &data[0]);
            return true;
        }, LoadOption().aync());*/
    }
    catch (...) {}
    printf("바로 떨어짐\n");
    
    //쫌 기다려준다(비동기 Http 요청이 끝나기 전에 프로그램 종료됨)
    std::this_thread::sleep_for(std::chrono::seconds(5));
}


//-----------------------------------------------------------------------------
void test_StaticString()
{
    std::mutex lock;
    std::set<StaticString> test_set;

    std::thread([&lock, &test_set]()
    {
        for (int32 i = 0; i < 1000; ++i)
        {
            char buf[1024];
            TOD_SNPRINTF(buf, sizeof(buf), "Thread%d", i);
            {
                std::lock_guard<std::mutex> guard(lock);
                test_set.insert(buf);
            }
        }
    }).detach();

    std::thread([&lock, &test_set]()
    {
        for (int32 i = 1000; i < 2000; ++i)
        {
            char buf[1024];
            TOD_SNPRINTF(buf, sizeof(buf), "Thread%d", i);
            {
                std::lock_guard<std::mutex> guard(lock);
                test_set.insert(buf);
            }
        }
    }).detach();

    std::thread([&lock, &test_set]()
    {
        for (int32 i = 0; i < 1000; ++i)
        {
            char buf[1024];
            TOD_SNPRINTF(buf, sizeof(buf), "Thread%d", i);
            {
                std::lock_guard<std::mutex> guard(lock);
                test_set.insert(buf);
            }
        }
    }).detach();

    TimeMgr::instance()->sleep(2000);

    assert(test_set.size() == 2000);

    for (int32 i = 0; i < 2000; ++i)
    {
        char buf[1024];
        TOD_SNPRINTF(buf, sizeof(buf), "Thread%d", i);
        assert(test_set.find(buf) != test_set.end());
    }

    StaticString s("Thread1");
    assert(s == "Thread1");
}


//-----------------------------------------------------------------------------
void test_ArgParser(int argc, char* argv[])
{   
    ArgParser arg_parser("executer.exe --cmd --bool true --bool2 --int32 3 --string test");
    arg_parser.addOption<bool>("bool", "desc", false);
    arg_parser.addOption<bool>("bool2", "desc", false);
    arg_parser.addOption<int32>("int32", "desc", 0);
    arg_parser.addOption<String>("string", "desc", "default");
    arg_parser.addCommand("cmd", "desc", [&arg_parser]()
    {
        bool out_bool = false;
        assert(arg_parser.getOption("bool", out_bool));
        assert(out_bool == true);

        bool out_bool2 = false;
        assert(arg_parser.getOption("bool2", out_bool2));
        assert(out_bool2 == true);

        int32 out_int32 = 0;
        assert(arg_parser.getOption("int32", out_int32));
        assert(out_int32 == 3);

        String out_string;
        assert(arg_parser.getOption("string", out_string));
        assert(out_string == "test");

        return true;
    });
    assert(arg_parser.parse() == true);
    assert(arg_parser.execute() == true);
}


//-----------------------------------------------------------------------------
void test_BinaryStreamWriter()
{
    BinaryStreamWriter writer;
    writer.write(10);
    writer.write(String("test"));
    assert(writer.size() == 12);
    assert(writer.remain() == 16);
    assert(writer.capacity() == 28);
}


//-----------------------------------------------------------------------------
void test_Http()
{
    {
        //Sync
        tod::Http r;
        const tod::Http::Response& response = r.request("https://curl.haxx.se").await();
        assert(response.getStatusCode() == 200);
    }

    {
        //Async
        tod::Http r;
        r.request("https://curl.haxx.se").then([](const tod::Http::Response & response)
        {
            assert(response.getStatusCode() == 200);
        });
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
}


//-----------------------------------------------------------------------------
void test_Log()
{
    Logger::instance()->setStandardLog(Logger::StandardLog().console());

    std::thread([]()
    {
        while (true)
        {
            TOD_LOG("ERROR", "test %d", 10);
            Sleep(1);
        }
    }).detach();

    std::thread([]()
    {
        while (true)
        {
            TOD_LOG("INFO", "test");
            Sleep(1);
        }
    }).detach();

    while (1)
    {
        Sleep(1);
    }
}

#include <regex>


namespace tod::graphics
{
    template<class Facet>
    struct deletable_facet : Facet
    {
        template<class ...Args>
        deletable_facet(Args&& ...args): Facet(std::forward<Args>(args)...) {}
        ~deletable_facet() {}
    };

    class ShaderParser
    {
    public:
        /*
            주석 : 여러줄, 한줄
            cbuffer
            struct
            #define
            #if defined
            #else
            #endif
            VertexShader
            FragmentShader            
        */

        struct CodeBuilder
        {
            virtual~CodeBuilder() {}
            virtual void beginStruct(const std::wstring& name) = 0;
            virtual void endStruct() = 0;
            virtual void beginCBuffer(const std::wstring& name) = 0;
            virtual void endCBuffer() = 0;
            virtual void beginInputLayout(const std::wstring& name) = 0;
            virtual void endInputLayout() = 0;
            virtual void writeVariable(const std::wstring& type, const std::wstring& name) = 0;
            virtual void writeInputLayoutVariable(const std::wstring& type, const std::wstring& name, const std::wstring& semantic) = 0;
        };

        struct HeaderCodeBuilder : public CodeBuilder
        {
            HeaderCodeBuilder(std::wstring* output)
                : output(output)
            {
                if (nullptr != output)
                {
                    output->append(L"#pragma once\n");
                    output->append(L"#include \"tod/graphics/vector2.h\"\n");
                    output->append(L"#include \"tod/graphics/vector3.h\"\n");
                    output->append(L"#include \"tod/graphics/vector4.h\"\n");
                    output->append(L"#include \"tod/graphics/matrix44.h\"\n");
                    output->append(L"#include \"tod/graphics/compactfloat.h\"\n");
                    output->append(L"namespace tod::graphics\n");
                    output->append(L"{\n\n");
                }
            }
            virtual~HeaderCodeBuilder()
            {
                if (nullptr != output)
                {
                    output->append(L"\n}\n");
                }
            }

            void beginStruct(const std::wstring& name) override
            {
                output->append(L"struct ");
                output->append(name);
                output->append(L"\n");
                output->append(L"{\n");
            }

            void endStruct() override
            {
                output->append(L"};\n\n");
            }

            void beginCBuffer(const std::wstring& name) override
            {
                output->append(L"struct ");
                output->append(name);
                output->append(L"\n");
                output->append(L"{\n");
            }

            void endCBuffer() override
            {
                output->append(L"};\n\n");
            }

            void writeVariable(const std::wstring& type, const std::wstring& name) override
            {
                //Indent
                output->append(L"    ");

                if (L"float4x4" == type)
                {
                    output->append(L"Matrix44 " + name);
                }
                else if (L"float" == type)
                {
                    output->append(L"float " + name);
                }
                else if (L"float2" == type)
                {
                    output->append(L"Vector2 " + name);
                }
                else if (L"float3" == type)
                {
                    output->append(L"Vector3 " + name);
                }
                else if (L"float4" == type)
                {
                    output->append(L"Vector4 " + name);
                }
                else if (L"int32" == type)
                {
                    output->append(L"int32 " + name);
                }
                else if (L"uint32" == type)
                {
                    output->append(L"uint32 " + name);
                }
                else if (L"R32G32B32A32F" == type)
                {
                    output->append(L"Vector4 " + name);
                }
                else if (L"R32G32B32F" == type)
                {
                    output->append(L"Vector3 " + name);
                }
                else if (L"R32G32F" == type)
                {
                    output->append(L"Vector2 " + name);
                }
                else if (L"R8G8B8A8U" == type)
                {
                    output->append(L"std::array<uint8, 4> " + name);
                }
                else if (L"R8G8B8A8UN" == type)
                {
                    output->append(L"std::array<CompactFloat<uint8>, 4> " + name);
                }
                else
                {
                    //배열
                    std::wregex array_re(L"([[:alpha:]]+\\w*)\\[([[:digit:]]+)\\]");
                    const auto b = std::wsregex_iterator(type.begin(), type.end(), array_re);
                    const auto e = std::wsregex_iterator();
                    if (0 < std::distance(b, e))
                    {
                        for (std::wsregex_iterator i = b; i != e; ++i)
                        {
                            const std::wsmatch& match = *i;
                            const auto& array_type = match[1].str();
                            output->append(L"std::array<" + array_type + L", " + match[2].str() + L"> " + name);
                        }
                    }
                    else
                    {
                        output->append(type + L" " + name);
                    }
                }

                output->append(L";\n");
            }

            void beginInputLayout(const std::wstring& name) override
            {
                output->append(L"struct ");
                output->append(name);
                output->append(L"\n");
                output->append(L"{\n");
            }

            void endInputLayout() override
            {
                output->append(L"};\n\n");
            }

            void writeInputLayoutVariable(const std::wstring& type, const std::wstring& name, const std::wstring& semantic) override
            {
                this->writeVariable(type, name);
            }

            std::wstring* output;
        };

        struct HLSLCodeBuilder : public CodeBuilder
        {
            HLSLCodeBuilder(std::wstring* output)
                : output(output)
            {
            }
            virtual~HLSLCodeBuilder()
            {
            }

            void beginStruct(const std::wstring& name) override
            {
                output->append(L"struct ");
                output->append(name);
                output->append(L"\n");
                output->append(L"{\n");
            }

            void endStruct() override
            {
                output->append(L"};\n\n");
            }

            void beginCBuffer(const std::wstring& name) override
            {
                output->append(L"cbuffer ");
                output->append(name);
                output->append(L"\n");
                output->append(L"{\n");
            }

            void endCBuffer() override
            {
                output->append(L"};\n\n");
            }

            void writeVariable(const std::wstring& type, const std::wstring& name) override
            {
                //Indent
                output->append(L"    ");

                if (L"int32" == type)
                {
                    output->append(L"int" + name);
                }
                if (L"uint32" == type)
                {
                    output->append(L"uint" + name);
                }
                else
                {
                    output->append(type + L" " + name);
                }

                output->append(L";\n");
            }

            void beginInputLayout(const std::wstring& name) override
            {
                output->append(L"struct ");
                output->append(name);
                output->append(L"\n");
                output->append(L"{\n");
            }

            void endInputLayout() override
            {
                output->append(L"};\n\n");
            }

            void writeInputLayoutVariable(const std::wstring& type, const std::wstring& name, const std::wstring& semantic) override
            {
                //Indent
                output->append(L"    ");

                output->append(type + L" " + name + L" : " + semantic);

                output->append(L";\n");
            }
            
            std::wstring* output;
        };

#define CODEBUILD(func, ...) for (auto& cb = code_builders.begin(); cb != code_builders.end(); ++cb) { (*cb)-> func (__VA_ARGS__); }
        typedef std::vector<CodeBuilder*> CodeBuilders;

        bool convert(
              const std::wstring& code
            , OUT std::wstring* header_code = nullptr
            , OUT std::wstring* hlsl_code = nullptr
            , OUT std::wstring* glsl_code = nullptr)
        {
            CodeBuilders code_builders;
            HeaderCodeBuilder header_code_builder(header_code);
            HLSLCodeBuilder hlsl_code_builder(hlsl_code);
            if (nullptr != header_code)
            {
                code_builders.push_back(&header_code_builder);
            }
            if (nullptr != hlsl_code)
            {
                code_builders.push_back(&hlsl_code_builder);
            }

            std::wstring pure_code;

            //주석제거
            {
                {
                    //한줄 주석
                    std::wstring eleminate_comments;
                    std::wregex commnect_re(L"//.*");
                    pure_code = std::regex_replace(code, commnect_re, L"");
                }

                {
                    //여러줄 주석
                    std::wstring eleminate_comments;
                    std::wregex commnect_re(L"/\\*[[:s:][:print:]]*?\\*/");
                    pure_code = std::regex_replace(pure_code, commnect_re, L"");
                }
            }


            //struct
            {
                std::wregex cbuffer_re(L"\\s+struct\\s+(\\w+)\\s+\\{\\s+([\\w\\s;\\[\\]]*)\\}\\s*;");
                const auto b = std::wsregex_iterator(pure_code.begin(), pure_code.end(), cbuffer_re);
                const auto e = std::wsregex_iterator();
                for (std::wsregex_iterator i = b; i != e; ++i)
                {
                    const std::wsmatch& match = *i;
                    CODEBUILD(beginStruct, match[1].str());
                    this->convertMemberVariables(match[2].str(), code_builders);
                    CODEBUILD(endStruct);
                }
            }
            

            //cbuffer
            {
                std::wregex cbuffer_re(L"\\s+cbuffer\\s+(\\w+)\\s+\\{\\s+([\\w\\s;\\[\\]]*)\\}\\s*;");
                const auto b = std::wsregex_iterator(pure_code.begin(), pure_code.end(), cbuffer_re);
                const auto e = std::wsregex_iterator();
                for (std::wsregex_iterator i = b; i != e; ++i)
                {
                    const std::wsmatch& match = *i;
                    CODEBUILD(beginCBuffer, match[1].str());
                    this->convertMemberVariables(match[2].str(), code_builders);
                    CODEBUILD(endCBuffer);
                }
            }


            //inputlayout
            {
                std::wregex cbuffer_re(L"\\s+inputlayout\\s+(\\w+)\\s+\\{\\s+([\\w\\s;\\[\\]:]*)\\}\\s*;");
                const auto b = std::wsregex_iterator(pure_code.begin(), pure_code.end(), cbuffer_re);
                const auto e = std::wsregex_iterator();
                for (std::wsregex_iterator i = b; i != e; ++i)
                {
                    const std::wsmatch& match = *i;
                    CODEBUILD(beginInputLayout, match[1].str());
                    this->convertInputLayoutVariables(match[2].str(), code_builders);
                    CODEBUILD(endInputLayout);
                }
            }

            return true;
        }

        void convertMemberVariables(const std::wstring& code, CodeBuilders& code_builders) const
        {
            const std::wregex re(L"\\s*([[:alpha:]]+[\\w\\[\\]]*)\\s+([[:alpha:]]+[\\[\\]\\w]*)\\s*;");
            const auto b = std::wsregex_iterator(code.begin(), code.end(), re);
            const auto e = std::wsregex_iterator();
            for (std::wsregex_iterator i = b; i != e; ++i)
            {
                const std::wsmatch& match = *i;                
                CODEBUILD(writeVariable, match[1].str(), match[2].str());
            }
        }

        void convertInputLayoutVariables(const std::wstring& code, CodeBuilders& code_builders) const
        {
            const std::wregex re(L"\\s*([[:alpha:]]+[\\w\\[\\]]*)\\s+([[:alpha:]]+[\\[\\]\\w]*)[ \t]*:[ \t]*([[:upper:][:digit:]]*)\\s*;");
            const auto b = std::wsregex_iterator(code.begin(), code.end(), re);
            const auto e = std::wsregex_iterator();
            for (std::wsregex_iterator i = b; i != e; ++i)
            {
                const std::wsmatch& match = *i;
                CODEBUILD(writeInputLayoutVariable, match[1].str(), match[2].str(), match[3].str());
            }
        }
    };
};

void test_ParseTodShader()
{
    const wchar_t* shader_code = L"\
 /*test안녕하세요1234567890!@#$%^&*()_+`~:;\"'<>,.?/|\\=-asdkjfhkljsdaf\n\
        asdasd*/ \
    cbuffer TodBaseFrameCBuffer //하하하\n\
    {\n\
        float4x4 matWVP;\n\
        float4x4 matWorld; //주석\n\
        float4x4 matInvWorld;\n\
        float4x4 matView;\n\
        float4x4 matInvView; /*이것은 주석입니다.*/\n\
        float4x4 matProj;\n\
        float3 vecViewPos;\n\
        //float fTime;\n\
        uint32 iLightCount;\n\
        float padding[3];\n\
    };\n\
\n\
    struct Light\n\
    {\n\
        float intensity;\n\
    };\n\
\n\
    cbuffer TodBaseLightCBuffer\n\
    {\n\
        Light lights[8];\n\
    };\n\
\n\
    inputlayout NormalVSInput\n\
    {\n\
        R32G32B32F vecPosition  : POSITION;\n\
        R32G32B32F vecNormal    : NORMAL;\n\
        R32G32F    vecUV        : TEXCOORD;\n\
    };\n\
\n\
    inputlayout SkinnedVSInput\n\
    {\n\
        R32G32B32F  vecPosition  : POSITION;\n\
        R32G32B32F  vecNormal    : NORMAL;\n\
        R32G32F     vecUV        : TEXCOORD;\n\
        R8G8B8A8U   boneIndex     : BLENDINDICES;\n\
        R8G8B8A8UN  weight       : BLENDWEIGHTS;\n\
    };\n\
    ";

    tod::graphics::ShaderParser shader_parser;
    std::wstring hlsl, header;
    shader_parser.convert(shader_code, &header, &hlsl);
}


void test_CompactFloat()
{
    tod::graphics::CompactFloat<uint8> f = 0.5f;
    assert(0.501960814f == f);
    assert(f * 10 == 5.01960814f);
    f = 0;
    assert(0 == f);
    f = 0.1f;
    assert(0.101960786f == f);
}


//-----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    Logger::instance()->setStandardLog(tod::Logger::StandardLog().console());


    test_ParseTodShader();


    /*

    test_CompactFloat();
    test_Log();
    test_FileSystem();

    REGISTER_TYPE(Fighter);

    test_Interpolation();    
    test_Random();
    test_TimeMgr();
    test_NodeRelease();
    test_Utilites();
    test_Pool();
    test_any();
    test_Reflection();
    test_EventDispatcher();
    test_Kernel();
    test_NodeHierarchy();
    test_Serializer();
    test_ThreadQueue();
    test_FileSystem();
    test_StaticString();
    test_ArgParser(argc, argv);
    test_BinaryStreamWriter();
    test_Http();*/

    return 0;
}
