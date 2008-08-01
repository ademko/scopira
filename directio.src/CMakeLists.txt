
PROJECT(scopiradio)

INCLUDE(FindwxWidgets)
FIND_PACKAGE(wxWidgets REQUIRED base xml)

#MESSAGE(${wxWidgets_INCLUDE_DIRS})
#MESSAGE(${wxWidgets_DEFINITIONS})

ADD_LIBRARY(scopiradio STATIC
    scopira/wx/directio.cpp)
I_ADD_RELATIVE_INCLUDE(scopiradio ".")
I_ADD_INCLUDE(scopiradio ${wxWidgets_INCLUDE_DIRS})
I_TARGET_LINK_LIBRARY(scopiradio scopira)
I_ADD_COMPILE_FLAGS(scopiradio ${wxWidgets_DEFINITIONS})
TARGET_LINK_LIBRARIES(scopiradio ${wxWidgets_LIBRARIES})

if(WIN32)
I_ADD_COMPILE_FLAGS(scopiradio /DwxUSE_UNICODE)      # why i need to explicitly set this, i dont know
endif(WIN32)
