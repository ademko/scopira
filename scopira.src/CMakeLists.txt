
#
# Example use:
#   (if your CMakelists.txt is the primary file)
#
# Add this to the top of your CMakeLists.txt:
#   SET(SCOPIRA_SRC_DIR CACHE PATH "Path to the Scopira source root directory")
#   ADD_SUBDIRECTORY(${SCOPIRA_SRC_DIR}/scopira.src build_scopira)
#   ADD_SUBDIRECTORY(${SCOPIRA_SRC_DIR}/directio.src build_directio)    # optional, etc
#
# Then do:
#   ADD_EXECUTABLE(yourtarget files.cpp)
#   I_TARGET_LINK_LIBRARY(yourtarget scopira)   # or scopiraxml, etc
#
# Example use:
#   (if your CMakeLists.txt is just a secondardy file)
#
#   ADD_LIBRARY(somelib files.cpp)
#   I_TARGET_LINK_LIBRARY(somelib scopiraui)
#   I_ADD_RELATIVE_INCLUDE(somelib .)
#
# Then in the master file:
#   ADD_SUBDIRECTORY(path_to/scopira.src build_scopira)
#   ADD_SUBDIRECTORY(somelib.src build_somelib)


PROJECT(scopira)

#BUILD_SHARED_LIBS option
SET(BUILD_SHARED_LIBS "YES" CACHE BOOL "Build shared libraries rather than static ones")

#SCOPIRA_LIBXML2_BUILD option
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/scopira/tool/xml.h)
  INCLUDE(FindLibXml2)
  SET(SCOPIRA_LIBXML2_BUILD ${LIBXML2_FOUND} CACHE BOOL "Build the libxml2 based scopira XML library")
endif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/scopira/tool/xml.h)

#SCOPIRA_GTKUI_BUILD
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/scopira/coreui/loop.h AND UNIX)
  INCLUDE(FindPkgConfig)
  pkg_search_module(GTK2 gtk+-2.0 gthread-2.0)
  pkg_search_module(GTHREAD gthread-2.0)
  SET(SCOPIRA_GTKUI_BUILD ${GTK2_FOUND} CACHE BOOL "Build the (older) GTK+2 based scopira User Interface library")
endif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/scopira/coreui/loop.h AND UNIX)

#PAKIT/PAUKIT (together)
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/pakit/types.h AND UNIX)
  SET(SCOPIRA_PAKIT_BUILD "YES" CACHE BOOL "Build the PAKit (Pattern Analysis) kit")
endif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/pakit/types.h AND UNIX)

#
# Utility Stuff
#
MACRO(I_ADD_COMPILE_FLAGS TARGET)
  GET_TARGET_PROPERTY(C ${TARGET} COMPILE_FLAGS)
  string(REGEX REPLACE ";" " " ARGN_NEW "${ARGN}")
  if(C)
    SET_TARGET_PROPERTIES(${TARGET} PROPERTIES COMPILE_FLAGS "${C} ${ARGN_NEW}")
  else(C)
    SET_TARGET_PROPERTIES(${TARGET} PROPERTIES COMPILE_FLAGS "${ARGN_NEW}")
  endif(C)
  GET_TARGET_PROPERTY(C ${TARGET} COMPILE_FLAGS)
ENDMACRO(I_ADD_COMPILE_FLAGS)

MACRO(I_ADD_INCLUDE TARGET)
  foreach(DIR ${ARGN})
    I_ADD_COMPILE_FLAGS(${TARGET} -I${DIR})
  endforeach(DIR)
ENDMACRO(I_ADD_INCLUDE)

MACRO(I_ADD_RELATIVE_INCLUDE TARGET RELDIR)
  I_ADD_COMPILE_FLAGS(${TARGET} -I${CMAKE_CURRENT_SOURCE_DIR}/${RELDIR})
ENDMACRO(I_ADD_RELATIVE_INCLUDE)

MACRO(I_TARGET_LINK_LIBRARY TARGET SRCTARGET)
  GET_TARGET_PROPERTY(C ${SRCTARGET} TYPE)
  if(C)
    GET_TARGET_PROPERTY(C ${SRCTARGET} COMPILE_FLAGS)
    if(C)
      GET_TARGET_PROPERTY(ORIG ${TARGET} COMPILE_FLAGS)
      if(ORIG)
        SET(C "${ORIG} ${C}")
      endif(ORIG)
      SET_TARGET_PROPERTIES(${TARGET} PROPERTIES COMPILE_FLAGS ${C})
    endif(C)
    TARGET_LINK_LIBRARIES(${TARGET} ${SRCTARGET})
  else(C)
    MESSAGE(FATAL_ERROR "I_TARGET_LINK_LIBRARY missing library: " ${SRCTARGET})
  endif(C)
ENDMACRO(I_TARGET_LINK_LIBRARY)

