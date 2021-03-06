
set(PROJECT_NAME "RayTracingInOneWeekend")

#---------------------------------------
macro(SETUP_PROJECT MODE TARGET_NAME STR_TARGET_SOURCES STR_TARGET_LIBS)
	string(REPLACE " " ";" LIST_TARGET_SOURCES ${STR_TARGET_SOURCES})
	string(REPLACE " " ";" LIST_TARGET_LIBS ${STR_TARGET_LIBS})
	
	if(${STR_TARGET_SOURCES} STREQUAL " ")
		message(WARNING "Target [${TARGET_NAME}] has no source, so it won't be generated.")
	else()
		if(${MODE} STREQUAL "EXE")
			add_executable( ${TARGET_NAME} ${LIST_TARGET_SOURCES})
			set(INSTALL_DIR "bin")
		elseif(${MODE} STREQUAL "LIB")
			add_library(${TARGET_NAME} ${LIST_TARGET_SOURCES})
			set(INSTALL_DIR "lib/${FOLDER_NAME}")
		else()
			message(FATAL_ERROR "Mode [${MODE}] is not supported, so target [TARGET_NAME] is not generated!")
			set(MODE_NOTSUPPORT " ")
		endif()
		if(NOT DEFINED MODE_NOTSUPPORT)
			if( NOT ${FOLDER_NAME} STREQUAL " ")
				SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES FOLDER ${FOLDER_NAME})
			endif()
			if(NOT ${STR_TARGET_LIBS} STREQUAL " ")
				target_link_libraries( ${TARGET_NAME} ${LIST_TARGET_LIBS} )
			endif()
			install (TARGETS ${TARGET_NAME} DESTINATION ${INSTALL_DIR})
			message(STATUS "Setup Project ${Mode} ${FOLDER_NAME}/[${TARGET_NAME}]")
		endif()
	endif()
endmacro(SETUP_PROJECT TARGET_NAME STR_TARGET_SOURCES STR_TARGET_LIBS)

macro(SETUP_PROJECT_DR MODE TARGET_NAME STR_TARGET_SOURCES STR_TARGET_LIBS_COMMON STR_TARGET_LIBS_DEBUG STR_TARGET_LIBS_RELEASE)
	string(REPLACE " " ";" LIST_TARGET_SOURCES ${STR_TARGET_SOURCES})

	string(REPLACE " " ";" STR_TARGET_LIBS_COMMON ${STR_TARGET_LIBS_COMMON})
	string(REPLACE " " ";" LIST_TARGET_LIBS_COMMON ${STR_TARGET_LIBS_COMMON})

	string(REPLACE " " ";" STR_TARGET_LIBS_DEBUG ${STR_TARGET_LIBS_DEBUG})
	string(REPLACE " " ";" LIST_TARGET_LIBS_DEBUG ${STR_TARGET_LIBS_DEBUG})

	string(REPLACE " " ";" STR_TARGET_LIBS_RELEASE ${STR_TARGET_LIBS_RELEASE})
	string(REPLACE " " ";" LIST_TARGET_LIBS_RELEASE ${STR_TARGET_LIBS_RELEASE})
	
	if(${STR_TARGET_SOURCES} STREQUAL " ")
		message(WARNING "Target [${TARGET_NAME}] has no source, so it won't be generated.")
	else()
		if(${MODE} STREQUAL "EXE")
			add_executable( ${TARGET_NAME} ${LIST_TARGET_SOURCES})
			set(INSTALL_DIR "bin")
		elseif(${MODE} STREQUAL "LIB")
			add_library(${TARGET_NAME} ${LIST_TARGET_SOURCES})
			set(INSTALL_DIR "lib/${FOLDER_NAME}")
		else()
			message(FATAL_ERROR "Mode [${MODE}] is not supported, so target [TARGET_NAME] is not generated!")
			set(MODE_NOTSUPPORT " ")
		endif()
		if(NOT DEFINED MODE_NOTSUPPORT)
			if( NOT ${FOLDER_NAME} STREQUAL " ")
				SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES FOLDER ${FOLDER_NAME})
			endif()
			if(NOT ${STR_TARGET_LIBS_COMMON} STREQUAL " ")
				target_link_libraries( ${TARGET_NAME} ${LIST_TARGET_LIBS_COMMON} )
			endif()
			if(NOT ${STR_TARGET_LIBS_DEBUG} STREQUAL " ")
				target_link_libraries( ${TARGET_NAME} debug ${LIST_TARGET_LIBS_DEBUG} )
			endif()
			if(NOT ${STR_TARGET_LIBS_RELEASE} STREQUAL " ")
				target_link_libraries( ${TARGET_NAME} optimized ${LIST_TARGET_LIBS_RELEASE} )
			endif()
			install (TARGETS ${TARGET_NAME} DESTINATION ${INSTALL_DIR})
			message(STATUS "Setup Project ${Mode} ${FOLDER_NAME}/[${TARGET_NAME}]")
		endif()
	endif()
endmacro(SETUP_PROJECT_DR MODE TARGET_NAME STR_TARGET_SOURCES STR_TARGET_LIBS_COMMON STR_TARGET_LIBS_DEBUG STR_TARGET_LIBS_RELEASE)

#---------------------------------------
macro(ADD_ALL_SUBDIR)
	file(GLOB _children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)
	set(_dirlist "")
	foreach(_child ${_children})
		if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${_child})
			list(APPEND _dirlist ${_child})
		endif()
	endforeach()
	#SET(${result} ${dirlist})
	foreach(_subdir ${_dirlist})
		add_subdirectory(${_subdir})
	endforeach()
endmacro(ADD_ALL_SUBDIR)
#---------------------------------------
macro(GET_DIR_NAME DIRNAME)
	string(REGEX MATCH "([^/]*)$" TMP ${CMAKE_CURRENT_SOURCE_DIR})
	set(${DIRNAME} ${TMP})
endmacro(GET_DIR_NAME DIRNAME)

#---------------------------------------
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
cmake_policy(VERSION 3.1.0)

project (${PROJECT_NAME})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

# The version number.
set (${PROJECT_NAME}_VERSION_MAJOR 1)
set (${PROJECT_NAME}_VERSION_MINOR 0)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_MODULE_PATH
  "${CMAKE_SOURCE_DIR}/CMake"
  ${CMAKE_MODULE_PATH}
)
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

configure_file(config/ROOT_PATH.h.in config/ROOT_PATH.h)

include_directories (
	${PROJECT_SOURCE_DIR}/include
	${CMAKE_BINARY_DIR}/config
	# ...
)
link_directories (
	${PROJECT_SOURCE_DIR}/lib
	# ...
)

add_subdirectory (src)

include (InstallRequiredSystemLibraries)
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set (CPACK_PACKAGE_VERSION_MAJOR "${${PROJECT_NAME}_VERSION_MAJOR}")
set (CPACK_PACKAGE_VERSION_MINOR "${${PROJECT_NAME}_VERSION_MINOR}")
set (CPACK_PACKAGE_CONTACT       "641614112@qq.com")
include (CPack)