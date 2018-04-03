cmake_minimum_required(VERSION 3.4)
project(EasyLang)

set(PROJECT_NAME EasyLang)
set(CMAKE_CXX_STANDARD 11)

include_directories (
	deps/memory-allocators/includes/
)


option(JIT_COMPILER "LLVM based jit compiler" OFF)
if( JIT_COMPILER )
	find_package(LLVM REQUIRED CONFIG)
	message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
	message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

	include_directories(${LLVM_INCLUDE_DIRS})
	if(LLVM_BUILD_MAIN_SRC_DIR)
		include_directories(${LLVM_BUILD_MAIN_SRC_DIR}/tools/clang/include)
		include_directories(${LLVM_BUILD_BINARY_DIR}/tools/clang/include)
	endif()
	link_directories(${LLVM_LIBRARY_DIRS})
	add_definitions(${LLVM_DEFINITIONS})
	
	add_definitions(-DEASYLANG_JIT_ACTIVE)
endif()

add_definitions(
		-D__STDC_LIMIT_MACROS
		-D__STDC_CONSTANT_MACROS
)


# message(STATUS "User selected librarys = ${LIBRARY_LIST}")
# message(STATUS "User selected components = ${COMPONENT_LIST}")
# message(STATUS "    = ${llvm_libs}")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(EasyLang
        src/main.cpp
        include/Lexer.h
        src/Lexer.cpp
        src/ASTs.cpp
        include/ASTs.h
        Tests/LexerTests.h
        Tests/AstTests.h
        include/Macros.h
        include/Backend.h
        src/PrimativeValue.cpp
        include/PrimativeValue.h
        include/Exceptions.h
        src/InterpreterBackend.cpp
        include/InterpreterBackend.h
        Tests/InterpreterTests.h
        src/lib/iolib.cpp
        include/lib/iolib.h
        src/lib/corelib.cpp
        include/lib/corelib.h
        src/lib/arraylib.cpp
        include/lib/arraylib.h
        src/System.cpp
        include/Scope.h
        src/Scope.cpp
        src/EasyEngine.cpp
        include/EasyEngine.h
        include/Catch.h
		include/Definitions.h
		include/FunctionDispatch.h
        src/Vm.cpp 
		include/Vm.h
		Tests/VmTests.h
		src/VmBackend.cpp
		include/VmBackend.h
		include/LLVMEasyEngine.h
		src/LLVMBackend.cpp
		include/LLVMBackend.h
		include/termcolor.hpp
		include/Console.h

		deps/memory-allocators/src/Allocator.cpp 
		deps/memory-allocators/src/CAllocator.cpp 
		deps/memory-allocators/src/LinearAllocator.cpp 
		deps/memory-allocators/src/StackAllocator 
		deps/memory-allocators/src/PoolAllocator 
		deps/memory-allocators/src/FreeListAllocator.cpp 
        )


if(CMAKE_BUILD_TYPE MATCHES Debug)
	target_compile_definitions(EasyLang PUBLIC _DEBUG=1)
endif()

add_compile_options(
		-std=c++11
		#        -D_DEBUG
		#        -g
)

if( JIT_COMPILER )
	llvm_map_components_to_libnames(llvm_libs Core
			ExecutionEngine
			Interpreter
			MC
			MCJIT
			Support
			nativecodegen)

	# Link against LLVM libraries
	target_link_libraries(EasyLang ${llvm_libs} ${LLVM_SYSTEM_LIBS})
endif()