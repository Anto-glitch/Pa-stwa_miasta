cmake_minimum_required(VERSION 3.16)
project(Panstwa_Miasta LANGUAGES CXX)

add_executable(client
    client/client.cpp
    client/screens/generate_sprite.cpp
    client/screens/common/exit_button.cpp
    client/screens/common/cursor_interactions.cpp
    client/systems/socket_connection.cpp
    client/systems/socket_message.cpp
    client/utils/utils.cpp
    client/screens/common/create_text.cpp
    client/screens/common/textbox.h

)

add_executable(server
    server/server.cpp
    server/systems/server_connection.cpp
    server/systems/socket_message.cpp
    server/utils/utils.cpp
    server/handlers/client_handler.cpp
)

find_package(SFML 3.0 COMPONENTS Graphics Window System REQUIRED)

target_link_libraries(client PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)

target_link_libraries(server PRIVATE
    SFML::System
)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
