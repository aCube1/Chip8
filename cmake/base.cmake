include_guard()

# Generate compile_commands.json for tooling that uses it.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set output directory layout.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Set CMAKE modules path
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules)

# Ensure a CMAKE_BUILD_TYPE is set.
if(NOT CMAKE_BUILD_TYPE)
	message(STATUS "No build type specified. Defaulting to Debug.")
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type." FORCE)
endif()

# Create a list of valid CMAKE_BUILD_TYPES for cmake-gui and ccmake.
set_property(
	CACHE
		CMAKE_BUILD_TYPE
	PROPERTY
		STRINGS
			"Debug"
			"Release"
			"MinSizeRel"
			"RelWithDebInfo"
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_definitions(-DUSE_DEBUG=1)
endif()
