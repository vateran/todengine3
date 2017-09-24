#pragma once
namespace tod
{

template <typename T>
class EnumOption
{
public:
    typedef EnumOption<T> type;
    
public:
    EnumOption():option(0) {}
    EnumOption(int option):option(option) {}
    
    type& add(T value)
    {
        this->option |= (1<<static_cast<int>(value));
        return *this;
    }
    
    bool isOn(T value) const
    {
        return (this->option & (1<<static_cast<int>(value)))>0;
    }
    
    void on(T value)
    {
        this->option |= 1<<static_cast<int>(value);
    }
    
    void off(T value)
    {
        this->option &= ~(1<<static_cast<int>(value));
    }
    
    void flip(T value)
    {
        if (this->isOn(value)) this->off(value);
        else this->on(value);
    }
    
    int getOption() const
    {
        return this->option;
    }
    
    bool operator [] (T value) const
    {
        return this->isOn(value);
    }
    
    operator int () const
    {
        return this->option;
    }
    
private:
    int option;
};

}
