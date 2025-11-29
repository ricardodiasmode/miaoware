cmake_minimum_required(VERSION 3.26)

set(CMAKE_CXX_STANDARD 17)
set(PROJECT_NAME "miaoware")
project(${PROJECT_NAME})

set(SDL2_ROOT "C:/Program Files/SDL2" CACHE PATH "Root path for SDL2 and SDL2_image")
set(GLEW_ROOT "C:/Program Files/glew" CACHE PATH "Root path for GLEW")

find_package(SDL2 2.32.2 EXACT REQUIRED)
find_package(OpenGL REQUIRED)

set(GLEW_INCLUDE_DIR "${GLEW_ROOT}/include")
set(GLEW_LIB_MSVC    "${GLEW_ROOT}/lib/Release/x64/glew32.lib")
set(GLEW_LIB_MINGW   "${GLEW_ROOT}/lib/Release/x64/libglew32.a")  # adapte se for diferente

set(SDL2_IMAGE_INCLUDE_DIR "${SDL2_ROOT}/include")
set(SDL2_IMAGE_LIB_MSVC   "${SDL2_ROOT}/lib/x64/SDL2_image.lib")
set(SDL2_IMAGE_LIB_MINGW  "${SDL2_ROOT}/lib/x64/libSDL2_image.a")
set(SDL2_IMAGE_DLL        "${SDL2_ROOT}/lib/x64/SDL2_image.dll")

set(SDL2_MIXER_INCLUDE_DIR "${SDL2_ROOT}/include")
set(SDL2_MIXER_LIB_MSVC   "${SDL2_ROOT}/lib/x64/SDL2_mixer.lib")
set(SDL2_MIXER_DLL        "${SDL2_ROOT}/lib/x64/SDL2_mixer.dll")

set(SDL2_TTF_INCLUDE_DIR "${SDL2_ROOT}/include")
set(SDL2_TTF_LIB_MSVC   "${SDL2_ROOT}/lib/x64/SDL2_ttf.lib")
set(SDL2_TTF_DLL        "${SDL2_ROOT}/lib/x64/SDL2_ttf.dll")

file(GLOB_RECURSE SOURCES
        Source/*.cpp
        Source/*.h
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE "${GLEW_INCLUDE_DIR}")

target_include_directories(${PROJECT_NAME} PRIVATE "${SDL2_IMAGE_INCLUDE_DIR}")

target_include_directories(${PROJECT_NAME} PRIVATE "${SDL2_MIXER_INCLUDE_DIR}")

target_include_directories(${PROJECT_NAME} PRIVATE "${SDL2_TTF_INCLUDE_DIR}")

if (NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mconsole")
endif()

if (MSVC)
    message(STATUS "MSVC detected -> procurando import-libs MSVC...")
    if (EXISTS "${GLEW_LIB_MSVC}")
        set(GLEW_LIBRARY "${GLEW_LIB_MSVC}")
    else()
        message(FATAL_ERROR "GLEW MSVC import lib não encontrada em: ${GLEW_LIB_MSVC}")
    endif()

    if (EXISTS "${SDL2_IMAGE_LIB_MSVC}")
        set(SDL2_IMAGE_LIBRARY "${SDL2_IMAGE_LIB_MSVC}")
    elseif (EXISTS "${SDL2_IMAGE_LIB_MINGW}")
        # fallback raro: usar .a se só tiver
        set(SDL2_IMAGE_LIBRARY "${SDL2_IMAGE_LIB_MINGW}")
        message(WARNING "Usando lib do tipo MinGW como fallback para SDL2_image: ${SDL2_IMAGE_LIB_MINGW}")
    else()
        message(FATAL_ERROR "Import lib do SDL2_image não encontrada. Procure por SDL2_image.lib em ${SDL2_ROOT}/lib/x64")
    endif()
else()
    message(STATUS "Non-MSVC (assumindo MinGW) -> procurando import-libs MinGW...")
    if (EXISTS "${GLEW_LIB_MINGW}")
        set(GLEW_LIBRARY "${GLEW_LIB_MINGW}")
    elseif (EXISTS "${GLEW_LIB_MSVC}")
        set(GLEW_LIBRARY "${GLEW_LIB_MSVC}")
        message(WARNING "GLEW MSVC lib encontrada, mas estamos em toolchain não-MSVC. Pode haver incompatibilidade.")
    else()
        message(FATAL_ERROR "GLEW import lib para MinGW não encontrada em: ${GLEW_LIB_MINGW}")
    endif()

    if (EXISTS "${SDL2_IMAGE_LIB_MINGW}")
        set(SDL2_IMAGE_LIBRARY "${SDL2_IMAGE_LIB_MINGW}")
    elseif (EXISTS "${SDL2_IMAGE_LIB_MSVC}")
        set(SDL2_IMAGE_LIBRARY "${SDL2_IMAGE_LIB_MSVC}")
        message(WARNING "SDL2_image .lib (MSVC) encontrada em ambiente MinGW — cuidado com incompatibilidades.")
    else()
        message(FATAL_ERROR "Import lib do SDL2_image não encontrada. Procure por libSDL2_image.a em ${SDL2_ROOT}/lib/x64")
    endif()
endif()

message(STATUS "Usando GLEW import lib: ${GLEW_LIBRARY}")
message(STATUS "Usando SDL2_image import lib: ${SDL2_IMAGE_LIBRARY}")
message(STATUS "SDL2_image.dll esperado em: ${SDL2_IMAGE_DLL}")

# ---------- Sanity checks ----------
if (NOT EXISTS "${SDL2_ROOT}/include")
    message(FATAL_ERROR "SDL2 include não encontrado em: ${SDL2_ROOT}/include")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
        SDL2::SDL2main
        SDL2::SDL2
        ${SDL2_IMAGE_LIBRARY}
        ${GLEW_LIBRARY}
        ${SDL2_MIXER_LIB_MSVC}
        ${SDL2_TTF_LIB_MSVC}
        opengl32
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL2_ROOT}/lib/x64/SDL2.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL2_IMAGE_DLL}"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${GLEW_ROOT}/bin/Release/x64/glew32.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL2_MIXER_DLL}"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL2_TTF_DLL}"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)

file(GLOB_RECURSE SHADER_FILES "${CMAKE_SOURCE_DIR}/Shaders/*")

add_custom_target(copy_shaders ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Shaders"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/Shaders" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Shaders"
        SOURCES ${SHADER_FILES}
        COMMENT "Copying shaders (copy_shaders)"
)

add_dependencies(${PROJECT_NAME} copy_shaders)