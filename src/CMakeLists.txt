target_sources(FPGFX_app PRIVATE 
    App.cpp
    Log.cpp
    Renderer.cpp
    UI.cpp
    Transform.cpp
    Threading.cpp
    Format.cpp
    Mesh.cpp
    Types.h
    MathTypes.h
    Definitions.h)

add_subdirectory(ECS)