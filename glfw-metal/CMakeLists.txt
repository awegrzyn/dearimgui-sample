cmake_minimum_required(VERSION 3.10)
project(dearimgui-glfw-metal)

if(NOT APPLE)
  message(FATAL_ERROR "This can run only on macOS")
endif()




set(BACKEND
  ../3rd-party/imgui/backends/imgui_impl_glfw.cpp
  ../3rd-party/imgui/backends/imgui_impl_metal.mm
  ../3rd-party/imgui/imgui.cpp
  ../3rd-party/imgui/imgui_demo.cpp
  ../3rd-party/imgui/imgui_draw.cpp
  ../3rd-party/imgui/imgui_tables.cpp
  ../3rd-party/imgui/imgui_widgets.cpp
)

find_package(GLFW NAMES glfw3 CONFIG)

find_library(METAL Metal)
find_library(CF CoreFoundation)
find_library(FOUNDATION Foundation)
find_library(QUARTZ_CORE QuartzCore)
#set_target_properties(DebugGUI PROPERTIES COMPILE_FLAGS "-fobjc-arc")
set(MACOS_FRAMEWORKS ${METAL} ${CF} ${FOUNDATION} ${QUARTZ_CORE})

add_executable(imgui-sample main.mm ${BACKEND})

target_compile_features(imgui-sample PUBLIC cxx_std_17)

target_include_directories(imgui-sample PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/../3rd-party/imgui/
  ${CMAKE_CURRENT_SOURCE_DIR}/../3rd-party/imgui/backends/
)

target_link_libraries(imgui-sample PUBLIC glfw ${MACOS_FRAMEWORKS} PRIVATE dl)
