
# sample include/search/load/find code:
#
#FIND_PATH(SCOPIRA_SRC_PATH CMakeLists.txt DOC "Directory containing the scopira CMakeLists.txt and source code")
#IF(NOT SCOPIRA_SRC_PATH)
#  MESSAGE(FATAL_ERROR "SCOPIRA_SRC_PATH must be set")
#ENDIF(NOT SCOPIRA_SRC_PATH)
#INCLUDE(${SCOPIRA_SRC_PATH}/CMakeLists.txt)

#
# sample library test code
#
#GET_TARGET_PROPERTY(HAS_SCOPIRA scopira TYPE)
#IF(NOT HAS_SCOPIRA)
#  MESSAGE(FATAL_ERROR "This program requires scopira to be included")
#ENDIF(NOT HAS_SCOPIRA)


CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# a project lins is always required, it seems
# under win32, atleast, weird errors are emmited if this is not used
PROJECT(Project)

#BUILD_SHARED_LIBS option
SET(BUILD_SHARED_LIBS "YES" CACHE BOOL "Build shared libraries rather than static ones")

# this is useful is this file is ever INCLUDED by other files
GET_FILENAME_COMPONENT(THIS_PATH ${CMAKE_CURRENT_LIST_FILE} PATH)

#SCOPIRA_LIBXML2_BUILD option
IF(EXISTS ${THIS_PATH}/scopira/tool/xml.h)
  SET(SCOPIRA_LIBXML2_BUILD "NO" CACHE BOOL "Build the libxml2 based Scopira XML library")
IF(SCOPIRA_LIBXML2_BUILD)
INCLUDE(FindLibXml2)
ENDIF(SCOPIRA_LIBXML2_BUILD)
ENDIF(EXISTS ${THIS_PATH}/scopira/tool/xml.h)

#SCOPIRA_GTKUI_BUILD
IF(EXISTS ${THIS_PATH}/scopira/coreui/loop.h AND UNIX)
  INCLUDE(FindPkgConfig)
  pkg_search_module(GTK2 REQUIRED gtk+-2.0 gthread-2.0)
  pkg_search_module(GTHREAD REQUIRED gthread-2.0)
  SET(SCOPIRA_GTKUI_BUILD "NO" CACHE BOOL "Build the (older) GTK+2 based Scopira User Interface library")
ENDIF(EXISTS ${THIS_PATH}/scopira/coreui/loop.h AND UNIX)

#PAKIT/PAUKIT (together)
IF(EXISTS ${THIS_PATH}/pakit/types.h AND UNIX)
  SET(SCOPIRA_PAKIT_BUILD "NO" CACHE BOOL "Build the PAKit (Pattern Analysis) kit")
ENDIF(EXISTS ${THIS_PATH}/pakit/types.h AND UNIX)

