cmake_minimum_required(VERSION 3.14)
project(KaleidoscopeLanguage CXX)

include(CMakePrintHelpers)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# LLVM
set(LLVM_INCLUDE_DIRS f:/LLVM/llvm-project/llvm/include)
set(llvm_libs
    LLVMAnalysis.lib
    LLVMCore.lib
    LLVMExecutionEngine.lib
    LLVMInstCombine.lib
    LLVMObject.lib
    LLVMOrcJIT.lib
    LLVMRuntimeDyld.lib
    LLVMScalarOpts.lib
    LLVMSupport.lib
    LLVMX86CodeGen.lib
    LLVMX86AsmParser.lib
    LLVMX86Desc.lib
    LLVMX86Disassembler.lib
    LLVMX86Info.lib
    LLVMPasses.lib
    LLVMCoroutines.lib
    LLVMipo.lib
    LLVMFrontendOpenMP.lib
    LLVMLinker.lib
    LLVMIRPrinter.lib
    LLVMVectorize.lib
    LLVMJITLink.lib
    LLVMOrcTargetProcess.lib
    LLVMOrcShared.lib
    LLVMWindowsDriver.lib
    LLVMOption.lib
    LLVMAsmPrinter.lib
    LLVMCFGuard.lib
    LLVMGlobalISel.lib
    LLVMInstrumentation.lib
    LLVMSelectionDAG.lib
    LLVMCodeGen.lib
    LLVMAggressiveInstCombine.lib
    LLVMTarget.lib
    LLVMBitWriter.lib
    LLVMObjCARCOpts.lib
    LLVMTransformUtils.lib
    LLVMProfileData.lib
    LLVMSymbolize.lib
    LLVMDebugInfoPDB.lib
    LLVMDebugInfoMSF.lib
    LLVMDebugInfoBTF.lib
    LLVMDebugInfoDWARF.lib
    LLVMIRReader.lib
    LLVMBitReader.lib
    LLVMAsmParser.lib
    LLVMTextAPI.lib
    LLVMRemarks.lib
    LLVMBitstreamReader.lib
    LLVMCodeGenTypes.lib
    LLVMMCParser.lib
    LLVMMCDisassembler.lib
    LLVMMC.lib
    LLVMBinaryFormat.lib
    LLVMTargetParser.lib
    LLVMDebugInfoCodeView.lib
    LLVMDemangle.lib
)

list(TRANSFORM llvm_libs PREPEND "f:/LLVM/llvm-project/llvm/bin/Debug/lib/")


set(AST_SOURCES
    ast_builder.cpp
    ast_builder.h
    base_expr_ast.h
    binary_expr_ast.cpp
    binary_expr_ast.h
    call_expr_ast.cpp
    call_expr_ast.h
    function_ast.cpp
    function_ast.h
    variable_expr_ast.cpp
    variable_expr_ast.h
    prototype_ast.cpp
    prototype_ast.h
    number_expr_ast.h
    number_expr_ast.cpp
)

set(SOURCES
    KaleidoscopeJIT.h
    main.cpp
    tokenizer.cpp
    tokenizer.h
    errors_def.cpp
    errors_def.h
    enviroment.h
)

list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/src/)
list(TRANSFORM AST_SOURCES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/src/)

add_executable(${PROJECT_NAME} ${SOURCES} ${AST_SOURCES})

source_group("AST" FILES ${AST_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${LLVM_SOURCE_DIR}/include
    ${clang_SOURCE_DIR}/include
    ${llvm_BINARY_DIR}/tools/clang/include
    ${llvm_BINARY_DIR}/include
    ${LLVM_INCLUDE_DIRS}
)


target_link_libraries(${PROJECT_NAME} PRIVATE ${llvm_libs} )