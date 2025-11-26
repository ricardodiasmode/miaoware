cmake_minimum_required(VERSION 3.26)
# Configurando o padrão c++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_PREFIX_PATH "H:/msys64/mingw64")

set(webp_INCLUDE_PATH "H:/msys64/mingw64/include")
set(webp_LIBRARY "H:/msys64/mingw64/lib/libwebp.dll.a")

set(webpdemux_INCLUDE_PATH "H:/msys64/mingw64/include")
set(webpdemux_LIBRARY "H:/msys64/mingw64/lib/libwebpdemux.dll.a")

set(TIFF_INCLUDE_DIR "H:/msys64/mingw64/include")
set(TIFF_LIBRARY "H:/msys64/mingw64/lib/libtiff.dll.a")
set(JPEG_INCLUDE_DIR "H:/msys64/mingw64/include")
set(JPEG_LIBRARY "H:/msys64/mingw64/lib/libjpeg.dll.a")
set(libjxl_INCLUDE_PATH "H:/msys64/mingw64/include")
set(libjxl_LIBRARY "H:/msys64/mingw64/lib/libjxl.dll.a")
set(libjxl_threads_LIBRARY "H:/msys64/mingw64/lib/libjxl_threads.dll.a")
# --- PNG ---
set(PNG_PNG_INCLUDE_DIR "H:/msys64/mingw64/include")
set(PNG_LIBRARY "H:/msys64/mingw64/lib/libpng.dll.a")

# --- ZLIB ---
set(ZLIB_INCLUDE_DIR "H:/msys64/mingw64/include")
set(ZLIB_LIBRARY "H:/msys64/mingw64/lib/libz.dll.a")

set(SDL2_mixer_DIR "H:/msys64/mingw64/lib/cmake/SDL2_mixer")
set(CMAKE_PREFIX_PATH "H:/msys64/mingw64")
set(SDL2_ttf_DIR "H:/msys64/mingw64/lib/cmake/SDL2_ttf")


# Configurando o nome e executável do projeto
set(PROJECT_NAME "mioware")
project(${PROJECT_NAME})

find_package(SDL2 2.32.2 EXACT REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL2_ttf REQUIRED)
include_directories(${SDL2_TTF_INCLUDE_DIRS})


add_executable(${PROJECT_NAME}
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
        Source/Game.cpp
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
        Source/Renderer/Font.h
        Source/Renderer/Font.cpp
        Source/Utils/ObjectManager.cpp
        Source/Utils/ObjectManager.h
        Source/Terminal.h
        Source/Terminal.cpp
)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(SDL2_image REQUIRED)
include_directories(${SDL2_IMAGE_INCLUDE_DIRS})


target_link_libraries(${PROJECT_NAME} PRIVATE
        mingw32           # necessário para SDL2main
        SDL2main
        SDL2               # lib principal SDL2
        SDL2_image
        SDL2_ttf
        GLEW::GLEW
        OpenGL32
)

