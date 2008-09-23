
GET_TARGET_PROPERTY(HAS_SCOPIRA scopira TYPE)
IF(NOT HAS_SCOPIRA)
  MESSAGE(FATAL_ERROR "The directio library requires scopira to be included")
ENDIF(NOT HAS_SCOPIRA)

INCLUDE(FindwxWidgets)
FIND_PACKAGE(wxWidgets REQUIRED base xml)

# this is useful is this file is ever INCLUDED by other files
GET_FILENAME_COMPONENT(THIS_PATH ${CMAKE_CURRENT_LIST_FILE} PATH)

ADD_LIBRARY(scopiradio STATIC
    ${THIS_PATH}/scopira/wx/directio.cpp)

INCLUDE_DIRECTORIES(${THIS_PATH})

INCLUDE_DIRECTORIES(${wxWidgets_INCLUDE_DIRS})
ADD_DEFINITIONS(${wxWidgets_DEFINITIONS})
IF(WIN32)
ADD_DEFINITIONS(/DwxUSE_UNICODE)      # why i need to explicitly set this, i dont know
ENDIF(WIN32)
TARGET_LINK_LIBRARIES(scopiradio ${wxWidgets_LIBRARIES})

TARGET_LINK_LIBRARIES(scopiradio scopira)

