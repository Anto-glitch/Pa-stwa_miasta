cmake_minimum_required(VERSION 4.0)
project(Panstwa_Miasta)

add_executable(client
    client/client.cpp
    client/screens/image_sprite.cpp
    client/systems/socket_connection.cpp
    client/systems/socket_write.cpp
)

add_executable(server
    server/server.cpp
    server/systems/server_connection.cpp
    server/systems/socket_read.cpp
    server/utils/utils.cpp
)

find_package(SFML 3.0 COMPONENTS Graphics Window System Network REQUIRED)

target_link_libraries(client PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)

target_link_libraries(server PRIVATE
    SFML::System
)