set(SCOPIRA_SOURCES 
  ${THIS_PATH}/scopira/agent/agent.cpp
  ${THIS_PATH}/scopira/agent/clusteragent.cpp
  ${THIS_PATH}/scopira/agent/context.cpp
  ${THIS_PATH}/scopira/agent/ids.cpp
  ${THIS_PATH}/scopira/agent/localagent.cpp
  ${THIS_PATH}/scopira/agent/loop.cpp
  ${THIS_PATH}/scopira/agent/monitorservice.cpp
  ${THIS_PATH}/scopira/agent/register.cpp
  ${THIS_PATH}/scopira/agent/task.cpp
  ${THIS_PATH}/scopira/agent/uptimeservice.cpp
  ${THIS_PATH}/scopira/agent/timeslice.cpp
  ${THIS_PATH}/scopira/basekit/color.cpp
  ${THIS_PATH}/scopira/basekit/math.cpp
  ${THIS_PATH}/scopira/basekit/matrixmath.cpp
  ${THIS_PATH}/scopira/basekit/narray.cpp
  ${THIS_PATH}/scopira/basekit/stringvector.cpp
  ${THIS_PATH}/scopira/basekit/vectormath.cpp
  ${THIS_PATH}/scopira/core/loop.cpp
  ${THIS_PATH}/scopira/core/model.cpp
  ${THIS_PATH}/scopira/core/objecttype.cpp
  ${THIS_PATH}/scopira/core/project.cpp
  ${THIS_PATH}/scopira/core/register.cpp
  ${THIS_PATH}/scopira/core/sidekickthread.cpp
  ${THIS_PATH}/scopira/core/view.cpp
  ${THIS_PATH}/scopira/tool/archiveflow.cpp
  ${THIS_PATH}/scopira/tool/binflow.cpp
  ${THIS_PATH}/scopira/tool/bufferflow.cpp
  ${THIS_PATH}/scopira/tool/cacheflow.cpp
  ${THIS_PATH}/scopira/tool/diriterator.cpp
  ${THIS_PATH}/scopira/tool/distflow.cpp
  ${THIS_PATH}/scopira/tool/dll.cpp
  ${THIS_PATH}/scopira/tool/file.cpp
  ${THIS_PATH}/scopira/tool/fileflow.cpp
  ${THIS_PATH}/scopira/tool/flow.cpp
  ${THIS_PATH}/scopira/tool/hash.cpp
  ${THIS_PATH}/scopira/tool/hexflow.cpp
  ${THIS_PATH}/scopira/tool/netflow.cpp
  ${THIS_PATH}/scopira/tool/nullflow.cpp
  ${THIS_PATH}/scopira/tool/object.cpp
  ${THIS_PATH}/scopira/tool/objflowloader.cpp
  ${THIS_PATH}/scopira/tool/output.cpp
  ${THIS_PATH}/scopira/tool/polyflow.cpp
  ${THIS_PATH}/scopira/tool/printflow.cpp
  ${THIS_PATH}/scopira/tool/prop.cpp
  ${THIS_PATH}/scopira/tool/propflow.cpp
  ${THIS_PATH}/scopira/tool/random.cpp
  ${THIS_PATH}/scopira/tool/regex.cpp
  ${THIS_PATH}/scopira/tool/sha1_impl.cpp
  ${THIS_PATH}/scopira/tool/stringflow.cpp
  ${THIS_PATH}/scopira/tool/textflow.cpp
  ${THIS_PATH}/scopira/tool/thread.cpp
  ${THIS_PATH}/scopira/tool/time.cpp
  ${THIS_PATH}/scopira/tool/tree.cpp
  ${THIS_PATH}/scopira/tool/url.cpp
  ${THIS_PATH}/scopira/tool/util.cpp
  ${THIS_PATH}/scopira/tool/uuid.cpp
  )

#include(FindThreads)
INCLUDE(${THIS_PATH}/FindPthreads.cmake)

IF(NOT WIN32)
FIND_LIBRARY(UUID_LIBRARY uuid DOC "libuuid library location")
#FIND_LIBRARY(DL_LIBRARY dl DOC "libdl library location")
ENDIF(NOT WIN32)

# scopira library
ADD_LIBRARY(scopira ${SCOPIRA_SOURCES})

IF(WIN32)
  ADD_DEFINITIONS(-DPLATFORM_win32)
  ADD_DEFINITIONS(/w44018)    #disable sign mismatch warning
  ADD_DEFINITIONS(-D_SECURE_SCL=0)    #disable check iterators
  IF(NOT BUILD_SHARED_LIBS)
    #this should be a flag (like scopira_EXPORTS provided by cmake, IMHO)
    ADD_DEFINITIONS(-Dscopira_STATIC)
  ENDIF(NOT BUILD_SHARED_LIBS)
  TARGET_LINK_LIBRARIES(scopira Ws2_32)   #add socket library
else(WIN32)
  ADD_DEFINITIONS(-DPLATFORM_linux)
  ADD_DEFINITIONS(-Wall -Wno-sign-compare -Wwrite-strings)
