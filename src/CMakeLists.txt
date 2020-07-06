cmake_minimum_required(VERSION 3.14)

find_package(Magnum REQUIRED
        GL
        MeshTools
        Shaders
        SceneGraph
        Trade
        Sdl2Application
        Primitives)

find_package(MagnumIntegration REQUIRED
        ImGui
        Eigen)

corrade_add_resource(clothsim_RESOURCES resources.conf)
set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)

set(clothsim_SRC
        App.cpp
        Cloth.cpp
        Drawable.cpp
        Integrators.cpp
        main.cpp
        Oscillator.cpp
        Planet.cpp
        Shaders.cpp
        System.cpp
        UI.cpp
        Util.cpp
        )

add_executable(clothsim ${clothsim_SRC} ${clothsim_RESOURCES})
set_target_properties(clothsim
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_compile_definitions(clothsim PRIVATE CLOTHSIM_SOURCE_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}")

if (NOT CORRADE_TARGET_EMSCRIPTEN)
        target_compile_options(clothsim PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)
        target_link_options(clothsim PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)
endif()

set_property(TARGET clothsim
        PROPERTY CXX_STANDARD_REQUIRED 20
        PROPERTY CXX_STANDARD 20
        PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
target_compile_features(clothsim PRIVATE cxx_std_20)

target_include_directories(clothsim SYSTEM PRIVATE ${MAGNUM_INCLUDE_DIR})
target_include_directories(clothsim SYSTEM PRIVATE ${MAGNUMEXTRAS_INCLUDE_DIR})
target_include_directories(clothsim SYSTEM PRIVATE ${CORRADE_INCLUDE_DIR})
target_include_directories(clothsim SYSTEM PRIVATE ${EIGEN_INCLUDE_DIR})

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
    -Wdouble-promotion
    #-Weffc++
    -Wno-error=conversion
    -fasynchronous-unwind-tables
    -fstack-protector-all
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
    -Wno-error=sign-conversion
    -Wno-error=missing-declarations
    -Wno-error=unused-variable
    -Wno-error=unused-function
    -Wno-error=unknown-pragmas
    -Wno-error=unused-parameter)

if (NOT CORRADE_TARGET_EMSCRIPTEN)
    target_compile_options(clothsim PRIVATE
            -Wuseless-cast
            -Wduplicated-cond
            -Wduplicated-branches
            -Wlogical-op)
endif()

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
	target_link_libraries(clothsim PRIVATE $<$<CONFIG:Release>:OpenMP::OpenMP_CXX>)
endif()

if (CORRADE_TARGET_EMSCRIPTEN)
    target_compile_options(clothsim PRIVATE
        "SHELL:-s ALLOW_MEMORY_GROWTH=1"
        "SHELL:-s BINARYEN_TRAP_MODE=clamp"
        "SHELL:-s DEMANGLE_SUPPORT=1"
        "SHELL:-s ASSERTIONS=1")
    target_link_options(clothsim PRIVATE
        "SHELL:-s ALLOW_MEMORY_GROWTH=1"
        "SHELL:-s BINARYEN_TRAP_MODE=clamp"
        "SHELL:-s DEMANGLE_SUPPORT=1"
        "SHELL:-s ASSERTIONS=1")
endif ()

target_link_libraries(clothsim PRIVATE
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
