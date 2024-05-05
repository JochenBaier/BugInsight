Assuming  https://vcpkg.io/ is installed and VCPKG_ROOT env variable is set. Compiler: VS 2022.

```bat
git clone https://github.com/JochenBaier/BugInsight.git
cd BugInsight
mkdir build
cd build
cmake ..
cmake --build . --config Release --parallel
```

