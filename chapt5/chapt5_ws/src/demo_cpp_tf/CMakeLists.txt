cmake_minimum_required(VERSION 3.8)
project(demo_cpp_tf)

# 查找依赖包
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(rclcpp REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)

# 1. 编译静态TF发布节点
add_executable(static_tf_broadcaster src/static_tf_broadcaster.cpp)
ament_target_dependencies(static_tf_broadcaster
  rclcpp tf2 tf2_ros geometry_msgs tf2_geometry_msgs
)

# 2. 编译动态TF发布节点
add_executable(dynamic_tf_broadcaster src/dynamic_tf_broadcaster.cpp)
ament_target_dependencies(dynamic_tf_broadcaster
  rclcpp tf2 tf2_ros geometry_msgs tf2_geometry_msgs
)

# 3. 编译TF查询节点
add_executable(tf_listener src/tf_listener.cpp)
ament_target_dependencies(tf_listener
  rclcpp tf2 tf2_ros geometry_msgs tf2_geometry_msgs
)

# 安装可执行文件（让ros2 run能找到）
install(TARGETS
  static_tf_broadcaster
  dynamic_tf_broadcaster
  tf_listener
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()