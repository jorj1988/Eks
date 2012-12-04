win32-g++ {
  QMAKE_CXXFLAGS += -std=gnu++0x
  DEFINES += X_CPPOX_SUPPORT X_CPPOX_VARIADIC_TEMPLATES_SUPPORT QT_NO_KEYWORDS
}

linux-g++ {
  QMAKE_CXXFLAGS += -std=gnu++0x
  DEFINES += X_CPPOX_SUPPORT X_CPPOX_VARIADIC_TEMPLATES_SUPPORT QT_NO_KEYWORDS
}

macx-g++ {
  QMAKE_CXXFLAGS += -std=gnu++0x
  DEFINES += X_CPPOX_SUPPORT X_CPPOX_VARIADIC_TEMPLATES_SUPPORT QT_NO_KEYWORDS
}

win32-msvc2010 {
  DEFINES += X_CPPOX_SUPPORT X_CPPOX_OVERRIDE_SUPPORT X_HAS_LONG_LONG
}

win32-arm-msvc2012 {
  DEFINES += X_CPPOX_SUPPORT X_CPPOX_OVERRIDE_SUPPORT X_HAS_LONG_LONG
}







