cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(PROJECT_NAME "miaoware")
project(${PROJECT_NAME})

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

set(SOURCES
        Source/Renderer/Shader.cpp
        Source/Renderer/Shader.h
        Source/Main.cpp
        Source/Math.cpp
        Source/Math.h
        Source/Random.cpp
        Source/Random.h
        Source/Game.cpp
        Source/Game.h
        Source/Actors/Actor.cpp
        Source/Actors/Actor.h
        Source/Components/Component.cpp
        Source/Components/Component.h
        Source/Components/Drawing/DrawComponent.cpp
        Source/Components/Drawing/DrawComponent.h
        Source/Components/Physics/RigidBodyComponent.cpp
        Source/Components/Physics/RigidBodyComponent.h
        Source/Components/Physics/AABBColliderComponent.cpp
        Source/Components/Physics/AABBColliderComponent.h
        Source/Components/ParticleSystemComponent.cpp
        Source/Components/ParticleSystemComponent.h
        Source/Renderer/Renderer.cpp
        Source/Renderer/Renderer.h
        Source/Renderer/VertexArray.h
        Source/Renderer/VertexArray.cpp
        Source/Renderer/Texture.cpp
        Source/Renderer/Texture.h
        Source/Actors/Block.cpp
        Source/Actors/Block.h
        Source/Actors/Goomba.cpp
        Source/Actors/Goomba.h
        Source/Actors/Spawner.cpp
        Source/Actors/Spawner.h
        Source/Actors/Cat.cpp
        Source/Actors/Cat.h
        Source/Components/Drawing/AnimatorComponent.cpp
        Source/Components/Drawing/AnimatorComponent.h
        Source/Components/Drawing/RectComponent.cpp
        Source/Components/Drawing/RectComponent.h
        Source/Actors/Background.cpp
        Source/Actors/Background.h
        Source/Components/Drawing/TextureComponent.cpp
        Source/Components/Drawing/TextureComponent.h
        Source/Actors/MovingBlock.cpp
        Source/Actors/MovingBlock.h
        Source/Actors/SpawnBlock.cpp
        Source/Actors/SpawnBlock.h
        Source/Actors/Mushroom.cpp
        Source/Actors/Mushroom.h
        Source/Utils/ObjectManager.cpp
        Source/Utils/ObjectManager.h
)

add_executable(${PROJECT_NAME} ${SOURCES})

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
    # Some Find modules export *_LIBRARIES or *_LIBRARY
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