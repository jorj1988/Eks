#ifndef XFUTURE_H
#define XFUTURE_H

#include "XGlobal.h"
#include "Utilities/XProperty.h"
#include "Utilities/XAssert.h"
#include <atomic>

namespace Eks
{

class ThreadSafeAllocator;

namespace detail
{
template <typename Fn, typename Arg, typename Result> class ContinuationTask;
class TaskImpl;

template <typename Fn, typename Arg> class ContinuationTaskResult
  {
  typedef typename std::result_of<Fn(Arg)>::type Type;
  };

template <typename Fn> class ContinuationTaskResult<Fn, void>
  {
  typedef typename std::result_of<Fn()>::type Type;
  };
}

// Represents a function which returns [ResultType]
template <typename ResultType> class Task
  {
public:
  typedef ResultType Result;
  typedef detail::TaskImpl Impl;
  typedef Impl *Token;

XProperties:
  // the Task token that represents the completion of the task
  XRORefProperty(Token, token);

public:
  // create a new task
  Task(Eks::ThreadSafeAllocator *allocator);

  // complete the task now, with the given value as an argument
  template <typename T> static void completeTaskSync(Token token, const T *val);

  // creates a new task that completes when both [this] and [oth] are complete.
  template <typename OthResult> Task<void> whenBoth(const Task<OthResult> &oth);

  // creates a new task that completes when both [this] and [oth] are complete.
  template <typename OthResult> Task<void> operator&&(const Task<OthResult> &oth)
    {
    return whenBoth(oth);
    }

  // create a new task which executes [fn] and add the task as a dependent
  // (ie to be completed when [this] completes)
  template <typename Fn>
      Task<typename detail::ContinuationTaskResult<Fn, Result>::Type> then(const Fn &fn) const
    {
    typedef typename std::result_of<Fn(Result)>::type DerivedResult;

    return detail::ContinuationTask<Fn, Result, DerivedResult>::create(*this, fn);
    }
  };

}

#include "Async/XAsyncTaskImpl.h"

#endif // XFUTURE_H