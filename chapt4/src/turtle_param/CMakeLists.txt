cmake_minimum_required(VERSION 3.8)
project(turtle_param)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 查找动态参数必备依赖
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rcl_interfaces REQUIRED)

# 编译C++节点源码
add_executable(dynamic_param_node src/dynamic_param_node.cpp)
ament_target_dependencies(
  dynamic_param_node
  rclcpp
  rcl_interfaces
)

# 安装节点到系统路径（ros2 run能找到）
install(TARGETS
  dynamic_param_node
  DESTINATION lib/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()