cmake_minimum_required(VERSION 3.21)

project(FPGFX VERSION 0.1.0 LANGUAGES CXX C)

find_package(Eigen3 REQUIRED)
find_package(spdlog REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

add_executable(FPGFX_app src/main.cpp)

set_target_properties(
        FPGFX_app
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/archive"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        CXX_STANDARD 17
        CMAKE_EXPORT_COMPILE_COMMANDS ON)

target_link_libraries(FPGFX_app PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        spdlog::spdlog
        Eigen3::Eigen
        fmt::fmt
        assimp::assimp
        EnTT::EnTT
        imgui::imgui)

add_subdirectory(src)
