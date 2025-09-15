cmake_minimum_required(VERSION 3.10)
project(DataRaceDetector)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- ThreadSanitizer Configuration ---
# Check if the user wants to build with TSan, e.g., cmake -DUSE_TSAN=ON ..
option(USE_TSAN "Enable ThreadSanitizer" OFF)

if(USE_TSAN)
    # Add the ThreadSanitizer flags for both compilation and linking
    add_compile_options(-fsanitize=thread)
    add_link_options(-fsanitize=thread)
    
    # On some systems (Linux), you might also need to explicitly link against the TSan library.
    # add_link_options(-fuse-ld=lld) # Optional: Using the LLD linker can be better with sanitizers.
    
    message(STATUS "Building with ThreadSanitizer enabled")
endif()

# Add the main include directory for ALL targets (detector_lib, detector_main)
include_directories(${PROJECT_SOURCE_DIR}/include)

# Create a library from all our source files
add_library(detector_lib STATIC
    src/VectorClock.cpp
    src/LockSet.cpp
    src/ShadowMemory.cpp
    src/Detector.cpp
    src/Hook.cpp
)

# Create the main executable that uses the library
add_executable(detector_main main.cpp)
target_link_libraries(detector_main detector_lib pthread)

# Create an example program that is instrumented with our macros
add_executable(race_example samples/race_example.cpp)
# Link the example to our library and pthread
target_link_libraries(race_example detector_lib pthread)
# !!! CRITICAL FIX !!!
# The 'samples' directory is not automatically told about the include directory.
# This line specifically adds the include path for the race_example target.
target_include_directories(race_example PRIVATE ${PROJECT_SOURCE_DIR}/include)