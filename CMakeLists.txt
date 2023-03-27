cmake_minimum_required(VERSION 3.11)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(messenger VERSION ${PROJECT_VERSION})

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")

find_package(Boost COMPONENTS system REQUIRED)
find_package(Threads)
#find_package(Boost COMPONENTS REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(messenger_server_cli server.cpp MsgServer.cpp Session.cpp Message.cpp UserBase.cpp History.cpp)
if(WIN32)
    add_executable(messenger_client_cli client.cpp MsgClient.cpp Message.cpp)
else()
    add_executable(messenger_client_cli client.cpp MsgClient.cpp Message.cpp conio.h/conio.h)
endif()

if (WIN32)
    target_link_libraries(messenger_server_cli -lws2_32 -lwsock32)
    target_link_libraries(messenger_client_cli -lws2_32 -lwsock32)
endif()

set_target_properties(messenger_server_cli messenger_client_cli PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        )

if (MSVC)
    target_compile_options(messenger_server_cli PRIVATE
            /W4
            )
    target_compile_options(messenger_client_cli PRIVATE
            /W4
            )
else ()
    target_compile_options(messenger_server_cli PRIVATE
            -Wall -Wextra -pedantic
            )
    target_compile_options(messenger_client_cli PRIVATE
            -Wall -Wextra -pedantic
            )
endif()

install(TARGETS messenger_server_cli messenger_client_cli RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)



