cmake_minimum_required(VERSION 3.14)
project(KaleidoscopeLanguage CXX)
include(FetchContent)
include(CMakePrintHelpers)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

FetchContent_Declare(LLVMF
    GIT_REPOSITORY https://github.com/llvm/llvm-project.git
    GIT_TAG llvmorg-17.0.6
) 

FetchContent_MakeAvailable(LLVMF)

cmake_print_variables(llvmf_SOURCE_DIR)
add_subdirectory(${llvmf_SOURCE_DIR}/llvm)

# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()

set(SOURCES
    ast_builder.cpp
    ast_builder.h

    tokenizer.cpp
    tokenizer.h

    main.cpp
)


list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/src/)



add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${LLVM_SOURCE_DIR}/include
    ${clang_SOURCE_DIR}/include
    ${llvm_BINARY_DIR}/tools/clang/include
    ${llvm_BINARY_DIR}/include
)
add_dependencies(${PROJECT_NAME} intrinsics_gen)
target_link_libraries(${PROJECT_NAME} PRIVATE
    LLVMOption
    LLVMSupport
    clangAST
    clangASTMatchers
    clangBasic
    clangDriver
    clangFrontend
    clangLex
    clangRewrite
    clangRewriteFrontend
    clangSerialization
    clangStaticAnalyzerFrontend
    clangTooling
    clangSema
)