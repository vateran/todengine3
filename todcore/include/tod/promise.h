#pragma once
#include "tod/basetype.h"
#include "tod/exception.h"
#include <mutex>
#include <condition_variable>
#include <functional>
namespace tod
{

template <typename RESULT_TYPE, typename SELF_TYPE>
class Promise
{
public:
    typedef std::function<void()> CommitFunc;
    typedef std::function<void(const RESULT_TYPE&)> CallbackFunc;
    typedef std::list<CallbackFunc> Callbacks;
    typedef Promise<RESULT_TYPE, SELF_TYPE> PromiseType;

public:
    Promise();

    void then(const CallbackFunc& callbackFunc) const;
    const RESULT_TYPE& await(uint32 limitMilliSecond = 0) const;

private:
    uint32 addRef() const;
    uint32 release() const;
    void complete() const;

private:
    mutable RESULT_TYPE result;
    mutable std::mutex signalLock;
    mutable std::condition_variable signal;
    mutable CommitFunc commitFunc;
    mutable CallbackFunc callbackFunc;
    mutable uint32 refCount;
    friend SELF_TYPE;
};

}
