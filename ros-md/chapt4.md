chapt4

async_send_request:

客户端调用 `async_send_request()` 发起服务请求后，**不会阻塞当前主线程**，无需原地等待服务端的响应；

主线程可以继续执行其他任务（比如 Qt 界面刷新、话题订阅、定时器回调），当服务端返回响应时，ROS2 会自动触发我们预先设置的「回调函数」，在回调中处理响应结果。

请求例子

```cpp
// 核心：异步发送 + 绑定响应回调函数
client_->async_send_request(
    request,  // 第一步构建的请求对象
    std::bind(&节点类名::响应回调函数, this, std::placeholders::_1) // 响应回调
);
```

同步请求

```cpp
// 同步调用：发起请求后，主线程卡死，直到收到服务端响应/超时
auto response = client->send_request(request);
// 下方代码，必须等上面的响应返回后才会执行
RCLCPP_INFO(this->get_logger(), "同步请求已收到响应");
```

异步请求

```cpp
// 异步调用：发起请求后立刻返回，主线程继续往下执行，无任何等待
client->async_send_request(request, std::bind(&回调函数, this, std::placeholders::_1));
// 下方代码，会和「等待响应」并行执行
RCLCPP_INFO(this->get_logger(), "异步请求已发起，主线程继续工作");
```



使用SharedFuture作为未来接受的结果

在async_send_request函数中，第一个参数是函数的请求，第二个参数是函数（lambda表达式）,入口参数是rclcpp::Client<接口包名>：：SharedFuture

![image-20251231164735446](C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20251231164735446.png)





声明参数的值，可以在rqt的configration中修改，但是只能是修改自己声明的参数的值，而不能修改已经定义好的成员变量的值

**再同步到成员变量**：`this->get_parameter("example_int", example_int_)` 的作用，就是把参数系统里 `example_int` 的值（默认值 10，或外部启动节点时传入的自定义值）**读取并赋值给类的成员变量 `example_int_`**

- 成员变量 `example_int_` 本身只是一个普通的 C++ 变量，**声明时不需要提前赋值**，它的值完全由 `get_parameter` 从 ROS 2 参数系统中获取并填充。
- 如果外部启动节点时通过指令（比如 `ros2 run pkg node --ros-args -p example_int:=20`）覆盖了默认值，那么 `example_int_` 最终会被赋值为 20，而非声明时的 10。

1. **参数声明**：
   - `this->declare_parameter("k_",1.0);`：声明一个名为`k_`的参数，默认值为 1.0（比例系数的默认值）
   - `this->declare_parameter("max_speed",1.0);`：声明一个名为`max_speed`的参数，默认值为 1.0（最大线速度的默认值）
2. **参数获取**：
   - `this->get_parameter("k_",k_);`：从参数服务器获取`k_`的值，赋值给成员变量`k_`
   - `this->get_parameter("max_speed_",max_speed_);`：从参数服务器获取`max_speed_`的值，赋值给成员变量`max_speed_`

创建的节点参数是不是可以在节点之间共享，而c++的成员变量不能共享

![image-20260101191355357](C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20260101191355357.png)

![image-20260101191510846](C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20260101191510846.png)

设置服务通信的消息接口

![image-20260101205558099](C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20260101205558099.png)



列出参数列表：

使用list_parameters

![image-20260101205739866](C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20260101205739866.png)

```cpp
yzh@yzh-virtual-machine:~/chapt4$ ros2 interface show rcl_interfaces/srv/SetParameters
# A list of parameters to set.
Parameter[] parameters
        string name
        ParameterValue value
                uint8 type
                bool bool_value
                int64 integer_value
                float64 double_value
                string string_value
                byte[] byte_array_value
                bool[] bool_array_value
                int64[] integer_array_value
                float64[] double_array_value
                string[] string_array_value

---
# Indicates whether setting each parameter succeeded or not and why.
SetParametersResult[] results
        bool successful
        string reason
    
```

其中response对象封装了以下参数

```cpp
# Indicates whether setting each parameter succeeded or not and why.
SetParametersResult[] results
        bool successful
        string reason
```





###### launch启动多个节点

我们使用python创建launch文件

在.py文件里
generate_launch_description函数是定死的

**def generate_launch_description():#这个函数的返回值就是产生launch的描述**

x = launch_ros.actions.Node(
        package='demo_cpp_service',这个是功能包的名字
        executable="turtle_control",这个功能包中可执行文件的名字
        output='screen',这个是输出的日志
        parameters=[{'max_speed': launch.substitutions.LaunchConfiguration(
  'launch_max_speed', default='2.0')}],

并且output除了可以输出screen文件外还可以输出log文件以及both

把三个不同的x传入到对应的返回文件中

  launch_description = launch.LaunchDescription([
        action_declare_arg_max_spped,
        action_node_turtle_control,
        action_node_patrol_client,
        action_node_turtlesim_node
    ])

以launch_description接收并且返回

```
#导入对应的库
import launch
import launch_ros

def generate_launch_description():#这个函数的返回值就是产生launch的描述
    action_declare_arg_max_spped = launch.actions.DeclareLaunchArgument('launch_max_speed', default_value='2.0')

    action_node_turtle_control = launch_ros.actions.Node(
        package='demo_cpp_service',
        executable="turtle_control",
        output='screen',
        parameters=[{'max_speed': launch.substitutions.LaunchConfiguration(
  'launch_max_speed', default='2.0')}],

    )
    action_node_patrol_client = launch_ros.actions.Node(
        package='demo_cpp_service',
        executable="patrol_client",
        output='log',
    )
    action_node_turtlesim_node = launch_ros.actions.Node(
        package='turtlesim',
        executable='turtlesim_node',
        output='both',
    )
   # 合成启动描述并返回
    launch_description = launch.LaunchDescription([
        action_declare_arg_max_spped,
        action_node_turtle_control,
        action_node_patrol_client,
        action_node_turtlesim_node
    ])
    return launch_description


```

然后，定义好了launch文件以后

在CMakelist.txt文件中，添加以下内容

![image-20260107220057691](C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20260107220057691.png)

编译完成后

执行

![image-20260107220146559](C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20260107220146559.png)

###### launch进行参数赋值

<img src="C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20260108122139372.png" alt="image-20260108122139372" style="zoom: 50%;" />

在小海龟模拟器中，启动命令可以添加参数对其背景颜色进行更改

ros2 run turtlesim turtlesim_node --ros-args -p background_g:=255

于是在launch文件中，声明一个launch文件的参数的动作

launch.actions.DeclareLaunchArgument()

###### 为什么要先写 `DeclareLaunchArgument`？—— 定义 “可外部调节的参数入口”

`launch.actions.DeclareLaunchArgument(...)` 的本质是 **在 launch 文件中 “注册一个参数”**，作用类似给程序开一个 “外部调节旋钮”，而不是把参数值 “写死”。

根据以下代码

```
action_declare_arg_max_spped = launch.actions.DeclareLaunchArgument(
    'launch_max_speed',  # 1. 参数名（相当于“旋钮的名字”）
    default_value='2.0'  # 2. 默认值（旋钮没拧时的初始值）
)
```

1.比如你启动 launch 文件时，想临时把小海龟最大速度改成 3.0，不需要改代码，直接在命令行加参数即可：

```
ros2 launch demo_cpp_service your_launch_file.launch.py launch_max_speed:=3.0
```

2.提供默认值，避免报错

###### launch.substitutions.LaunchConfiguration

这个函数是在prameters的键值对中寻找前面DeclareLaunchArgument声明的这个参数
