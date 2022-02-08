cmake_minimum_required(VERSION 3.8)

project(molc)

add_executable(molc "main.c" "parse.h" "parse.c")

install(TARGETS molc RUNTIME DESTINATION bin)