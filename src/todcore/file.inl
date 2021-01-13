namespace tod
{

//-----------------------------------------------------------------------------
template <typename TYPE>
uint32 File::write(const TYPE& value)
{
    TOD_ASSERT(nullptr != this->fp, "open 해야함");
    TOD_ASSERT(true == this->openMode.isWrite(), "write로 오픈안함");
    return static_cast<uint32>(fwrite(&value, sizeof(TYPE), 1, fp));
}

//-----------------------------------------------------------------------------
template <typename TYPE>
uint32 File::read(TYPE& value_out)
{
    TOD_ASSERT(nullptr != this->fp, "open 해야함");
    TOD_ASSERT(true == this->openMode.isRead(), "read로 오픈안함");
    return static_cast<uint32>(fread((void*)&value_out, sizeof(value_out), 1, this->fp));
}

}
