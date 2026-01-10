#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "turtlesim/msg/pose.hpp"
#include <cmath>

class TurtlePIDController : public rclcpp::Node
{
private:
    double last_x_, last_y_;  // 上一次位置
    double target_x_, target_y_;  // 目标位置
    double k_;  // 比例系数
    double max_speed_;  // 最大线速度
    double max_angular_speed_;  // 最大角速度
    
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr subscription_;
    
public:
    explicit TurtlePIDController(const std::string& node_name): Node(node_name)
    { 
        // 初始化控制参数
        last_x_ = -1.0;  // 初始标记
        last_y_ = -1.0;
        target_x_ = 8.0;  // 目标位置 (8.0, 8.0)
        target_y_ = 8.0;
        k_ = 1.0;  // 比例系数
        max_speed_ = 2.0;  // 最大线速度
        max_angular_speed_ = 2.0;  // 最大角速度
        
        // 创建发布者和订阅者
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>(
            "/turtle1/cmd_vel", 10);
        subscription_ = this->create_subscription<turtlesim::msg::Pose>(
            "/turtle1/pose", 10,
            std::bind(&TurtlePIDController::on_pose_received, this, std::placeholders::_1)
        );
        
        RCLCPP_INFO(this->get_logger(), "PID控制器启动");
        RCLCPP_INFO(this->get_logger(), "目标位置: (%.1f, %.1f)", target_x_, target_y_);
    }

    void on_pose_received(const turtlesim::msg::Pose::SharedPtr pose)
    {   
        // 1. 打印位置变化
        if (last_x_ < 0) {  // 第一次接收
            last_x_ = pose->x;
            last_y_ = pose->y;
            RCLCPP_INFO(this->get_logger(), "初始位置: (%.2f, %.2f)", pose->x, pose->y);
        } else if (pose->x != last_x_ || pose->y != last_y_) {
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
    auto node = std::make_shared<TurtlePIDController>("turtle_pid_controller");
    RCLCPP_INFO(node->get_logger(), "小海龟PID控制器启动");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}