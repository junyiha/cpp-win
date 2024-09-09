## 简介

+ C++功能测试项目

## 构建配置

+ cmake依赖的模块配置文件为 `.vscode/path.cmake`，这个是用来配置依赖库的路径，内容如下
```cmake
# for base
set(Base_DIR "C:/programs/packages/")

# for opencv
set(OpenCV_DIR "C:/programs/packages/opencv4.55/build")

# for qt
set(Qt_DIR "C:/programs/Qt514/5.14.0/msvc2017_64/lib/cmake/")
```