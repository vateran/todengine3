#pragma once
#include <vector>
#include "tod/basetype.h"
namespace tod
{

class Buffer
{
public:
    typedef std::vector<int8> DataType;
    typedef DataType::iterator iterator;
    typedef DataType::const_iterator const_iterator;

public:
    Buffer() {}
    Buffer(const Buffer& buffer) { this->d = buffer.d; }
    Buffer(Buffer&& buffer) { this->d = std::move(buffer.d); }
    virtual~Buffer() {}

    iterator insert(const_iterator where, const int8& value) { return this->d.emplace(where, value); }
    iterator insert(const_iterator where, int8&& value) { return this->d.emplace(where, _STD move(value)); }
    iterator insert(const_iterator where, int8* b, int8* e) { return this->d.insert(where, b, e); }
    iterator begin() { return this->d.begin(); }
    const_iterator begin() const { return this->d.begin(); }
    iterator end() { return this->d.end(); }
    const_iterator end() const { return this->d.end(); }
    void clear() { this->d.clear(); }
    bool empty() const { return this->d.empty(); }
    size_t size() const { return this->d.size(); }
    void resize(size_t size) { this->d.resize(size); }
    int8& at(size_t index) { return this->d[index]; }
    const int8& at(size_t index) const { return this->d[index]; }
    int8* data() { return this->d.data(); }
    int8& operator [] (size_t index) { return this->d[index]; }
    const int8& operator [] (size_t index) const { return this->d[index]; }
    Buffer* operator & () { return this; }

private:
    DataType d;
};

class MemMappedBuffer : public Buffer
{
public:
};

}