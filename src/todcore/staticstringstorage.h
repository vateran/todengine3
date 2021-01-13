#pragma once
namespace tod
{

class StaticStringStorage : public Singleton<StaticStringStorage>
{
public:
    StaticStringStorage();

    int32 addString(const String& str);
    const String& getString(int32 hash) const;

private:
    typedef std::unordered_map<int32, String> Strings;
    mutable std::mutex lock;
    Strings strings;
};

}