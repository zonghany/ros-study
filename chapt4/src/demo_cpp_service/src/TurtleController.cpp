#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "turtlesim/msg/pose.hpp"
#include "chapt4_interfaces/srv/patrol.hpp"
#include <cmath>

using Patrol = chapt4_interfaces::srv::Patrol;
const int SUCCESS = 1;
const int FAIL = 0;

class TurtleController : public rclcpp::Node
{
private:
    double last_x_, last_y_;  // 上一次位置
    double target_x_, target_y_;  // 目标位置
    double k_;  // 比例系数
    double max_speed_;  // 最大线速度
    double max_angular_speed_;  // 最大角速度
    
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr subscription_;
    rclcpp::Service<Patrol>::SharedPtr patrol_server_;  // 巡逻服务服务器
    
public:
    explicit TurtleController(): Node("turtlectrl")
    { 
        this->declare_parameter("k",1.0);
        this->declare_parameter("max_speed",1.0);
        this->get_parameter("k",k_);
        this->get_parameter("max_speed",max_speed_);
        // 初始化控制参数
        last_x_ = -1.0;  // 初始标记
        last_y_ = -1.0;
        target_x_ = 8.0;  // 初始目标位置
        target_y_ = 8.0;
        k_ = 1.0;  // 比例系数
        max_speed_ = 2.0;  // 最大线速度
        max_angular_speed_ = 2.0;  // 最大角速度
        
        // 创建发布者和订阅者
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>(
            "/turtle1/cmd_vel", 10);
        subscription_ = this->create_subscription<turtlesim::msg::Pose>(
            "/turtle1/pose", 10,
            std::bind(&TurtleController::on_pose_received, this, std::placeholders::_1)
        );

        // 创建巡逻服务
        patrol_server_ = this->create_service<Patrol>(
            "patrol",[&](const std::shared_ptr<Patrol::Request> request,
                      std::shared_ptr<Patrol::Response> response) -> void {
                      // 判断巡逻点是否在模拟器边界内 (turtlesim默认边界0-11.0)
                  if ((0 < request->target_x && request->target_x < 11.0f)
                  && (0 < request->target_y && request->target_y < 11.0f)) {
                      target_x_ = request->target_x;
                      target_y_ = request->target_y;
                      response->result = SUCCESS;
                      RCLCPP_INFO(this->get_logger(), "更新目标位置: (%.1f, %.1f)", 
                                 target_x_, target_y_);
                  } else {
                      response->result = FAIL;
                      RCLCPP_WARN(this->get_logger(), "目标位置超出边界: (%.1f, %.1f)",
                                 request->target_x, request->target_y);
                  }
              });
        
        RCLCPP_INFO(this->get_logger(), "PID控制器启动");
        RCLCPP_INFO(this->get_logger(), "初始目标位置: (%.1f, %.1f)", target_x_, target_y_);
    }

    void on_pose_received(const turtlesim::msg::Pose::SharedPtr pose)
    {   
        // 1. 打印位置变化
        if (last_x_ < 0) {  // 第一次接收
            last_x_ = pose->x;
            last_y_ = pose->y;
            RCLCPP_INFO(this->get_logger(), "初始位置: (%.2f, %.2f)", pose->x, pose->y);
        } else if (std::fabs(pose->x - last_x_) > 0.01 || std::fabs(pose->y - last_y_) > 0.01) {
            RCLCPP_INFO(this->get_logger(), "位置变化: (%.2f, %.2f) -> (%.2f, %.2f)", 
                       last_x_, last_y_, pose->x, pose->y);
            last_x_ = pose->x;
            last_y_ = pose->y;
        }
        
        // 2. PID控制逻辑
        auto current_x = pose->x;
        auto current_y = pose->y;
        auto current_theta = pose->theta;  // 当前朝向（弧度）
        
        // 计算到目标的距离
        auto distance = std::sqrt(
            (target_x_ - current_x) * (target_x_ - current_x) +
            (target_y_ - current_y) * (target_y_ - current_y)
        );
        
        // 计算目标方向的角度
        auto target_angle = std::atan2(target_y_ - current_y, target_x_ - current_x);
        
        // 角度归一化到[-π, π]
        auto angle_diff = target_angle - current_theta;
        while (angle_diff > M_PI) angle_diff -= 2.0 * M_PI;
        while (angle_diff < -M_PI) angle_diff += 2.0 * M_PI;
        
        RCLCPP_DEBUG(this->get_logger(), "距离: %.3f, 角度差: %.3f", distance, angle_diff);
        
        // 3. 控制策略
        auto cmd = geometry_msgs::msg::Twist();
        
        if (distance > 0.1) {  // 如果距离目标大于0.1
            if (std::fabs(angle_diff) > 0.2) {  // 如果角度差大于0.2弧度
                // 先转向：比例控制
                cmd.angular.z = 2.0 * angle_diff;
            } else {
                // 再前进：比例控制
                cmd.linear.x = k_ * distance;
            }
        } else {
            RCLCPP_INFO(this->get_logger(), "✅ 到达目标附近!");
        }
        
        // 4. 限制速度最大值
        if (cmd.linear.x > max_speed_) {
            cmd.linear.x = max_speed_;
        }
        if (std::fabs(cmd.angular.z) > max_angular_speed_) {
            cmd.angular.z = (cmd.angular.z > 0) ? max_angular_speed_ : -max_angular_speed_;
        }
        
        // 5. 发布控制命令
        publisher_->publish(cmd);
    }
};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TurtleController>();
    RCLCPP_INFO(node->get_logger(), "小海龟PID控制器启动，支持通过patrol服务修改目标位置");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}