set(SCOPIRA_SOURCES 
  scopira/agent/agent.cpp
  scopira/agent/clusteragent.cpp
  scopira/agent/context.cpp
  scopira/agent/ids.cpp
  scopira/agent/localagent.cpp
  scopira/agent/loop.cpp
  scopira/agent/monitorservice.cpp
  scopira/agent/register.cpp
  scopira/agent/task.cpp
  scopira/agent/uptimeservice.cpp
  scopira/basekit/color.cpp
  scopira/basekit/math.cpp
  scopira/basekit/matrixmath.cpp
  scopira/basekit/narray.cpp
  scopira/basekit/stringvector.cpp
  scopira/basekit/vectormath.cpp
  scopira/core/loop.cpp
  scopira/core/model.cpp
  scopira/core/objecttype.cpp
  scopira/core/project.cpp
  scopira/core/register.cpp
  scopira/core/sidekickthread.cpp
  scopira/core/view.cpp
  scopira/tool/archiveflow.cpp
  scopira/tool/binflow.cpp
  scopira/tool/bufferflow.cpp
  scopira/tool/cacheflow.cpp
  scopira/tool/diriterator.cpp
  scopira/tool/distflow.cpp
  scopira/tool/dll.cpp
  scopira/tool/file.cpp
  scopira/tool/fileflow.cpp
  scopira/tool/flow.cpp
  scopira/tool/hash.cpp
  scopira/tool/hexflow.cpp
  scopira/tool/netflow.cpp
  scopira/tool/nullflow.cpp
  scopira/tool/object.cpp
  scopira/tool/objflowloader.cpp
  scopira/tool/output.cpp
  scopira/tool/polyflow.cpp
  scopira/tool/printflow.cpp
  scopira/tool/prop.cpp
  scopira/tool/propflow.cpp
  scopira/tool/random.cpp
  scopira/tool/regex.cpp
  scopira/tool/sha1_impl.cpp
  scopira/tool/stringflow.cpp
  scopira/tool/textflow.cpp
  scopira/tool/thread.cpp
  scopira/tool/time.cpp
  scopira/tool/tree.cpp
  scopira/tool/url.cpp
  scopira/tool/util.cpp
  scopira/tool/uuid.cpp
  )

#include(FindThreads)
INCLUDE(./FindPthreads.cmake)

if(NOT WIN32)
FIND_LIBRARY(UUID_LIBRARY uuid DOC "libuuid library location")
#FIND_LIBRARY(DL_LIBRARY dl DOC "libdl library location")
endif(NOT WIN32)

# scopira library
ADD_LIBRARY(scopira ${SCOPIRA_SOURCES} )

if(WIN32)
  I_ADD_COMPILE_FLAGS(scopira -DPLATFORM_win32)
  I_ADD_COMPILE_FLAGS(scopira /w44018)    #disable sign mismatch warning
  I_ADD_COMPILE_FLAGS(scopira -D_SECURE_SCL=0)    #disable check iterators
  if(NOT BUILD_SHARED_LIBS)
    #this should be a flag (like scopira_EXPORTS provided by cmake, IMHO
    I_ADD_COMPILE_FLAGS(scopira -Dscopira_STATIC)
  endif(NOT BUILD_SHARED_LIBS)
  TARGET_LINK_LIBRARIES(scopira Ws2_32)   #add socket library
else(WIN32)
  I_ADD_COMPILE_FLAGS(scopira -DPLATFORM_linux)
  I_ADD_COMPILE_FLAGS(scopira -Wall -Wno-sign-compare -Wwrite-strings)
endif(WIN32)

I_ADD_COMPILE_FLAGS(scopira
  -I${CMAKE_CURRENT_SOURCE_DIR}
  -I${PTHREADS_INCLUDE_DIR})
if(${CMAKE_SIZEOF_VOID_P} GREATER 4)
  I_ADD_COMPILE_FLAGS(scopira -DPLATFORM_64)
endif(${CMAKE_SIZEOF_VOID_P} GREATER 4)
TARGET_LINK_LIBRARIES(scopira ${CMAKE_THREAD_LIBS_INIT} ${UUID_LIBRARY}
  ${CMAKE_DL_LIBS}
  ${PTHREADS_LIBRARY})

#scopira agent executable
ADD_EXECUTABLE(scopiraagent
  scopira/agent/agent_mainline.cpp )
I_TARGET_LINK_LIBRARY(scopiraagent scopira)

#scopiraxml libxml2 based
if(SCOPIRA_LIBXML2_BUILD)
  ADD_LIBRARY(scopiraxml
      scopira/tool/xml.cpp
      scopira/basekit/directio.cpp)
  I_TARGET_LINK_LIBRARY(scopiraxml scopira)
  I_ADD_COMPILE_FLAGS(scopiraxml ${LIBXML2_DEFINITIONS} -I${LIBXML2_INCLUDE_DIR})
  TARGET_LINK_LIBRARIES(scopiraxml ${LIBXML2_LIBRARIES})
