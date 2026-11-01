cmake_minimum_required(VERSION 4.0)
project(Panstwa_Miasta)

add_executable(client
    client/client.cpp
    client/image_sprite.cpp
    client/socket_connection.cpp
    client/socket_write.cpp
)

add_executable(server
    server/server.cpp
    server/server_connection.cpp
    server/socket_read.cpp
)

find_package(SFML 3.0 COMPONENTS Graphics Window System Network REQUIRED)

target_link_libraries(client PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Network
)

target_link_libraries(server PRIVATE
    SFML::System
    SFML::Network
)