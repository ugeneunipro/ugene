project(qtscript LANGUAGES CXX)

find_package(Qt5 REQUIRED Core)

file(GLOB JSC_RUNTIME_SOURCES 3rdparty/javascriptcore/JavaScriptCore/runtime/*.cpp)
file(GLOB JSC_GENERATED_SOURCES 3rdparty/javascriptcore/JavaScriptCore/generated/*.cpp)
file(GLOB JSC_JIT_SOURCES 3rdparty/javascriptcore/JavaScriptCore/jit/*.cpp)
file(GLOB JSC_YARR_SOURCES 3rdparty/javascriptcore/JavaScriptCore/yarr/*.cpp)

file(GLOB JSC_WTF_SOURCES 3rdparty/javascriptcore/JavaScriptCore/wtf/*.cpp)
file(GLOB JSC_WTF_QT_SOURCES 3rdparty/javascriptcore/JavaScriptCore/wtf/qt/*.cpp)
list(APPEND JSC_WTF_SOURCES ${JSC_WTF_QT_SOURCES})
file(GLOB JSC_WTF_UNICODE_SOURCES 3rdparty/javascriptcore/JavaScriptCore/wtf/unicode/*.cpp)
list(APPEND JSC_WTF_SOURCES ${JSC_WTF_UNICODE_SOURCES})


file(GLOB API_SOURCES script/api/*.cpp)
file(GLOB BRIDGE_SOURCES script/bridge/*.cpp)
file(GLOB PARSER_SOURCES script/parser/*.cpp)

set(SOURCES
        3rdparty/javascriptcore/JavaScriptCore/pcre/pcre_compile.cpp
        3rdparty/javascriptcore/JavaScriptCore/pcre/pcre_exec.cpp
        3rdparty/javascriptcore/JavaScriptCore/pcre/pcre_tables.cpp
        3rdparty/javascriptcore/JavaScriptCore/pcre/pcre_ucp_searchfuncs.cpp
        3rdparty/javascriptcore/JavaScriptCore/pcre/pcre_xclass.cpp
        3rdparty/javascriptcore/JavaScriptCore/API/JSBase.cpp
        3rdparty/javascriptcore/JavaScriptCore/API/JSCallbackConstructor.cpp
        3rdparty/javascriptcore/JavaScriptCore/API/JSCallbackFunction.cpp
        3rdparty/javascriptcore/JavaScriptCore/API/JSCallbackObject.cpp
        3rdparty/javascriptcore/JavaScriptCore/API/JSClassRef.cpp
        3rdparty/javascriptcore/JavaScriptCore/API/JSContextRef.cpp
        3rdparty/javascriptcore/JavaScriptCore/API/JSObjectRef.cpp
        3rdparty/javascriptcore/JavaScriptCore/API/JSStringRef.cpp
        3rdparty/javascriptcore/JavaScriptCore/API/JSValueRef.cpp
        3rdparty/javascriptcore/JavaScriptCore/API/OpaqueJSString.cpp
        3rdparty/javascriptcore/JavaScriptCore/assembler/ARMAssembler.cpp
        3rdparty/javascriptcore/JavaScriptCore/assembler/MacroAssemblerARM.cpp
        3rdparty/javascriptcore/JavaScriptCore/bytecode/CodeBlock.cpp
        3rdparty/javascriptcore/JavaScriptCore/bytecode/JumpTable.cpp
        3rdparty/javascriptcore/JavaScriptCore/bytecode/Opcode.cpp
        3rdparty/javascriptcore/JavaScriptCore/bytecode/SamplingTool.cpp
        3rdparty/javascriptcore/JavaScriptCore/bytecode/StructureStubInfo.cpp
        3rdparty/javascriptcore/JavaScriptCore/bytecompiler/BytecodeGenerator.cpp
        3rdparty/javascriptcore/JavaScriptCore/bytecompiler/NodesCodegen.cpp
        3rdparty/javascriptcore/JavaScriptCore/debugger/DebuggerActivation.cpp
        3rdparty/javascriptcore/JavaScriptCore/debugger/DebuggerCallFrame.cpp
        3rdparty/javascriptcore/JavaScriptCore/debugger/Debugger.cpp
        3rdparty/javascriptcore/JavaScriptCore/interpreter/CallFrame.cpp
        3rdparty/javascriptcore/JavaScriptCore/interpreter/Interpreter.cpp
        3rdparty/javascriptcore/JavaScriptCore/interpreter/RegisterFile.cpp
        3rdparty/javascriptcore/JavaScriptCore/parser/Lexer.cpp
        3rdparty/javascriptcore/JavaScriptCore/parser/Nodes.cpp
        3rdparty/javascriptcore/JavaScriptCore/parser/ParserArena.cpp
        3rdparty/javascriptcore/JavaScriptCore/parser/Parser.cpp
        3rdparty/javascriptcore/JavaScriptCore/profiler/Profile.cpp
        3rdparty/javascriptcore/JavaScriptCore/profiler/ProfileGenerator.cpp
        3rdparty/javascriptcore/JavaScriptCore/profiler/ProfileNode.cpp
        3rdparty/javascriptcore/JavaScriptCore/profiler/Profiler.cpp
        ${JSC_GENERATED_SOURCES}
        ${JSC_JIT_SOURCES}
        ${JSC_RUNTIME_SOURCES}
        ${JSC_WTF_SOURCES}
        ${JSC_YARR_SOURCES}
        ${API_SOURCES}
        ${BRIDGE_SOURCES}
        ${PARSER_SOURCES}
)

# Create a library
add_library(qtscript SHARED ${SOURCES})

# Add include directories
target_include_directories(qtscript PRIVATE
        3rdparty/javascriptcore
        3rdparty/javascriptcore/JavaScriptCore
        3rdparty/javascriptcore/JavaScriptCore/API
        3rdparty/javascriptcore/JavaScriptCore/ForwardingHeaders
        3rdparty/javascriptcore/JavaScriptCore/assembler
        3rdparty/javascriptcore/JavaScriptCore/bytecode
        3rdparty/javascriptcore/JavaScriptCore/bytecompiler
        3rdparty/javascriptcore/JavaScriptCore/debugger
        3rdparty/javascriptcore/JavaScriptCore/generated
        3rdparty/javascriptcore/JavaScriptCore/interpreter
        3rdparty/javascriptcore/JavaScriptCore/jit
        3rdparty/javascriptcore/JavaScriptCore/parser
        3rdparty/javascriptcore/JavaScriptCore/pcre
        3rdparty/javascriptcore/JavaScriptCore/profiler
        3rdparty/javascriptcore/JavaScriptCore/runtime
        3rdparty/javascriptcore/JavaScriptCore/wrec
        3rdparty/javascriptcore/JavaScriptCore/wtf
        3rdparty/javascriptcore/JavaScriptCore/wtf/unicode
        3rdparty/javascriptcore/JavaScriptCore/yarr
        script
        script/api
        script/bridge
        script/moc
        script/parser
)


# Add compile definitions
target_compile_definitions(qtscript PRIVATE
        BUILDING_JavaScriptCore
        BUILDING_QT__=1
        BUILDING_WTF
        ENABLE_JIT=0
        ENABLE_JSC_MULTIPLE_THREADS=0
        JSC=QTJSC
        JS_NO_EXPORT
        LOG_DISABLED=1
        QLALR_NO_QSCRIPTGRAMMAR_DEBUG_INFO
        QT_BUILDING_QT
        QT_BUILD_SCRIPT_LIB
        QT_NO_NARROWING_CONVERSIONS_IN_CONNECT
        USE_SYSTEM_MALLOC=1
        WTF=QTWTF
        WTF_CHANGES=1
        WTF_USE_ACCELERATED_COMPOSITING
        WTF_USE_JAVASCRIPTCORE_BINDINGS=1
        jscyyerror=qtjscyyerror
        jscyylex=qtjscyylex
        jscyyparse=qtjscyyparse
)

# Suppress all warnings.
target_compile_options(qtscript PRIVATE
        $<$<CXX_COMPILER_ID:Clang>:-w>
        $<$<CXX_COMPILER_ID:GNU>:-w>
        $<$<CXX_COMPILER_ID:MSVC>:/w>
)

target_link_libraries(qtscript PRIVATE Qt5::Core)
if (WIN32)
    target_link_libraries(qtscript PRIVATE winmm.lib)
elseif (APPLE)
    find_library(COREFOUNDATION_LIBRARY CoreFoundation REQUIRED)
    target_link_libraries(qtscript PRIVATE ${COREFOUNDATION_LIBRARY})
endif ()