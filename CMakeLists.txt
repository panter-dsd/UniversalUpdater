set (PROJECT_OUTPUT_NAME uu)
project(UU)

cmake_minimum_required( VERSION 2.6 )

INCLUDE(FindQt4)
find_package( Qt4 REQUIRED )
SET(QT_USE_QTXML 1)
SET(QT_USE_QTNETWORK 1)
include( ${QT_USE_FILE})

add_definitions(-DUNICODE)

include_directories(
src
src/core
src/gui
${CMAKE_CURRENT_BINARY_DIR}
)

################################################################
# src
################################################################

set (src_SRC
src/main.cpp
)
set (src_HEADERS
)

################################################################
# src/core
################################################################

set (src_core_SRC
src/core/core.cpp
src/core/defaultversionnumbering.cpp
src/core/productversion.cpp
src/core/abstractupdateconfig.cpp
src/core/abstractupdater.cpp
src/core/updateconfigfactory.cpp
src/core/xmlupdateconfig.cpp
src/core/webupdater.cpp
src/core/updaterfactory.cpp
src/core/updateschecker.cpp
src/core/configloader.cpp
src/core/settingschangechecker.cpp
src/core/updatesmodel.cpp
)

set (src_core_HEADERS
src/core/abstractupdater.h
src/core/webupdater.h
src/core/updateschecker.h
src/core/configloader.h
src/core/settingschangechecker_p.h
src/core/settingschangechecker.h
src/core/updatesmodel.h
)

################################################################
# src/gui
################################################################

set (src_gui_SRC
src/gui/mainwindow.cpp
src/gui/updaterwidget.cpp
src/gui/updatepreferencewidget.cpp
src/gui/preferencesdialog.cpp
src/gui/updatedownloaddialog.cpp
)

set (src_gui_HEADERS
src/gui/mainwindow.h
src/gui/updaterwidget.h
src/gui/abstractpreferencewidget.h
src/gui/updatepreferencewidget.h
src/gui/preferencesdialog.h
src/gui/updatedownloaddialog.h
)

set (src_gui_UI
src/gui/mainwindow.ui
src/gui/updaterwidget.ui
src/gui/updatepreferencewidget.ui
src/gui/preferencesdialog.ui
src/gui/updatedownloaddialog.ui
)

################################################################
# all
################################################################
set(
	SOURCES
	${src_SRC}
	${src_core_SRC}
	${src_gui_SRC}
)

set(
	HEADERS
	${src_HEADERS}
	${src_core_HEADERS}
	${src_gui_HEADERS}
)

set(
	UI
	${src_gui_UI}
)

qt4_wrap_ui( UI_HEADERS ${UI} )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )

set( RESOURCES resource.qrc )

qt4_add_resources( QRC_SOURCES ${RESOURCES} )

if(MINGW)
  ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/rc.o
  COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR}
  -i${CMAKE_CURRENT_SOURCE_DIR}/resource.rc
   -o ${CMAKE_CURRENT_BINARY_DIR}/rc.o)
	SET(rc_RC ${rc_RC} ${CMAKE_CURRENT_BINARY_DIR}/rc.o)
else(MINGW)
	SET(rc_RC ${rc_RC} resource.rc)
endif(MINGW)

if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
	add_executable( ${PROJECT_OUTPUT_NAME} ${SOURCES} ${MOC_SOURCES} ${QRC_SOURCES} ${UI_HEADERS} ${rc_RC})
else()
	add_executable( ${PROJECT_OUTPUT_NAME} WIN32 ${SOURCES} ${MOC_SOURCES} ${QRC_SOURCES} ${UI_HEADERS} ${rc_RC})
endif()

target_link_libraries( ${PROJECT_OUTPUT_NAME} ${QT_LIBRARIES})
