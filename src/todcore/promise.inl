namespace tod
{

//-----------------------------------------------------------------------------
template <typename RESULT_TYPE, typename SELF_TYPE>
Promise<RESULT_TYPE, SELF_TYPE>::Promise()
: refCount(1)
{
}


//-----------------------------------------------------------------------------
template <typename RESULT_TYPE, typename SELF_TYPE>
void
Promise<RESULT_TYPE, SELF_TYPE>::then(const CallbackFunc& callbackFunc) const
{
    this->addRef();
    this->callbackFunc = callbackFunc;
}


//-----------------------------------------------------------------------------
template <typename RESULT_TYPE, typename SELF_TYPE>
const RESULT_TYPE&
Promise<RESULT_TYPE, SELF_TYPE>::await(uint32 limitMilliSecond) const
{
    TOD_ASSERT(
        nullptr == this->callbackFunc
        , "then 과 함께 사용할 수 없습니다.");

    std::unique_lock<std::mutex> guard(this->signalLock);

    if (0 == limitMilliSecond)
    {
        this->signal.wait(guard);
    }
    else
    {
        this->signal.wait_for(
            guard
            , std::chrono::milliseconds(limitMilliSecond));
    }

    return this->result;
}


//-----------------------------------------------------------------------------
template <typename RESULT_TYPE, typename SELF_TYPE>
uint32 Promise<RESULT_TYPE, SELF_TYPE>::addRef() const
{
    return ++this->refCount;
}


//-----------------------------------------------------------------------------
template <typename RESULT_TYPE, typename SELF_TYPE>
uint32 Promise<RESULT_TYPE, SELF_TYPE>::release() const
{
    if (0 == --this->refCount)
    {
        delete this;
        return 0;
    }

    return this->refCount;
}


//-----------------------------------------------------------------------------
template <typename RESULT_TYPE, typename SELF_TYPE>
void Promise<RESULT_TYPE, SELF_TYPE>::complete() const
{
    {
        std::unique_lock<std::mutex> guard(this->signalLock);
        this->signal.notify_all();
    }

    if (nullptr != this->callbackFunc)
    {
        callbackFunc(this->result);
        this->release();
    }
}
}