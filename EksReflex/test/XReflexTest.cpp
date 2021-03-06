#include "XReflexTest.h"
#include "XUnorderedMap"
#include "Reflex/FunctionBuilder.h"
#include "Reflex/ClassBuilder.h"
#include "Reflex/NamespaceBuilder.h"
#include "Reflex/EmbeddedTypes.h"
#include <QtTest>

#define FLOAT_VAL 5.0f
#define DOUBLE_VAL 5.0
#define INT_VAL 2222
#define SELF_VAL 500

#define QCOMPARE_NO_RETURN(actual, expected) \
do {\
    if (!QTest::qCompare(actual, expected, #actual, #expected, __FILE__, __LINE__))\
      throw std::exception();\
} while (0)

class A
  {
public:
  void pork1(const float& in, double* pork)
    {
    QCOMPARE_NO_RETURN(this->pork, SELF_VAL);
    QCOMPARE_NO_RETURN(in, FLOAT_VAL);
    QCOMPARE_NO_RETURN(*pork, DOUBLE_VAL);
    }

  int pork2(A *a) const
    {
    QCOMPARE_NO_RETURN(pork, SELF_VAL);
    QCOMPARE_NO_RETURN(a->pork, SELF_VAL);
    return INT_VAL;
    }

  static A* pork3(const float& in)
    {
    QCOMPARE_NO_RETURN(in, FLOAT_VAL);
    static A a;
    a.pork = SELF_VAL;
    return &a;
    }

  int pork;
  };

namespace Eks
{
namespace Reflex
{
namespace detail
{
template <> struct TypeResolver<A> 
  {
  static const Type *find()
    {
    static Type t;
    return &t;
    }
  };

}
}
}

class TestBuilder : public Eks::Reflex::BuilderBase
  {
  Eks::Reflex::Symbol lookupSymbol(const char*) X_OVERRIDE
    {
    xAssertFail();
    return Eks::Reflex::Symbol();
    }

  Eks::UnorderedMap<Eks::String*, Eks::Reflex::SymbolData> symbols;
  };

class InvocationBuilder
  {
public:
  class Arguments
    {
  public:
    void **_args;
    void *_this;
    void *_result;
    };
  typedef Arguments *CallData;

  struct Result
    {
    typedef void (*Signature)(CallData);
    Signature fn;
    };

  template <typename Builder> static Result build()
    {
    Result r = { call<Builder> };
    return r;
    }

  template <typename Builder> static void call(CallData data)
    {
    Builder::call(data);
    }

  template <typename T> static T getThis(CallData args)
    {
    return (T)args->_this;
    }

  template <xsize I, typename Tuple>
      static typename std::tuple_element<I, Tuple>::type unpackArgument(CallData args)
    {
    typedef typename std::tuple_element<I, Tuple>::type Arg;
    typedef typename std::remove_reference<Arg>::type NoRef;
    return *(NoRef*)args->_args[I];
    }

  template <typename Return, typename T> static void packReturn(CallData data, T &&result)
    {
    *(Return*)data->_result = result;
    }
  };

class ClassBuilder
  {
public:
  typedef InvocationBuilder::Result Function;

  template <typename Fn> static Function build(const Fn &fn)
    {
    return fn.buildInvocation<InvocationBuilder>();
    }
  };

void EksReflexTest::functionWrapTest()
  {
  using namespace Eks::Reflex;
  typedef A ReflexClass;

  auto method1 = REFLEX_METHOD(pork1);
  auto method2 = REFLEX_METHOD(pork2);
  auto method3 = REFLEX_METHOD(pork3);

  QCOMPARE(method1.returnType(), findType<void>());
  QCOMPARE(method2.returnType(), findType<int>());
  QCOMPARE(method3.returnType(), findType<A*>());

  QCOMPARE(method1.argumentCount(), 2U);
  QCOMPARE(method2.argumentCount(), 1U);
  QCOMPARE(method3.argumentCount(), 1U);

  QCOMPARE(method1.argumentType<0>(), findType<const float&>());
  QCOMPARE(method1.argumentType<1>(), findType<double*>());
  QCOMPARE(method2.argumentType<0>(), findType<A *>());
  QCOMPARE(method3.argumentType<0>(), findType<const float &>());

  QCOMPARE(decltype(method1)::Helper::Const::value, false);
  QCOMPARE(decltype(method2)::Helper::Const::value, true);
  QCOMPARE(decltype(method3)::Helper::Const::value, false);

  QCOMPARE(decltype(method1)::Helper::Static::value, false);
  QCOMPARE(decltype(method2)::Helper::Static::value, false);
  QCOMPARE(decltype(method3)::Helper::Static::value, true);

  QCOMPARE(findType<decltype(method1)::Helper::Class>(), findType<A>());
  QCOMPARE(findType<decltype(method2)::Helper::Class>(), findType<A>());
  QCOMPARE(findType<decltype(method3)::Helper::Class>(), findType<void>());
  }

void EksReflexTest::functionInvokeTest()
  {
  using namespace Eks::Reflex;
  typedef A ReflexClass;

  auto method1 = REFLEX_METHOD(pork1);
  auto method2 = REFLEX_METHOD(pork2);
  auto method3 = REFLEX_METHOD(pork3);

  auto inv1 = method1.buildInvocation<InvocationBuilder>();
  auto inv2 = method2.buildInvocation<InvocationBuilder>();
  auto inv3 = method3.buildInvocation<InvocationBuilder>();

  A a;
  a.pork = SELF_VAL;
  double dbl = DOUBLE_VAL;

  float flt = FLOAT_VAL;

  const float &arg11 = flt;
  double *arg12 = &dbl;
  A* arg21 = &a;
  const float &arg31 = flt;
  void *args1[] = { (void*)&arg11, &arg12 };
  void *args2[] = { &arg21 };
  void *args3[] = { (void*)&arg31 };

  int result2;
  A* result3;

  A ths;
  ths.pork = SELF_VAL;

  InvocationBuilder::Arguments data1 = { args1, &ths, nullptr };
  InvocationBuilder::Arguments data2 = { args2, &ths, &result2 };
  InvocationBuilder::Arguments data3 = { args3, &ths, &result3 };

  try
    {
    inv1.fn(&data1);
    inv2.fn(&data2);
    inv3.fn(&data3);
    }
  catch(...)
    {
    }

  QCOMPARE(result2, INT_VAL);
  QCOMPARE(result3->pork, SELF_VAL);
  }

void EksReflexTest::classWrapTest()
  {
  auto owner = TestBuilder();
  auto ns = REFLEX_NAMESPACE("", owner);
  auto cls = REFLEX_CLASS(ClassBuilder, A, ns);

  auto method1 = REFLEX_METHOD(pork1);
  auto method2 = REFLEX_METHOD(pork2);
  auto method3 = REFLEX_METHOD(pork3);

  cls.add(method1);
  cls.add(method2);
  cls.add(method3);
  }

QTEST_APPLESS_MAIN(EksReflexTest)
