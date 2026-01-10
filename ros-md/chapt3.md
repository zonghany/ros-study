#### chapt3

###### ros2中的话题和消息接口相关命令

![image-20251221140614483](C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20251221140614483.png)

```
ros2 run turtlesim turtlesim_node
ros2 node info /turtlesim
#查看节点找到他的话题
```

![image-20251221141011194](C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20251221141011194.png)

```
ros2 topic echo /turtle1/pose
#echo加上话题的名字，用于查看话题的内容
```

![image-20251221141126792](C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20251221141126792.png)

![image-20251221142025220](C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20251221142025220.png)

```
x，y，乌龟的朝向(弧度)，线速度，角速度（逆时针大于0，顺时针小于0）
```



```
ros2 topic info /turtle1/cmd_vel -v
#查看话题的消息接口第二种方法（第一种是使用ros2 node info <话题名字>）
```

![image-20251221141329792](C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20251221141329792.png)

------------------------------------------------------------------------------

![image-20251221141354777](C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20251221141354777.png)

```
ros2 interface show geometry_msgs/msg/Twist
#查看详细的消息接口的订义的内容
```

![image-20251221141537430](C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20251221141537430.png)

###### 查看节点的信息加上节点的名字

![image-20251221141632287](C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20251221141632287.png)



```
ros2 topic pub /turtle1/cmd_vel geometry_msgs/msg/Twist "{linear: {x: 0.5, y: 0.0} }"
```

---------------------------------------------------------------------------------

###### 定时器的使用

```
#include "rclcpp/rclcpp.hpp"
#include <chrono>

using namespace std::chrono_literals;  // 使用时间字面量，如1000ms

class SimpleTimerNode : public rclcpp::Node
{
public:
    SimpleTimerNode() : Node("simple_timer_node")
    {
        // 创建一个定时器，每隔1秒执行一次
        timer_ = this->create_wall_timer(
            1000ms,  // 时间间隔：1000毫秒 = 1秒
            std::bind(&SimpleTimerNode::timer_callback, this)  // 回调函数
        );
        
        RCLCPP_INFO(this->get_logger(), "定时器已创建，间隔1秒");
    }
    
private:
    void timer_callback()
    {
        RCLCPP_INFO(this->get_logger(), "定时器回调执行！");
    }
    
    rclcpp::TimerBase::SharedPtr timer_;  // 定时器指针
};
```



###### 使用c++代码创建节点控制小海龟

首先创建好目录在src目录下创建功能包

```
ros2 pkg create demo_cpp_topic --build-type ament_cmake --dependencies rclcpp geometry_msgs turtlesim --license Apache-2.0
```



```
#include <iostream>
#include "rclcpp/rclcpp.hpp"  //添加rclcpp库
#include "geometry_msgs/msg/twist.hpp"  //引入话题控制小海龟
#include <chrono>

using namespace std::chrono_literals;//增加这个头文件以后可以使用时间字面量比如ms
//创建TurtleCircleNode类继承rclcpp::Node类，取消隐式转化

class TurtleCircleNode: public rclcpp::Node
{
    private:
        rclcpp::TimerBase::SharedPtr timer_;
        //rclcpp下的发布者
        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr Publisher_; //创建定时器，创建发布者的智能指针
    public:
        explicit TurtleCircleNode(const std::string& node_name):Node(node_name)
        { 
            //create_pubisher<消息的类型>("话题的名字",Qos(默认预设就是10))
            Publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel",10);
           
            timer_ = this->create_wall_timer(1000ms,std::bind(&TurtleCircleNode::timer_callback,this)); 
        };

        void timer_callback()
        {   
            auto msg  = geometry_msgs::msg::Twist();
            msg.linear.x=1.0;
            msg.angular.z=0.5;
            Publisher_->publish(msg);
            RCLCPP_INFO(this->get_logger(), "发布速度命令: 线速度=%.1f, 角速度=%.1f", 
                   msg.linear.x, msg.angular.z);
        };
};

int main(int argc,char **argv)
{
    rclcpp::init(argc,argv);
    auto node = std::make_shared<TurtleCircleNode>("turtle_node");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
```

