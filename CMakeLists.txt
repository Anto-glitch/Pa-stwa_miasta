cmake_minimum_required(VERSION 4.0)
project(Panstwa_Miasta)

add_executable(client client/init_connection.cpp)
add_executable(server server/init_function.cpp)

find_package(SFML 3.0 COMPONENTS graphics window system REQUIRED)

target_link_libraries(client PRIVATE sfml-graphics sfml-window sfml-system sfml-network)
target_link_libraries(server PRIVATE sfml-system sfml-network)
