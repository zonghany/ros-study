##### chapt6

<img src="C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20260107185532364.png" alt="image-20260107185532364" style="zoom:33%;" />

ctrl+alt+t开启终端

创建文件，创建包

```c++
ros2 pkg create fishbot_description --build-type ament_cmake --license Apache-2.0
```

创建urdf目录，创建first_robot.urdf

构建urdf文件

```
robot
link
visual
origin
geometry
cylinder
material
color
```

构建完成后执行

```
urdf_to_graphviz xx.urdf xxname
```

![image-20260107194852669](C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20260107194852669.png)

###### 2.rviz中显示机器人

在chapt6中打开rviz2



1.点击add，再选择模块类型robotmodel

2.然后在描述来源Description Source中选择File（因为默认是通过话题来接收机器人的URDF内容），再在Description File中选择xx.urdf文件

![image-20260108120327263](C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20260108120327263.png)

其中 no transform指的是没有tf变换，不过由于没有在urdf文件中描述map和base_link以及imu_link的关系，

![image-20260108120420715](C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20260108120420715.png)所以会显示找不到，所以在fixed Frame中改选一下就可以![image-20260108120605811](C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20260108120605811.png)



之后下载

sudo apt install ros-$ROS_DISTRO-robot-state-publisher和

sudo apt install ros-$ROS_DISTRO-joint-state-publisher

运行这两个节点

之后再建launch的目录

###### 创建launch文件：

![image-20260108192420195](C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20260108192420195.png)

首先要导包，让其可以进行路径拼接

![image-20260108182940860](C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20260108182940860.png)，目的是为了找到first_robot.urdf文件

![image-20260108182715156](C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20260108182715156.png)

通过get_package_share_directory寻找

![image-20260108183000646](C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20260108183000646.png)

作为发布者，发布urdf文件，但是在launch中只有路径，此时把路径转化为urdf内容

```
robot_description = launch_ros.parameter_descriptions.ParameterValue(
        launch.substitutions.Command(
            ['xacro ', launch.substitutions.LaunchConfiguration('model')]),
        value_type=str)
```

等价于以下的步骤

```
# 解析文件并输出URDF文本
xacro /path/to/first_robot.urdf

# 把输出的文本作为robot_description参数传给robot_state_publisher
ros2 run robot_state_publisher robot_state_publisher --ros-args -p robot_description:="$(xacro /path/to/first_robot.urdf)"
```

- 启动 `rviz2` 节点时，通过 `arguments=['-d', default_rviz_config_path]` 将该路径作为参数传给 `rviz2` 可执行文件；
- **RViz 进程启动时**，才会根据 `-d` 参数加载该路径下的 `display_model.rviz` 配置文件。

```python
import launch
import launch_ros
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    # 获取默认路径
    urdf_tutorial_path = get_package_share_directory('fishbot_description')
    default_model_path = urdf_tutorial_path + '/urdf/first_robot.urdf'
    default_rviz_config_path = urdf_tutorial_path + '/config/rviz/display_model.rviz'
    # 为 Launch 声明参数
    action_declare_arg_mode_path = launch.actions.DeclareLaunchArgument(
        name='model', default_value=str(default_model_path),
        description='URDF 的绝对路径')
    # 获取文件内容生成新的参数
    robot_description = launch_ros.parameter_descriptions.ParameterValue(
        launch.substitutions.Command(
            ['xacro ', launch.substitutions.LaunchConfiguration('model')]),
        value_type=str)
    # 状态发布节点
    robot_state_publisher_node = launch_ros.actions.Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        parameters=[{'robot_description': robot_description}]
    )
    # 关节状态发布节点
    joint_state_publisher_node = launch_ros.actions.Node(
        package='joint_state_publisher',
        executable='joint_state_publisher',
    )
    # RViz 节点
    rviz_node = launch_ros.actions.Node(
        package='rviz2',
        executable='rviz2',
        arguments=['-d', default_rviz_config_path]
    )
    return launch.LaunchDescription([
        action_declare_arg_mode_path,
        joint_state_publisher_node,
        robot_state_publisher_node,
        rviz_node
    ])
```

`ParameterValue` 几乎是 ROS 2 Launch 文件中给节点传递**动态参数**的标准方式，尤其常见于：

- 传递机器人 URDF/SDF 描述（通过 xacro 动态生成）；
- 传递从 Launch 参数（`LaunchConfiguration`）解析的动态值；
- 传递系统命令（`Command`）执行结果作为参数；
- 确保参数类型与节点期望的类型严格匹配（避免隐式类型转换错误）。

在这个例子中，ParameterValue是传入一个命令，返回命令的执行结果并且这个结果定义为str类型的

```
 # 获取文件内容生成新的参数
    robot_description = launch_ros.parameter_descriptions.ParameterValue(
        launch.substitutions.Command(
            ['xacro ', launch.substitutions.LaunchConfiguration('model')]),
        value_type=str)
```

ros2 run rviz2 rviz2

xacro [文件路径]可以把xacro文件转化为urdf文件

xacro /home/yzh/project-ros/chapt6/src/fishbot_description/urdf/first_robot.xacro

![image-20260108194048392](C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20260108194048392.png)

运行时指定文件的路径

执行ros2 launch fishbot_description display_robot.taunch.py后进行了一些设置



<img src="C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20260108194923550.png" alt="image-20260108194923550" style="zoom:50%;" />

<img src="C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20260108194943181.png" alt="image-20260108194943181" style="zoom:50%;" />

###### 创建机器人加传感器部件

首先，创建好urdf.xacro文件后可以构建编译一下

