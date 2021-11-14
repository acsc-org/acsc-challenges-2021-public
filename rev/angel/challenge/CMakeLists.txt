cmake_minimum_required(VERSION 3.14)

project(angel)

set(CMAKE_CXX_FLAGS "-O1 -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(ANGELSCRIPT_LIBRARY_NAME angelscript)

set(ANGEL_SDK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sdk)
add_subdirectory(${ANGEL_SDK_DIR}/angelscript/projects/cmake)

set(ANGEL_ADDON_SRCS
    ${ANGEL_SDK_DIR}/add_on/scriptbuilder/scriptbuilder.cpp
    ${ANGEL_SDK_DIR}/add_on/scriptstdstring/scriptstdstring.cpp
    ${ANGEL_SDK_DIR}/add_on/scriptarray/scriptarray.cpp
    ${ANGEL_SDK_DIR}/add_on/scriptfile/scriptfile.cpp
    )

add_executable(angel_compiler angel_compiler.cpp ${ANGEL_ADDON_SRCS})
target_link_libraries(angel_compiler ${ANGELSCRIPT_LIBRARY_NAME})
target_include_directories(angel_compiler PRIVATE
    ${ANGEL_SDK_DIR}/angelscript/include
    ${ANGEL_SDK_DIR}/add_on
    )

add_executable(angel_executor angel_executor.cpp ${ANGEL_ADDON_SRCS})
target_link_libraries(angel_executor ${ANGELSCRIPT_LIBRARY_NAME})
target_include_directories(angel_executor PRIVATE
    ${ANGEL_SDK_DIR}/angelscript/include
    ${ANGEL_SDK_DIR}/add_on
    )

# # コンパイラ依存のオプション
# if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
#     # aliasがstrictな前提の最適化をしない
#     target_compile_options(ramiel_compiler PRIVATE "-fno-strict-aliasing")
# elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
#     # BOM無しUTF-8で書かれたソースコードを受け付け、入出力もUTF-8にする
#     target_compile_options(ramiel_compiler PRIVATE "/utf-8")
# endif()

target_compile_options(angel_compiler PRIVATE "-fno-strict-aliasing")
target_compile_options(angel_executor PRIVATE "-fno-strict-aliasing")

# スクリプトファイルを実行フォルダにコピー
add_custom_command(TARGET angel_compiler POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/angel.as $<TARGET_FILE_DIR:angel_compiler>)
add_custom_command(TARGET angel_executor POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/flag.txt $<TARGET_FILE_DIR:angel_executor>)


