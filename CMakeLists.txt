set (PROJECT_OUTPUT_NAME SimiconUpdater)
project(${PROJECT_OUTPUT_NAME})

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
src/core/defaultversionnumbering.cpp
src/core/productversion.cpp
src/core/abstractupdateconfig.cpp
src/core/abstractupdater.cpp
src/core/qtxmlupdateconfig.cpp
src/core/qtwebupdater.cpp
)
set (src_core_HEADERS
src/core/abstractupdater.h
src/core/qtwebupdater.h
)

################################################################
# all
################################################################
set(
SOURCES
${src_SRC}
${src_core_SRC}
)

set(
HEADERS
${src_HEADERS}
${src_core_HEADERS}
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