修改CMakeList.txt

```
包文件已经在上面的功能包代码中自动创建
手动要做的如下：

#添加可执行文件
add_executable(turtle_circle src/turtle_circle.cpp)
#添加依赖
ament_target_dependencies(turtle_circle rclcpp geometry_msgs)
#添加目标路径
install(TARGETS turtle_circle
DESTINATION lib/${PROJECT_NAME}
)
```

构建成功以后打开小乌龟模拟器，即可运行

```
ros2 run turtlesim turtlesim_node
```

###### 发布者和订阅者的方法模板

**发布者相关**

```
// 创建发布者
template<typename MessageT>
std::shared_ptr<rclcpp::Publisher<MessageT>>
create_publisher(
    const std::string &topic_name,
    const rclcpp::QoS &qos,
    rclcpp::PublisherOptionsWithAllocator<AllocatorT> options = ...)
```

**订阅者相关**

```
// 创建订阅者
template<typename MessageT>
std::shared_ptr<rclcpp::Subscription<MessageT>>
create_subscription(
    const std::string &topic_name,
    const rclcpp::QoS &qos,
    CallbackT &&callback,
    rclcpp::SubscriptionOptionsWithAllocator<AllocatorT> options = ...)
```

```
反正切函数-已知斜率求角度
```

![image-20251223211639528](C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20251223211639528.png)

###### 订阅者

实现订阅者订阅消息

当小海龟位置改变时，就发送小海龟位置到命令行

```
#include <iostream>
#include "rclcpp/rclcpp.hpp"  //添加rclcpp库
#include "geometry_msgs/msg/twist.hpp"  //引入话题控制小海龟
#include "turtlesim/msg/pose.hpp"
#include <chrono>

using namespace std::chrono_literals;//增加这个头文件以后可以使用时间字面量比如ms
//创建TurtleCircleNode类继承rclcpp::Node类，取消隐式转化

class TurtleCircleNode: public rclcpp::Node
{
    private:
        double x_,y_;
        rclcpp::TimerBase::SharedPtr timer_;
        //rclcpp下的发布者
        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr Publisher_; //创建定时器，创建发布者的智能指针
        rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr Subscription_; //创建定时器，创建发布者的智能指针
    public:
        explicit TurtleCircleNode(const std::string& node_name):Node(node_name)
        { 
            //create_pubisher<消息的类型>("话题的名字",Qos(默认预设就是10))
            Publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel",10);
            Subscription_ = this->create_subscription<turtlesim::msg::Pose>("/turtle1/pose",10,std::bind(&
                TurtleCircleNode::on_pose_received,this,std::placeholders::_1));
           // timer_ = this->create_wall_timer(1000ms,std::bind(&TurtleCircleNode::timer_callback,this)); 
        };

        void on_pose_received(const turtlesim::msg::Pose::SharedPtr pose)
        {   
            x_=pose->x;
            y_=pose->y;
            if(x_!=pose->x||y_!=pose->y){
                    RCLCPP_INFO(this->get_logger(), "小海龟位置改变: x=%.1f, y=%.1f", 
                   pose->x, pose->y);
            }
            x_=pose->x;
            y_=pose->y;

        };
};

int main(int argc,char **argv)
{
    rclcpp::init(argc,argv);
    auto node = std::make_shared<TurtleCircleNode>("turtle_node");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
```

<img src="C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20251223221307576.png" alt="image-20251223221307576" style="zoom:67%;" />

CMakeList.txt的修改

```
add_executable(turtle_description src/turtle_description.cpp)

ament_target_dependencies(turtle_description rclcpp geometry_msgs turtlesim)

install(TARGETS 
turtle_circle
learn_timer
turtle_description
DESTINATION lib/${PROJECT_NAME}
)
```

###### 关于发布者和订阅者的闭环控制



###### 关于包引用的一些格式定义

![image-20251224171431534](C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20251224171431534.png)

内置的消息清单

![image-20251224171803779](C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20251224171803779.png)
