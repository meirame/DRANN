cmake_minimum_required(VERSION 3.20)
project(DRANN LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(drann DRANN.cpp)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(drann PRIVATE -Wall -Wextra -Wpedantic)
endif()
include(CTest)
if(BUILD_TESTING)
  add_executable(drann_tests tests/DRANN_tests.cpp)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(drann_tests PRIVATE -Wall -Wextra -Wpedantic)
  endif()
  add_test(NAME drann_core COMMAND drann_tests)
endif()

option(DRANN_BUILD_GUI "Compilar la ventana C++ de entrenamiento" ON)
if(DRANN_BUILD_GUI)
  find_package(OpenGL REQUIRED)
  find_package(glfw3 3.3 REQUIRED)
  include(FetchContent)
  if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
  endif()
  # Versiones compatibles fijadas y archivos verificados antes de compilar.
  FetchContent_Declare(imgui
    URL https://codeload.github.com/ocornut/imgui/tar.gz/refs/tags/v1.91.9b
    URL_HASH SHA256=8e1bbc76c71d74fef2fb85db7e7ca8eba13d6a86623c54992b60162db554ffdb)
  FetchContent_Declare(implot
    URL https://codeload.github.com/epezent/implot/tar.gz/refs/tags/v0.17
    URL_HASH SHA256=0aa3ff4fb97e553608e6758e77980eedf01745628fe6c025e647f941ae674127)
  FetchContent_MakeAvailable(imgui implot)

  add_library(drann_imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp)
  target_include_directories(drann_imgui PUBLIC
    ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
  target_compile_definitions(drann_imgui PRIVATE GLFW_INCLUDE_NONE)
  target_link_libraries(drann_imgui PUBLIC glfw OpenGL::GL ${CMAKE_DL_LIBS})

  add_library(drann_implot STATIC
    ${implot_SOURCE_DIR}/implot.cpp ${implot_SOURCE_DIR}/implot_items.cpp)
  target_include_directories(drann_implot PUBLIC ${implot_SOURCE_DIR})
  target_link_libraries(drann_implot PUBLIC drann_imgui)

  add_executable(drann_gui DRANN.cpp graphics/DRANN_plot.cpp)
  target_compile_definitions(drann_gui PRIVATE DRANN_GUI)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(drann_gui PRIVATE -Wall -Wextra -Wpedantic)
  endif()
  target_link_libraries(drann_gui PRIVATE drann_implot)
endif()