###### 创建机器人执行器部件

###### 贴合地面和虚拟部件

###### 添加部件的碰撞属性以及质量和惯性

![image-20260109131126917](C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20260109131126917.png)

##### Gazebo

```
sudo apt install gazebo
```

```
mkdir -p ~/.gazebo
cd ~/.gazebo
git clone https://gitee.com/ohhuo/gazebo_models.git
~/.gazebo/models
rm -rf ~/.gazebo/models/.git  #以防误识别为模型
```

editor,building editor

![image-20260109155016267](C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20260109155016267.png)

```
sudo apt install ros-$ROS_DISTRO-gazebo-ros-pkgs
```

这个功能包同时提供了launch以及可执行文件供我们使用，以方便我们打开gazebo加载世界模型

**首先，在gazebo中只能加载sdf格式的文件，然后先把xacro格式的文件加载为URDF格式的文件，紧接着，把urdf格式的文件转换为sdf格式的文件供gazebo进行一个加载**





gazebo_sim.launch.py和display_robot.launch.py

提供xacro把xacro文件转换成urdf文件

设置default_xacro_path路径，对应fishbot.urdf.xacro

设置default_gazebo_world_path路径，对应custom_room.world

gazebo_sim.launch文件内容如下

```
import launch
import launch_ros
from ament_index_python.packages import get_package_share_directory
from launch.launch_description_sources import PythonLaunchDescriptionSource

def generate_launch_description():
    # 获取默认路径
    robot_name_in_model = "fishbot"
    urdf_tutorial_path = get_package_share_directory('fishbot_description')
    default_model_path = urdf_tutorial_path + '/urdf/fishbot/fishbot.urdf.xacro'
    default_world_path = urdf_tutorial_path + '/world/custom_room.world'
    # 为 Launch 声明参数
    action_declare_arg_mode_path = launch.actions.DeclareLaunchArgument(
        name='model', default_value=str(default_model_path),
        description='URDF 的绝对路径')
    # 获取文件内容生成新的参数
    robot_description = launch_ros.parameter_descriptions.ParameterValue(
        launch.substitutions.Command(
            ['xacro ', launch.substitutions.LaunchConfiguration('model')]),
        value_type=str)
  	
    robot_state_publisher_node = launch_ros.actions.Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        parameters=[{'robot_description': robot_description}]
    )

    # 通过 IncludeLaunchDescription 包含另外一个 launch 文件
    launch_gazebo = launch.actions.IncludeLaunchDescription(
        PythonLaunchDescriptionSource([get_package_share_directory(
            'gazebo_ros'), '/launch', '/gazebo.launch.py']),
      	# 传递参数
        launch_arguments=[('world', default_world_path),('verbose','true')]
    )
    # 请求 Gazebo 加载机器人
    spawn_entity_node = launch_ros.actions.Node(
        package='gazebo_ros',
        executable='spawn_entity.py',
        arguments=['-topic', '/robot_description',
                   '-entity', robot_name_in_model, ])
    
    
    return launch.LaunchDescription([
        action_declare_arg_mode_path,
        robot_state_publisher_node,
        launch_gazebo,
        spawn_entity_node
     
    ])
```

期间遇到的问题

```
[gzserver-2] [Err] [Master.cc:96] EXCEPTION: Unable to start server[bind: Address already in use]. There is probably another Gazebo process running.
原因：Gazebo 默认使用 11345 端口作为通信主端口，当前该端口已被其他程序（最可能是之前未正常关闭的 Gazebo 进程）占用，导致新的 gzserver 无法绑定端口启动。
影响：Gazebo 核心服务无法启动，后续的机器人模型加载（spawn_entity）也会因找不到服务端而卡住。
以及
[gzclient-3] [Err] [RenderEngine.cc:749] Can't open display: 
[gzclient-3] [Msg] Could not load the Qt platform plugin "xcb" in "" even though it was found.
原因：
图形显示环境问题：Can't open display 表示当前终端没有正确配置图形显示（比如 SSH 远程连接未开启 X11 转发，或本地桌面环境异常）；
Qt 依赖缺失：xcb 是 Qt 的图形后端插件，虽然系统能找到该插件，但缺少其依赖库（如 libxcb-xinerama0、libxcb-render-util0 等），导致无法初始化界面。
影响：即使服务端启动成功，也无法通过图形界面查看 Gazebo 仿真场景。
```



使用gazebo标签对颜色进行保护![image-20260109191139933](C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20260109191139933.png)

| 目标对象类型 | 支持的核心子标签                                             | 作用说明                               |
| ------------ | ------------------------------------------------------------ | -------------------------------------- |
| `link`零件   | `<material>`、`<mass>`、`<inertia>`、`<selfCollide>`、`<texture>` | 配置视觉材质、质量、转动惯量、自碰撞等 |
| `joint`关节  | `<jointDamping>`、`<jointFriction>`、`<maxEffort>`、`<maxVelocity>` | 配置关节阻尼、摩擦、最大力、最大速度   |

```
 <gazebo reference="camera_link">
            <material>Gazebo/Blue</material>
        </gazebo>
```

<img src="C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20260109194339509.png" alt="image-20260109194339509" style="zoom:67%;" />

分别表示摩擦系数（mu1切向，mu2法向）（默认的都是1.0），kp表示接触的刚度系数，kd表示阻尼系数

###### 除此之外，gazebo还提供了其他的仿真插件

首先，使用插件控制机器人

![image-20260109204024980](C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20260109204024980.png)

使用激光雷达

<img src="C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20260109205434845.png" alt="image-20260109205434845" style="zoom:67%;" />