ENDIF(WIN32)

INCLUDE_DIRECTORIES(${THIS_PATH} ${PTHREADS_INCLUDE_DIR})
IF(CMAKE_SIZEOF_VOID_P GREATER 4)
  ADD_DEFINITIONS(-DPLATFORM_64)
ENDIF(CMAKE_SIZEOF_VOID_P GREATER 4)

TARGET_LINK_LIBRARIES(scopira
  ${CMAKE_THREAD_LIBS_INIT}
  ${UUID_LIBRARY}
  ${CMAKE_DL_LIBS}
  ${PTHREADS_LIBRARY})

#scopira agent executable
ADD_EXECUTABLE(scopiraagent
  ${THIS_PATH}/scopira/agent/agent_mainline.cpp )
TARGET_LINK_LIBRARIES(scopiraagent scopira)

#scopiraxml libxml2 based
IF(SCOPIRA_LIBXML2_BUILD)
  ADD_LIBRARY(scopiraxml
      ${THIS_PATH}/scopira/tool/xml.cpp
      ${THIS_PATH}/scopira/basekit/directio.cpp)
  ADD_DEFINITIONS(${LIBXML2_DEFINITIONS} -I${LIBXML2_INCLUDE_DIR})
  TARGET_LINK_LIBRARIES(scopiraxml scopira ${LIBXML2_LIBRARIES})
ENDIF(SCOPIRA_LIBXML2_BUILD)

#scopiraui gtk+2 based
IF(SCOPIRA_GTKUI_BUILD)
  IF(NOT SCOPIRA_LIBXML2_BUILD)
    MESSAGE(FATAL_ERROR "SCOPIRA_GTKUI_BUILD requires that SCOPIRA_LIBXML2_BUILD be ON")
  ENDIF(NOT SCOPIRA_LIBXML2_BUILD)
  ADD_LIBRARY(scopiraui
    ${THIS_PATH}/scopira/coreui/aboutwindow.cpp
    ${THIS_PATH}/scopira/coreui/app.cpp
    ${THIS_PATH}/scopira/coreui/button.cpp
    ${THIS_PATH}/scopira/coreui/canvascommon.cpp
    ${THIS_PATH}/scopira/coreui/canvas.cpp
    ${THIS_PATH}/scopira/coreui/checkbutton.cpp
    ${THIS_PATH}/scopira/coreui/dropdown.cpp
    ${THIS_PATH}/scopira/coreui/entry.cpp
    ${THIS_PATH}/scopira/coreui/filewindow.cpp
    ${THIS_PATH}/scopira/coreui/image.cpp
    ${THIS_PATH}/scopira/coreui/label.cpp
    ${THIS_PATH}/scopira/coreui/layout.cpp
    ${THIS_PATH}/scopira/coreui/listbox.cpp
    ${THIS_PATH}/scopira/coreui/loop.cpp
    ${THIS_PATH}/scopira/coreui/menu.cpp
    ${THIS_PATH}/scopira/coreui/messagewindow.cpp
    ${THIS_PATH}/scopira/coreui/output.cpp
    ${THIS_PATH}/scopira/coreui/progressbar.cpp
    ${THIS_PATH}/scopira/coreui/project.cpp
    ${THIS_PATH}/scopira/coreui/radiobutton.cpp
    ${THIS_PATH}/scopira/coreui/slider.cpp
    ${THIS_PATH}/scopira/coreui/spinbutton.cpp
    ${THIS_PATH}/scopira/coreui/viewcontainer.cpp
    ${THIS_PATH}/scopira/coreui/viewmenu.cpp
    ${THIS_PATH}/scopira/coreui/viewwidget.cpp
    ${THIS_PATH}/scopira/coreui/widget.cpp
    ${THIS_PATH}/scopira/coreui/window.cpp
    ${THIS_PATH}/scopira/coreui/xpmdata.cpp
    ${THIS_PATH}/scopira/lab/config.cpp
    ${THIS_PATH}/scopira/lab/dialog.cpp
    ${THIS_PATH}/scopira/lab/loop.cpp
    ${THIS_PATH}/scopira/lab/output.cpp
    ${THIS_PATH}/scopira/lab/sidekickloop.cpp
    ${THIS_PATH}/scopira/lab/view.cpp
    ${THIS_PATH}/scopira/lab/window.cpp
    ${THIS_PATH}/scopira/uikit/agentsmonitor.cpp
    ${THIS_PATH}/scopira/uikit/basic.cpp
    ${THIS_PATH}/scopira/uikit/canvasplot.cpp
    ${THIS_PATH}/scopira/uikit/color.cpp
    ${THIS_PATH}/scopira/uikit/cpuentry.cpp
    ${THIS_PATH}/scopira/uikit/directio.cpp
    ${THIS_PATH}/scopira/uikit/joystick.cpp
    ${THIS_PATH}/scopira/uikit/legend.cpp
    ${THIS_PATH}/scopira/uikit/matrix.cpp
    ${THIS_PATH}/scopira/uikit/models.cpp
    ${THIS_PATH}/scopira/uikit/plotter.cpp
    ${THIS_PATH}/scopira/uikit/seedentry.cpp
    ${THIS_PATH}/scopira/uikit/tiledlayout.cpp
    ${THIS_PATH}/scopira/uikit/vcrbutton.cpp
  )
  ADD_DEFINITIONS(${GTK2_CFLAGS} ${GTHREAD_CFLAGS})
  TARGET_LINK_LIBRARIES(scopiraui scopiraxml)
  TARGET_LINK_LIBRARIES(scopiraui ${GTK2_LIBRARIES} ${GTHREAD_LIBRARIES})

  ADD_EXECUTABLE(scopiraapp
    ${THIS_PATH}/scopira/uikit/appmainline.cpp)
  TARGET_LINK_LIBRARIES(scopiraapp scopiraui)
