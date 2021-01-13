namespace tod
{

//-----------------------------------------------------------------------------
template <typename TYPE>
uint32 File::write(const TYPE& value)
{
    TOD_ASSERT(nullptr != this->fp, "open �ؾ���");
    TOD_ASSERT(true == this->openMode.isWrite(), "write�� ���¾���");
    return static_cast<uint32>(fwrite(&value, sizeof(TYPE), 1, fp));
}

//-----------------------------------------------------------------------------
template <typename TYPE>
uint32 File::read(TYPE& value_out)
{
    TOD_ASSERT(nullptr != this->fp, "open �ؾ���");
    TOD_ASSERT(true == this->openMode.isRead(), "read�� ���¾���");
    return static_cast<uint32>(fread((void*)&value_out, sizeof(value_out), 1, this->fp));
}

}