endif(SCOPIRA_LIBXML2_BUILD)

#scopiraui gtk+2 based
if(SCOPIRA_GTKUI_BUILD)
  if(NOT SCOPIRA_LIBXML2_BUILD)
    MESSAGE(FATAL_ERROR "SCOPIRA_GTKUI_BUILD requires that SCOPIRA_LIBXML2_BUILD be ON")
  endif(NOT SCOPIRA_LIBXML2_BUILD)
  ADD_LIBRARY(scopiraui
    scopira/coreui/aboutwindow.cpp
    scopira/coreui/app.cpp
    scopira/coreui/button.cpp
    scopira/coreui/canvascommon.cpp
    scopira/coreui/canvas.cpp
    scopira/coreui/checkbutton.cpp
    scopira/coreui/dropdown.cpp
    scopira/coreui/entry.cpp
    scopira/coreui/filewindow.cpp
    scopira/coreui/image.cpp
    scopira/coreui/label.cpp
    scopira/coreui/layout.cpp
    scopira/coreui/listbox.cpp
    scopira/coreui/loop.cpp
    scopira/coreui/menu.cpp
    scopira/coreui/messagewindow.cpp
    scopira/coreui/output.cpp
    scopira/coreui/progressbar.cpp
    scopira/coreui/project.cpp
    scopira/coreui/radiobutton.cpp
    scopira/coreui/slider.cpp
    scopira/coreui/spinbutton.cpp
    scopira/coreui/viewcontainer.cpp
    scopira/coreui/viewmenu.cpp
    scopira/coreui/viewwidget.cpp
    scopira/coreui/widget.cpp
    scopira/coreui/window.cpp
    scopira/coreui/xpmdata.cpp
    scopira/lab/config.cpp
    scopira/lab/dialog.cpp
    scopira/lab/loop.cpp
    scopira/lab/output.cpp
    scopira/lab/sidekickloop.cpp
    scopira/lab/view.cpp
    scopira/lab/window.cpp
    scopira/uikit/agentsmonitor.cpp
    scopira/uikit/basic.cpp
    scopira/uikit/canvasplot.cpp
    scopira/uikit/color.cpp
    scopira/uikit/cpuentry.cpp
    scopira/uikit/directio.cpp
    scopira/uikit/joystick.cpp
    scopira/uikit/legend.cpp
    scopira/uikit/matrix.cpp
    scopira/uikit/models.cpp
    scopira/uikit/plotter.cpp
    scopira/uikit/seedentry.cpp
    scopira/uikit/tiledlayout.cpp
    scopira/uikit/vcrbutton.cpp
  )
  I_TARGET_LINK_LIBRARY(scopiraui scopiraxml)
  I_ADD_COMPILE_FLAGS(scopiraui ${GTK2_CFLAGS} ${GTHREAD_CFLAGS})
  TARGET_LINK_LIBRARIES(scopiraui ${GTK2_LIBRARIES} ${GTHREAD_LIBRARIES})

  ADD_EXECUTABLE(scopiraapp
    scopira/uikit/appmainline.cpp)
  I_TARGET_LINK_LIBRARY(scopiraapp scopiraui)
endif(SCOPIRA_GTKUI_BUILD)

#pakit 
if(SCOPIRA_PAKIT_BUILD)
  if(NOT SCOPIRA_LIBXML2_BUILD OR NOT SCOPIRA_GTKUI_BUILD)
    MESSAGE(FATAL_ERROR "SCOPIRA_PAKIT_BUILD requires both SCOPIRA_LIBXML2_BUILD and SCOPIRA_GTKUI_BUILD to be ON")
  endif(NOT SCOPIRA_LIBXML2_BUILD OR NOT SCOPIRA_GTKUI_BUILD)
  ADD_LIBRARY(pakit
      pakit/distances.cpp
      pakit/rdp.cpp
      pakit/types.cpp
      pakit/util.cpp
      )
  I_TARGET_LINK_LIBRARY(pakit scopiraxml)
  ADD_LIBRARY(pauikit
      pakit/ui/classifierstats.cpp
      pakit/ui/distances.cpp
      pakit/ui/filters.cpp
      pakit/ui/funcs.cpp
      pakit/ui/lda.cpp
      pakit/ui/models.cpp
      pakit/ui/patterneditor.cpp
      pakit/ui/patternsort.cpp
      pakit/ui/plot.cpp
      pakit/ui/printclassnstats.cpp
      pakit/ui/rdp.cpp
      )
  I_TARGET_LINK_LIBRARY(pauikit pakit)
  I_TARGET_LINK_LIBRARY(pauikit scopiraui)
endif(SCOPIRA_PAKIT_BUILD)