ENDIF(SCOPIRA_GTKUI_BUILD)

#pakit 
IF(SCOPIRA_PAKIT_BUILD)
  IF(NOT SCOPIRA_LIBXML2_BUILD OR NOT SCOPIRA_GTKUI_BUILD)
    MESSAGE(FATAL_ERROR "SCOPIRA_PAKIT_BUILD requires both SCOPIRA_LIBXML2_BUILD and SCOPIRA_GTKUI_BUILD to be ON")
  ENDIF(NOT SCOPIRA_LIBXML2_BUILD OR NOT SCOPIRA_GTKUI_BUILD)
  ADD_LIBRARY(pakit
      ${THIS_PATH}/pakit/distances.cpp
      ${THIS_PATH}/pakit/rdp.cpp
      ${THIS_PATH}/pakit/types.cpp
      ${THIS_PATH}/pakit/util.cpp
      )
  TARGET_LINK_LIBRARIES(pakit scopiraxml)
  ADD_LIBRARY(pauikit
      ${THIS_PATH}/pakit/ui/classifierstats.cpp
      ${THIS_PATH}/pakit/ui/distances.cpp
      ${THIS_PATH}/pakit/ui/filters.cpp
      ${THIS_PATH}/pakit/ui/funcs.cpp
      ${THIS_PATH}/pakit/ui/lda.cpp
      ${THIS_PATH}/pakit/ui/models.cpp
      ${THIS_PATH}/pakit/ui/patterneditor.cpp
      ${THIS_PATH}/pakit/ui/patternsort.cpp
      ${THIS_PATH}/pakit/ui/plot.cpp
      ${THIS_PATH}/pakit/ui/printclassnstats.cpp
      ${THIS_PATH}/pakit/ui/rdp.cpp
      )
  TARGET_LINK_LIBRARIES(pauikit pakit)
  TARGET_LINK_LIBRARIES(pauikit scopiraui)
ENDIF(SCOPIRA_PAKIT_BUILD)

