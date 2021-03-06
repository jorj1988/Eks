#ifndef XCONVERTSCRIPTSTL_H
#define XCONVERTSCRIPTSTL_H

#include <vector>
#include "XConvertFromScript.h"
#include "XConvertToScript.h"
#include "XScriptObject.h"

namespace XScript
{

namespace Convert
{

namespace internal
{

template <>struct JSToNative<std::string>
  {
  typedef std::string ResultType;

  ResultType operator()( Value const & ) const
    {
    /*static const std::string emptyString;
    v8::String::Utf8Value const utf8String( h );
    const char* s = *utf8String;
    return s
      ? std::string(s, utf8String.length())
      : emptyString;*/
    return std::string();
    }
  };

template <> struct JSToNative<std::string const &> : public JSToNative<std::string> {};



template <> struct NativeToJS<std::string>
  {
  Value operator()(std::string const &v) const
    {
    return Value(Eks::String(v));
    }
  };


template <typename ListT> struct NativeToJSList
  {
  Value operator()( ListT const & li ) const
    {
    typedef typename ListT::const_iterator IT;
    IT it = li.begin();
    Value rv = Value::newArray();
    for( int i = 0; li.end() != it; ++it, ++i )
      {
      rv.set(i, Convert::to(*it));
      }
    return rv;
    }
  };

template <typename T> struct NativeToJS<std::list<T> > : NativeToJSList< std::list<T> > {};
template <typename T> struct NativeToJS<std::vector<T> > : NativeToJSList< std::vector<T> > {};
template <typename T> struct NativeToJS<QVector<T> > : NativeToJSList< QVector<T> > {};
template <typename T> struct NativeToJS<QList<T> > : NativeToJSList< QList<T> > {};
template <typename T> struct NativeToJS<Eks::Vector<T> > : NativeToJSList< Eks::Vector<T> > {};
template <> struct NativeToJS<QStringList> : NativeToJSList<QStringList> {};

template <typename MapT> struct NativeToJSLookup
  {
  Value operator()(MapT const &li) const
    {
    typedef typename MapT::const_iterator IT;
    IT it( li.begin() );
    Object rv = Object::newMap();
    for( int i = 0; li.end() != it; ++it, ++i )
      {
      rv.set( (*it).first, Convert::to( (*it).second ) );
      }
    return rv;
    }
  };

template <typename KeyT, typename ValT> struct NativeToJS<std::map<KeyT, ValT> > : NativeToJSLookup<std::map<KeyT,ValT> > {};

template <typename ListT, typename ValueType = typename ListT::value_type> struct JSToNativeList
  {
  typedef ListT ResultType;

  ResultType operator()( Value jv ) const
    {
    typedef ValueType VALT;
    ListT li;
    if( !jv.isValid() || !jv.isArray() ) return li;
    for( xsize i = 0; i < jv.length(); ++i )
      {
      li.push_back( Convert::from<VALT>( jv.at(i)) );
      }
    return li;
    }
  };

template <typename T> struct JSToNative<std::list<T> > : JSToNativeList<std::list<T> > {};
template <typename T> struct JSToNative<std::vector<T> > : JSToNativeList<std::vector<T> > {};

#if 0 // untested code
/**
       UNTESTED!

       Intended as a base class for JSToNative specializations
       which proxy a std::map-like map.
    */
template <typename MapT,
          typename KeyType = typename MapT::mapped_type,
          typename ValueType = typename MapT::value_type>
struct JSToNative_map
  {
  typedef MapT ResultType;
  /**
           Converts jv to a MapT object.

           If jv->IsObject() then the returned object is populated from
           jv, otherwise the returned object is empty. Since it is
           legal for an object to be empty, it is not generically
           possible to know if this routine got an empty Object
           or a non-Object handle.
        */
  ResultType operator()( v8::Handle<v8::Value> jv ) const
    {
    typedef ValueType VALT;
    MapT map;
    if( jv.IsEmpty() || ! jv->IsObject() ) return map;
    Local<Object> obj( Object::Cast(*jv) );
    Local<Array> ar( obj->GetPropertyNames() );
    uint32_t ndx = 0;
    for( ; ar->Has(ndx); ++ndx )
      {
      Local<Value> const & k = ar->Get(Integer::New(ndx));
      if( ! obj->HasRealNamedProperty(k) ) continue;
      map[XScriptConvert::from<KeyType>(k)] = XScriptConvert::from<ValueType>(obj->Get(k));
      }
    return map;
    }
  };
#endif

}

}

}

#endif // XCONVERTSCRIPTSTL_H
