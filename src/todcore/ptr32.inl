namespace tod
{

//----------------------------------------------------------------------------
template <typename TYPE>
Ptr32<TYPE>::Ptr32()
: pointer(0)
{
}


//----------------------------------------------------------------------------
template <typename TYPE>
Ptr32<TYPE>::Ptr32(TYPE* ptr)
: pointer(0)
{
    if (nullptr != ptr)
    {
        this->encode(ptr);
    }
}


//----------------------------------------------------------------------------
template <typename TYPE>
void Ptr32<TYPE>::reset()
{
    this->pointer = 0;
}


//----------------------------------------------------------------------------
template <typename TYPE>
bool Ptr32<TYPE>::null() const
{
    return 0 == this->pointer;
}


//----------------------------------------------------------------------------
template <typename TYPE>
TYPE* Ptr32<TYPE>::get() const
{
    return static_cast<TYPE*>(this->decode());
}


//----------------------------------------------------------------------------
template <typename TYPE>
TYPE* Ptr32<TYPE>::operator -> () const
{
    return this->get();
}


//----------------------------------------------------------------------------
template <typename TYPE>
bool Ptr32<TYPE>::operator == (TYPE* ptr) const
{
    return (this->get() == ptr);
}


//----------------------------------------------------------------------------
template <typename TYPE>
bool Ptr32<TYPE>::operator != (TYPE* ptr) const
{
    return (this->get() != ptr);
}


//----------------------------------------------------------------------------
template <typename TYPE>
struct Ptr32<TYPE>::HighBitStorage : public Singleton<HighBitStorage>
{
public:
    HighBitStorage();

    int32 alloc(const int32 high_bit);
    inline int32 getHighBit(int32 index) const;

private:
    std::mutex m;
    int32 highBits[16];
};


//----------------------------------------------------------------------------
template <typename TYPE>
Ptr32<TYPE>::HighBitStorage::HighBitStorage()
{
    memset(this->highBits, 0, sizeof(this->highBits));
}


//----------------------------------------------------------------------------
template <typename TYPE>
int32 Ptr32<TYPE>::HighBitStorage::alloc(const int32 high_bit)
{
    for (int32 i = 0; i < 16; ++i)
    {
        if (high_bit == this->highBits[i])
        {
            return i;
        }
        else if (0 == this->highBits[i])
        {
            break;
        }
    }

    //1) 기존에 동일한 high bit 가 존재하는지 확인
    std::lock_guard<std::mutex> guard(this->m);
    for (int32 i = 0; i < 16; ++i)
    {
        if (high_bit == this->highBits[i])
        {
            return i;
        }
        else if (0 == this->highBits[i])
        {
            this->highBits[i] = high_bit;
            return i;
        }
    }

    //저장된 hight bit가 없거나 할당할 공간이 없음
    TOD_ASSERT(false, "high bit 를 저장할 공간이 없음");
    return -1;
}


//----------------------------------------------------------------------------
template <typename TYPE>
int32 Ptr32<TYPE>::HighBitStorage::getHighBit(int32 index) const
{
    return this->highBits[index];
}



//----------------------------------------------------------------------------
template <typename TYPE>
void Ptr32<TYPE>::encode(void* ptr)
{
    TOD_ASSERT(8 == (sizeof(ptr)), "64bit pointer 만 사용가능");
    TOD_ASSERT(0 == (reinterpret_cast<int64>(ptr) & 0xf), "align 되어있지 않은 pointer는 사용불가능");

    int32 high_bit = reinterpret_cast<int64>(ptr) >> 32;
    int32 high_bit_index = HighBitStorage::instance()->alloc(high_bit);
    this->pointer = static_cast<uint32>(reinterpret_cast<int64>(ptr)) | high_bit_index;
}


//----------------------------------------------------------------------------
template <typename TYPE>
void* Ptr32<TYPE>::decode() const
{
    int32 high_bit_index = this->pointer & 0xf;
    int64 ptr = HighBitStorage::instance()->getHighBit(high_bit_index);
    ptr = ((ptr << 32) | this->pointer) & 0xfffffffffffffff0ULL;
    return reinterpret_cast<void*>(ptr);
}

}