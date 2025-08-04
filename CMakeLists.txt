cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_STANDARD_REQUIRED True)

project(ytmus)

include_directories(${PROJECT_SOURCE_DIR}/include)

find_package(PkgConfig REQUIRED)
pkg_check_modules(MPV REQUIRED mpv)
pkg_check_modules(RTMIDI REQUIRED rtmidi)
pkg_check_modules(LIBLO REQUIRED liblo)
pkg_check_modules(NCURSES REQUIRED ncurses)

add_executable(ytmus main.cpp controller.cpp player.cpp midihandler.cpp oschandler.cpp tui.cpp)

target_include_directories(ytmus PRIVATE
    ${MPV_INCLUDE_DIRS}
    ${RTMIDI_INCLUDE_DIRS}
    ${LIBLO_INCLUDE_DIRS}
    ${NCURSES_INCLUDE_DIRS}
)

target_link_libraries(ytmus
    ${MPV_LIBRARIES}
    ${RTMIDI_LIBRARIES}
    ${LIBLO_LIBRARIES}
    ${NCURSES_LIBRARIES}
)

