
#cmake 版本大于3.25.1
cmake_minimum_required(VERSION 3.25.1)

#
project(mymoduo)

#设置 编译生成文件的路径     $(PROJECT_SOURCE_DIR) 源码根目录   
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

#设置调试信息    在原有编译选项后面追加内容  -g
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g ")


#定义参与编译的源文件  src下所有
#define the source file that participate in  compilation
aux_source_directory(. SRC_LIST)

#定义编译类型  SHARED为动态库  STATIC为静态库  最终编译为动态库
add_library(mymoduo SHARED ${SRC_LIST})