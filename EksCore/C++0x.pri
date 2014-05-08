win32-g++ {
  QMAKE_CXXFLAGS += -std=gnu++0x
  DEFINES += X_CPPOX_SUPPORT X_CPPOX_VARIADIC_TEMPLATES_SUPPORT QT_NO_KEYWORDS
}

linux-g++ {
  QMAKE_CXXFLAGS += -std=gnu++0x
  DEFINES += X_CPPOX_SUPPORT X_CPPOX_VARIADIC_TEMPLATES_SUPPORT QT_NO_KEYWORDS
}

macx-clang {
  QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
  QMAKE_CXXFLAGS += -std=c++11 -stdlib=libc++

  LIBS += -mmacosx-version-min=10.7 -stdlib=libc++
  DEFINES += X_CPPOX_SUPPORT X_CPPOX_VARIADIC_TEMPLATES_SUPPORT QT_NO_KEYWORDS X_HAS_LONG_LONG
}

linux-clang {
  QMAKE_CXXFLAGS += -std=c++11 -stdlib=libc++
  DEFINES += X_CPPOX_SUPPORT X_CPPOX_VARIADIC_TEMPLATES_SUPPORT QT_NO_KEYWORDS
}

linux-g++-64 {
  QMAKE_CXXFLAGS += -std=gnu++0x
  DEFINES += X_CPPOX_SUPPORT X_CPPOX_VARIADIC_TEMPLATES_SUPPORT QT_NO_KEYWORDS
}

win32-msvc2010 {
  DEFINES += X_CPPOX_SUPPORT X_CPPOX_OVERRIDE_SUPPORT X_HAS_LONG_LONG
}

win32-msvc2012|win32-arm-msvc2012 {
  DEFINES += X_CPPOX_SUPPORT X_CPPOX_OVERRIDE_SUPPORT X_HAS_LONG_LONG
}

HEADERS +=



