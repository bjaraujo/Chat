
project(Chat)

cmake_minimum_required(VERSION 3.0)

set(CHAT_NET_HEADERS
network.h
anyoption.h
aes256.hpp
)

set(CHAT_NET_SOURCES
main.cpp
network.cpp
anyoption.cpp
aes256.cpp
)

source_group("Header Files" FILES ${CHAT_NET_HEADERS})
source_group("Source Files" FILES ${CHAT_NET_SOURCES})

find_package(SDL REQUIRED)
find_package(SDL_net REQUIRED)

include_directories(
    ${SDL_INCLUDE_DIR}
    ${SDL_NET_INCLUDE_DIRS}   
)

add_executable(Chat ${CHAT_NET_HEADERS} ${CHAT_NET_SOURCES})

target_link_libraries(Chat
    ${SDL_LIBRARY}
    ${SDLNET_LIBRARY}
)

