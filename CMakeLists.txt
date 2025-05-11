cmake_minimum_required(VERSION 3.20)
project(MyCompiler LANGUAGES C CXX)

# Настройка стандарта C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск LLVM 
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_VERSION}")
message(STATUS "LLVM binaries: ${LLVM_TOOLS_BINARY_DIR}")
message(STATUS "LLVM libs: ${LLVM_LIBRARY_DIRS}")

# Добавление LLVM определений и настроек
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Используем современный target-based подход
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

# Указываем необходимые компоненты LLVM
llvm_map_components_to_libnames(llvm_libs
    Core
    Support
    ExecutionEngine
    MC
    X86CodeGen
    Target
    TargetParser
)

# Группировка исходников (лучше явно перечислять файлы)
file(GLOB_RECURSE sources 
    src/*.cpp 
    src/visitors/*.cpp 
    src/tokenization/*.cpp 
    src/parsing/*.cpp
)

# Исключаем тесты
list(FILTER sources EXCLUDE REGEX ".*_test\\.cpp$")

# Создание исполняемого файла
add_executable(${PROJECT_NAME} ${sources})

# Линковка с LLVM
target_link_libraries(${PROJECT_NAME} PRIVATE ${llvm_libs})

# Дополнительные настройки
if(LLVM_ENABLE_RTTI)
    target_compile_options(${PROJECT_NAME} PRIVATE "-frtti")
else()
    target_compile_options(${PROJECT_NAME} PRIVATE "-fno-rtti")
endif()

# Цели для запуска
add_custom_target(run ALL
    COMMAND ${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/example.txt ${CMAKE_SOURCE_DIR}/ast.txt
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running parser with example input"
)

# Установка (опционально)
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
