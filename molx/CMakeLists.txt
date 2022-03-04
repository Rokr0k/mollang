cmake_minimum_required(VERSION 3.8)

project(molx)

add_executable(${PROJECT_NAME} "main.c")

install(TARGETS molx RUNTIME DESTINATION bin)