cmake_minimum_required(VERSION 3.22)
project(wesex VERSION 1 DESCRIPTION "A Discord Bot" LANGUAGES CXX)


# Correctly append the path to your CMake module (FindDpp.cmake) here.
# Assuming cmake is a directory at the root of your project.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
add_subdirectory(external/json)
# Assuming the DPP library's root directory is /usr/local.
# You might not need to set this if DPP is installed in a standard location.
set(DPP_ROOT_DIR "/usr/local")

# Gather all source files from the src directory.
aux_source_directory("src" coresrc)

# Create an executable with the project name and all source files.
add_executable(${PROJECT_NAME} ${coresrc})

# Find the DPP package using FindDpp.cmake.
find_package(DPP REQUIRED)
find_package(nlohmann_json 3.11.3 REQUIRED)

set(CURL_DYLIB "/opt/homebrew/opt/curl/lib/libcurl.4.dylib")
set(JSON_DYLIB "/opt/homebrew/opt/nlohmann-json/")
# Link the DPP library.
target_link_libraries(${PROJECT_NAME} PRIVATE ${DPP_LIBRARIES}
  ${CURL_DYLIB} 
  nlohmann_json::nlohmann_json)

set(CURL_INCLUDE "/opt/homebrew/opt/curl/include/curl/")
set(JSON_INCLUDE "/opt/homebrew/Cellar/nlohmann-json/3.11.3/include/nlohmann/")
# Include the DPP header directories. Correcting the variable name here.
target_include_directories(${PROJECT_NAME} PRIVATE ${DPP_INCLUDE_DIR}
  ${CURL_INCLUDE} 
  )

# Set C++ version to C++17.
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    INSTALL_RPATH "/usr/local/lib"
)

