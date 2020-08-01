cmake_minimum_required(VERSION 3.13)

project(clothsim LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/modules/")

cmake_minimum_required(VERSION 3.13)

find_package(Corrade REQUIRED Main)
find_package(Magnum REQUIRED
        GL
        MeshTools
        Shaders
        SceneGraph
        Trade
        Primitives)

if(CORRADE_TARGET_EMSCRIPTEN)
        find_package(Magnum REQUIRED EmscriptenApplication)
else()
        find_package(Magnum REQUIRED Sdl2Application)
endif()


find_package(MagnumIntegration REQUIRED
        ImGui
        Eigen)

corrade_add_resource(clothsim_RESOURCES src/resources.conf)
set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)

set(clothsim_SRC
        src/App.cpp
        src/Cloth.cpp
        src/Drawable.cpp
        src/Integrators.cpp
        src/Oscillator.cpp
        src/Planet.cpp
        src/Shaders.cpp
        src/System.cpp
        src/UI.cpp
        src/Util.cpp
        )

add_subdirectory(src/)

add_executable(clothsim ${clothsim_SRC} ${clothsim_RESOURCES})
target_compile_definitions(clothsim PRIVATE CLOTHSIM_SOURCE_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}/src")

if (NOT CORRADE_TARGET_EMSCRIPTEN)
        target_compile_options(clothsim PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)
        target_link_options(clothsim PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)
endif()

set_property(TARGET clothsim
        PROPERTY CXX_STANDARD_REQUIRED 20
        PROPERTY CXX_STANDARD 20
        PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
target_compile_features(clothsim PRIVATE cxx_std_20)

if (NOT CORRADE_TARGET_EMSCRIPTEN)
        target_compile_options(clothsim PRIVATE
        -Wall
        -Wextra
        -Werror
        -Wconversion
        -Wshadow
        #-Wnon-virtual-dtor
        -Wpedantic
        #-Woverloaded-virtual
        -Wnull-dereference
        #-Weffc++
        -Wno-error=conversion
        -fasynchronous-unwind-tables
        -Werror=format-security
        -Wdisabled-optimization
        -Wformat=2
        -Wcast-align
        -Wformat-security
        -Wmissing-braces
        -Wparentheses
        -Wpointer-arith
        -Wredundant-decls
        -Wreturn-type
        -Wsign-compare
        -Wuninitialized
        -Wunreachable-code
        -Wunused
        -Wunused-label
        -Wunused-value
        -Wunused-variable
        -Wsign-conversion
        -Wno-error=double-promotion
        -Wno-error=sign-conversion
        -Wno-error=missing-declarations
        -Wno-error=unused-variable
        -Wno-error=unused-function
        -Wno-error=unknown-pragmas
        -Wno-error=unused-parameter)
endif()

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
	target_link_libraries(clothsim PRIVATE $<$<CONFIG:Release>:OpenMP::OpenMP_CXX>)
endif()

if (CORRADE_TARGET_EMSCRIPTEN)
    target_compile_options(clothsim PRIVATE
        #"SHELL:-s ALLOW_MEMORY_GROWTH=1"
        #"SHELL:-s BINARYEN_TRAP_MODE=clamp"
        "SHELL:-s DEMANGLE_SUPPORT=1"
        "SHELL:-s ASSERTIONS=1"
    )
    target_link_options(clothsim PRIVATE
        #"SHELL:-s ALLOW_MEMORY_GROWTH=1"
        #"SHELL:-s BINARYEN_TRAP_MODE=clamp"
        "SHELL:-s DEMANGLE_SUPPORT=1"
        "SHELL:-s ASSERTIONS=1"
    )
endif ()

target_link_libraries(clothsim PRIVATE
        Corrade::Main
        Magnum::Application
        Magnum::GL
        Magnum::Magnum
        Magnum::MeshTools
        Magnum::SceneGraph
        Magnum::Shaders
        Magnum::Trade
        Magnum::Primitives
        MagnumIntegration::ImGui
        MagnumIntegration::Eigen)

if (CORRADE_TARGET_EMSCRIPTEN)
    install(TARGETS clothsim DESTINATION ${MAGNUM_DEPLOY_PREFIX})
    install(FILES
            index.html
            EmscriptenApplication.js
            style.css
            DESTINATION ${MAGNUM_DEPLOY_PREFIX})
    install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/clothsim.js.mem
            ${CMAKE_CURRENT_BINARY_DIR}/clothsim.wasm
            DESTINATION ${MAGNUM_DEPLOY_PREFIX} OPTIONAL)
endif ()