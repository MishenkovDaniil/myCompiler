## Запуск проекта

- скачайте [`llvm`](https://llvm.org/docs/GettingStarted.html). Например,
``` bash 
git clone --depth 1 https://github.com/llvm/llvm-project.git
cd llvm-project
cmake -G Ninja llvm \
    -DCMAKE_BUILD_TYPE=Release \
    -DLLVM_TARGETS_TO_BUILD="X86;AArch64" \
    -DLLVM_ENABLE_PROJECTS="clang;lld" \
    -DLLVM_PARALLEL_LINK_JOBS=1 \
    -DCMAKE_BUILD_WITH_INSTALL_RPATH=1 \
    -DLLVM_ENABLE_RTTI=ON
cmake --build . 
``` 

***Пример приведен для генератора Ninja, можно подробнее узнать про другие на официальном сайте** [`llvm`](https://www.llvm.org/docs/GettingStarted.html)

- создайте из корня проекта директорию для хранения файлов сборки
``` bash 
    mkdir build && cd build
```
- соберите файл с указанием пути до llvm 
``` bash 
    cmake .. -DLLVM_DIR=path/to/llvm-project/cmake
```
- соберите проект
``` bash 
    cmake --build .
```
- запустите выполняемый файл 
``` bash 
    clang output.ll -o MyCompiler
```
или из корня проекта
``` bash 
    ./program <имя файла с кодом> <имя файла для вывода ast-дерева разбора>
```
