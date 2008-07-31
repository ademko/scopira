# - Find Scopira librar(ies)
# This module defines
#  SCOPIRA_DEFINITIONS  definitions scopira needs
#  SCOPIRA_INCLUDE_DIRS  directories that header files needed by scopira
#  SCOPIRA_LIBRARIES, libraries to link against to use SCOPIRA.
#  SCOPIRA_FOUND, If false, do not try to use SCOPIRA.
#  internal-ish things (not for general use)
#  SCOPIRA_INCLUDE_DIR, where to find the header files
#  SCOPIRA_LIBRARY, where to find the SCOPIRA library (core).

include(FindThreads)

include(FindLibXml2)

FIND_LIBRARY(UUID_LIBRARY uuid DOC "libuuid library location")
FIND_LIBRARY(DL_LIBRARY dl DOC "libdl library location")

#SCOPIRA_DEFINITIONS
set(SCOPIRA_DEFINITIONS -DPLATFORM_linux ${LIBXML2_DEFINITIONS})

#SCOPIRA_INCLUDE_DIRS
FIND_PATH(SCOPIRA_INCLUDE_DIR scopira/tool/object.h
  /usr/local/include
  /usr/include
)
set(SCOPIRA_INCLUDE_DIRS ${SCOPIRA_INCLUDE_DIR} ${LIBXML2_INCLUDE_DIR})

#SCOPIRA_LIBRARIES
FIND_LIBRARY(SCOPIRA_LIBRARY scopira
  /usr/lib
  /usr/local/lib
)
set(SCOPIRA_LIBRARIES ${CMAKE_THREAD_LIBS_INIT}
  ${UUID_LIBRARY} ${DL_LIBRARY} ${LIBXML2_LIBRARIES}
  ${SCOPIRA_LIBRARY})

#SCOPIRA_FOUND
IF(SCOPIRA_INCLUDE_DIR AND SCOPIRA_LIBRARY)
  SET( SCOPIRA_FOUND "YES" )
ENDIF(SCOPIRA_INCLUDE_DIR AND SCOPIRA_LIBRARY)

