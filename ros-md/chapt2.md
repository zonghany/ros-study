##### chapt2

```
ros2 node list         #查看当前节点
ros2 node info /节点名 #查看当前节点信息
```



ros2创建功能包

```c
ros2 pkg create --build-type ament_cmake --license Apache-2.0 demo_ros_cpp
```

创建cpp代码

在创建的CMakeLists.txt文件中进行修改配置

```cPP
cmake_minimum_required(VERSION 3.8)
project(demo_ros_cpp)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)

add_executable(cpp_node src/cpp_node.cpp)

# message(status ${rclcpp_INCLUDE_DIRS})
# target_include_directories(cpp_node PUBLIC ${rclcpp_INCLUDE_DIRS})#头文件包含
# target_link_libraries(cpp_node ${rclcpp_LIBRARIES})#库文件链接

ament_target_dependencies(cpp_node rclcpp)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()

```

其中

```c
# target_include_directories(cpp_node PUBLIC ${rclcpp_INCLUDE_DIRS})#头文件包含
# target_link_libraries(cpp_node ${rclcpp_LIBRARIES})#库文件链接
```

可以替代为

```c
ament_target_dependencies(cpp_node rclcpp)
```

之后在功能包中创建build文件

运行cmake命令将CMakeList.txt构建为Makefile文件

在build文件中，再使用make指令运行makefile，最后生成可执行文件

清除缓存就在`build`中清除就可以

一般来说，find_package(rclcpp REQUIRED)中包含了find_package(ament_cmake REQUIRED)





```
ldd cpp_node #用于查看cpp_node用了哪些库
printenv | grep AME查看这个环境变量有没有加入要编译的可执行程序的路径
```

运行`install/setup.bash`调用export命令去修改这个环境变量

之后再找到这个ros2 pkg prefix 的路径

根据这个输出的路径找lib文件，于是再CMakeLists中增加

```c
intsall(TARGETS
	cpp_node
	lib/${PROJECT_NAME}  #变量是工程的名字，为了可以反复用，实际上也可以是project里面的参数
)
```

后面在目录下构建而不是功能包里构建

```
ros2 run [功能包] [可执行文件] 
```

![image-20251216144424304](C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20251216144424304.png)

```
include --c++头文件
src --用于存放资源文件比如节点代码
CMakeLists.txt--构建资源文件，添加指令即可完成对依赖的查找可执行文件的添加安装等工作
LICENSE --协议内容
package --功能包的清单内容
```



`报错`

```
CMake Error: The current CMakeCache.txt directory
/home/yzh/.../ws_ros/src/pkg01_helloworld_cpp/CMakeCache.txt
is different than the directory
/home/yzh/.../ws_ros/pkg01_helloworld_cpp
where CMakeCache.txt was created
```

 **意思是：**

- `CMakeCache.txt` 曾经在 **错误的目录** 下生成过
- 现在你又在 **另一个目录** 里用 colcon 重新配置
- CMake 发现“缓存路径对不上”，为了防止编译结果乱写，**直接拒绝继续**

```
在根目录中清除cmake缓存
find src -name "CMakeCache.txt" -delete
find src -name "CMakeFiles" -type d -exec rm -rf {} +
```

 因为 CMake 会把“构建目录的绝对路径”写死在 `CMakeCache.txt` 里，而 colcon 要求每个包只能在 `build/<pkg>` 目录里构建。
 你在 `src` 里跑过 cmake，相当于“告诉 CMake：构建目录就是 src”，
 之后 colcon 再说“不对，构建目录应该是 build/pkg”，
 两个路径冲突，CMake 为了防止灾难，直接报错退出。

