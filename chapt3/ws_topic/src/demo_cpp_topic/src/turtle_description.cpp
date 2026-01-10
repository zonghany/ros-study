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