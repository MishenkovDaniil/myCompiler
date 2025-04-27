# cmake_minimum_required(VERSION 3.14)
# project(CompilersCourse LANGUAGES CXX)

# set(CMAKE_CXX_STANDARD 23)

# set(CMAKE_EXECUTABLE_SUFFIX ".out")
# set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# set(CMAKE_STATIC_LIBRARY_PREFIX "")
# set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")
# set(CMAKE_CXX_FLAGS_DEBUG "-fsanitize=undefined -fsanitize=address -O0 -g")
# set(CMAKE_CXX_FLAGS_RELEASE "-O2")

# include_directories(${PROJECT_SOURCE_DIR}/include)

# add_subdirectory(src)
# add_subdirectory(test)


cmake_minimum_required(VERSION 3.20)
project(YourCompiler LANGUAGES CXX)

# Настройка стандарта C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск LLVM
find_package(LLVM REQUIRED CONFIG)
message(STATUS "LLVM version: ${LLVM_VERSION}")
message(STATUS "LLVM libraries: ${LLVM_LIBS}")

# Настройка путей LLVM
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Указываем компоненты LLVM, которые будем использовать
llvm_map_components_to_libnames(llvm_libs
    Core          # Базовые функции
    Support       # Утилиты
    ExecutionEngine # Для JIT (опционально)
    MC            # Машинный код
    X86CodeGen    # Генерация кода для x86 (или другие цели)
)

# Группировка исходников
file(GLOB_RECURSE sources 
    src/*.cpp 
    src/visitors/*.cpp 
    src/tokenization/*.cpp 
    src/parsing/*.cpp
)

# Исключаем тесты (если есть)
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