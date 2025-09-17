cmake_minimum_required(VERSION 3.10)
project(DataRaceDetector)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- ThreadSanitizer Configuration ---
option(USE_TSAN "Enable ThreadSanitizer" OFF)

if(USE_TSAN)
    add_compile_options(-fsanitize=thread)
    add_link_options(-fsanitize=thread)
    message(STATUS "Building with ThreadSanitizer enabled")
endif()

# Add the main include directory for ALL targets
include_directories(${PROJECT_SOURCE_DIR}/include)

# Platform-specific settings
if(CMAKE_SYSTEM_NAME STREQUAL "Windows" OR WIN32)
    add_compile_definitions(DRD_WINDOWS=1)
    add_compile_definitions(NOMINMAX)  # Prevent Windows.h from defining min/max macros
    # For MinGW cross-compilation, we need to link statically
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
else()
    add_compile_definitions(DRD_LINUX=1)
    set(PLATFORM_LIBS dl pthread)
endif()

# --- ALWAYS INCLUDE OUR DETECTOR ANALYSIS (ADD THIS SECTION) ---
add_compile_definitions(DRD_ANALYSIS_ENABLED=1)

# Make sure our detector code is always compiled in
if(CMAKE_SYSTEM_NAME STREQUAL "Windows" OR WIN32)
    # On Windows, we need to be more explicit about including our analysis code
    add_compile_definitions(DETECTOR_ACTIVE=1)
endif()
# --- END OF ANALYSIS SECTION ---

# Create a library from all our source files
add_library(detector_lib STATIC
    src/VectorClock.cpp
    src/LockSet.cpp
    src/ShadowMemory.cpp
    src/Detector.cpp
    src/Hook.cpp
)

# Link the library against platform-specific libraries
target_link_libraries(detector_lib ${PLATFORM_LIBS})

# Create the main executable that uses the library
add_executable(detector_main main.cpp)
target_link_libraries(detector_main detector_lib ${PLATFORM_LIBS})

# Create an example program that is instrumented with our macros
add_executable(race_example samples/race_example.cpp)
# Link the example to our library and platform libs
target_link_libraries(race_example detector_lib ${PLATFORM_LIBS})
# Ensure the example can find our headers
target_include_directories(race_example PRIVATE ${PROJECT_SOURCE_DIR}/include)