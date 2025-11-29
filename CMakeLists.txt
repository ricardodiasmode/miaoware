cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(PROJECT_NAME "miaoware")
project(${PROJECT_NAME})

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer QUIET)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/Source/*.cpp"
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

if(APPLE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE GL_SILENCE_DEPRECATION)
endif()

if (TARGET SDL2::SDL2)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)
else()
    target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES})
endif()

if (TARGET SDL2::SDL2main)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2main)
endif()

if (TARGET SDL2_image::SDL2_image)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2_image::SDL2_image)
elseif (DEFINED SDL2_IMAGE_LIBRARIES)
    target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_IMAGE_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_IMAGE_LIBRARIES})
endif()

# SDL2_ttf
if (TARGET SDL2_ttf::SDL2_ttf)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2_ttf::SDL2_ttf)
elseif (DEFINED SDL2_TTF_LIBRARIES OR DEFINED SDL2_TTF_LIBRARY)
    if (DEFINED SDL2_TTF_INCLUDE_DIRS)
        target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_TTF_INCLUDE_DIRS})
    elseif(DEFINED SDL2_TTF_INCLUDE_DIR)
        target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_TTF_INCLUDE_DIR})
    endif()
    if (DEFINED SDL2_TTF_LIBRARIES)
        target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_TTF_LIBRARIES})
    else()
        target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_TTF_LIBRARY})
    endif()
elseif(APPLE)
    target_include_directories(${PROJECT_NAME} PRIVATE /Library/Frameworks/SDL2_ttf.framework/Headers)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework SDL2_ttf")
endif()

# SDL2_mixer
if (TARGET SDL2_mixer::SDL2_mixer)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2_mixer::SDL2_mixer)
elseif (DEFINED SDL2_MIXER_LIBRARIES OR DEFINED SDL2_MIXER_LIBRARY)
    if (DEFINED SDL2_MIXER_INCLUDE_DIRS)
        target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_MIXER_INCLUDE_DIRS})
    elseif(DEFINED SDL2_MIXER_INCLUDE_DIR)
        target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_MIXER_INCLUDE_DIR})
    endif()
    if (DEFINED SDL2_MIXER_LIBRARIES)
        target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_MIXER_LIBRARIES})
    else()
        target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_MIXER_LIBRARY})
    endif()
elseif(APPLE)
    # Fallback to macOS framework
    target_include_directories(${PROJECT_NAME} PRIVATE /Library/Frameworks/SDL2_mixer.framework/Headers)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework SDL2_mixer")
endif()

if (TARGET GLEW::GLEW)
    target_link_libraries(${PROJECT_NAME} PRIVATE GLEW::GLEW)
else()
    target_include_directories(${PROJECT_NAME} PRIVATE ${GLEW_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${GLEW_LIBRARIES})
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

if (WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Copying SDL2/GLEW/SDL2_image DLLs (if found)"
    )
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/Shaders" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Shaders"
    COMMENT "Copying shaders next to the executable"
)