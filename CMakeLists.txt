cmake_minimum_required(VERSION 2.8)

project(GenPi)

message(STATUS "Updating Git Submodules")
execute_process(
	COMMAND git submodule update --init --recursive
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)

SET(FLOAT_PRECISION "single" CACHE STRING "Floating-point precision (single or double)")
SET(FASTMATH OFF CACHE BOOL "Use approximate math functions")

find_path(RTAUDIO_INCLUDES "RtAudio.h")
if (RTAUDIO_INCLUDES)
	file(STRINGS "${RTAUDIO_INCLUDES}/RtAudio.h" RTAUDIO_VERSION_LINE REGEX RTAUDIO_VERSION)
	string(REGEX REPLACE "^.*\"(.*)\".*$" "\\1" RTAUDIO_VERSION  "${RTAUDIO_VERSION_LINE}")
	string(COMPARE LESS "${RTAUDIO_VERSION}" "4.1.0" RTAUDIO_OLD_VERSION)
	if (RTAUDIO_OLD_VERSION EQUAL 1)
		message(STATUS "RtAudio version ${RTAUDIO_VERSION} is too old. Redefining RtAudioError as RtError")
		add_definitions(-DRtAudioError=RtError)
	endif()
endif()

find_path(RTMIDI_INCLUDES "RtMidi.h")
if (RTMIDI_INCLUDES)
	file(STRINGS "${RTMIDI_INCLUDES}/RtMidi.h" RTMIDI_VERSION_LINE REGEX RTMIDI_VERSION)
	string(REGEX REPLACE "^.*\"(.*)\".*$" "\\1" RTMIDI_VERSION  "${RTMIDI_VERSION_LINE}")
	string(COMPARE LESS "${RTMIDI_VERSION}" "2.1.0" RTMIDI_OLD_VERSION)
	if (RTMIDI_OLD_VERSION EQUAL 1)
		message(STATUS "RtMidi version ${RTMIDI_VERSION} is too old. Redefining RtMidiError as RtError")
		add_definitions(-DRtMidiError=RtError)
	endif()
endif()

include_directories(
	"${PROJECT_SOURCE_DIR}/include"
	"${PROJECT_SOURCE_DIR}/dependencies/gen"
	"${PROJECT_SOURCE_DIR}/dependencies/jsonxx"
	"${RTAUDIO_INCLUDES}"
	"${RTMIDI_INCLUDES}"
)

# file(GLOB_RECURSE PROJECT_HEADERS

# )

if (APPLE)
	SET(CMAKE_C_COMPILER clang)
	SET(CMAKE_CXX_COMPILER clang++)
	set(CMAKE_CXX_FLAGS "-std=c++1y -stdlib=libc++")
else(NOT WIN32)
	SET(CMAKE_C_COMPILER gcc)
	SET(CMAKE_CXX_COMPILER g++)
	set(CMAKE_CXX_FLAGS "-std=c++1y")
endif()

if (${FLOAT_PRECISION} MATCHES "single")
	message(STATUS "Using single-precision floating-point math")
	add_definitions(-DGEN_USE_FLOAT32)
elseif (${FLOAT_PRECISION} MATCHES "double")
	message(STATUS "Using double-precision floating-point math")
else()
	message(FATAL_ERROR "Bad floating-point precision value. Aborting")
endif()
if (FASTMATH)
message(STATUS "Using approximate math functions")
add_definitions(-DGEN_USE_FASTMATH)
endif()

if (NOT APPLE)
	execute_process(COMMAND grep -o BCM2708 /proc/cpuinfo OUTPUT_VARIABLE RASPI)
	execute_process(COMMAND grep -o BCM2709 /proc/cpuinfo OUTPUT_VARIABLE RASPI2) # could also be RASPI3
	if (RASPI OR RASPI2)
		message(STATUS "defining RASPI")
		add_definitions(-DRASPI)
		set(RASPI_FILES "dependencies/rotaryencoder/rotaryencoder.c")
		include_directories(
			"${PROJECT_SOURCE_DIR}/dependencies/rotaryencoder"
		)
		find_library(WIRINGPI_LIB NAMES wiringPi)
		if (NOT WIRINGPI_LIB)
			message("\tNot using WiringPi (`apt-get install wiringpi` if you want to)")
		else()
			add_definitions(-DWIRINGPI)
			message("\twiringPi: Don't forget to run ./GenPi as sudo")
		endif()
	endif()
endif()

add_executable(	${PROJECT_NAME}
				include/GenPi.h
				include/RtAudioMidiHost.h
				include/Processor.h
				include/Host.h
				include/Object.h
				include/Settings.h
				include/AudioMidiHost.h
				include/ParameterHost.h
				include/HardwareEncoder.h
				src/main.cpp
				src/Object.cpp
				src/gen_exported.cpp
				dependencies/gen/genlib.cpp
				dependencies/gen/json.c
				dependencies/gen/json_builder.c
				dependencies/jsonxx/jsonxx.h
				dependencies/jsonxx/jsonxx.cc
				${RASPI_FILES}
				${PROJECT_HEADERS}
)

# Link RtAudio and it's dependencies
if (APPLE)
	find_library(RTAUDIO_LIB "librtaudio_static.a")
	target_link_libraries(${PROJECT_NAME} PUBLIC ${RTAUDIO_LIB})

	find_library(CARBON_LIB "Carbon")
	target_link_libraries(${PROJECT_NAME} PUBLIC ${CARBON_LIB})

	find_library(COREAUDIO_LIB "CoreAudio")
	target_link_libraries(${PROJECT_NAME} PUBLIC ${COREAUDIO_LIB})
else(NOT WIN32)
	find_library(RTAUDIO_LIB "rtaudio")
	target_link_libraries(${PROJECT_NAME} PUBLIC ${RTAUDIO_LIB})
endif()


# Link RtMidi
if (APPLE)
	find_library(RTMIDI_LIB "librtmidi.a")
	target_link_libraries(${PROJECT_NAME} PUBLIC ${RTMIDI_LIB})

	find_library(COREMIDI_LIB "CoreMIDI")
	target_link_libraries(${PROJECT_NAME} PUBLIC ${COREMIDI_LIB})
else(NOT WIN32)
	find_library(RTMIDI_LIB "rtmidi")
	target_link_libraries(${PROJECT_NAME} PUBLIC ${RTMIDI_LIB})
endif()

if (WIRINGPI_LIB)
	target_link_libraries(${PROJECT_NAME} PUBLIC wiringPi pthread)
endif()

UNSET(FLOAT_PRECISION CACHE)
UNSET(FASTMATH CACHE)
