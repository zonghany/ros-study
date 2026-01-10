#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "tf2/LinearMath/Quaternion.h"
#include "tf2_ros/buffer.h"
#include "tf2_ros/transform_listener.h"
// 新增：引入tf2_geometry_msgs的转换头文件
#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp"
using namespace std::chrono_literals;

class TFListener : public rclcpp::Node {
public:
  TFListener() : Node("tf_listener_node") {
    buffer_ = std::make_shared<tf2_ros::Buffer>(this->get_clock());
    listener_ = std::make_shared<tf2_ros::TransformListener>(*buffer_, this);
    timer_ = this->create_wall_timer(5s, std::bind(&TFListener::get_transform, this));
  }

private:
  void get_transform() {
    try {
      auto transform = buffer_->lookupTransform(
        "base_link",    
        "target_point", 
        this->get_clock()->now(),
        rclcpp::Duration::from_seconds(1.0)
      );

      // ========== 修正：四元数转换逻辑 ==========
      auto &trans = transform.transform.translation;
      // 1. 先把geometry_msgs的Quaternion转成tf2::Quaternion
      tf2::Quaternion tf_quat;
      tf2::fromMsg(transform.transform.rotation, tf_quat);
      // 2. 再从tf2::Quaternion计算欧拉角（roll/pitch/yaw，弧度制）
      double roll, pitch, yaw;
      tf2::Matrix3x3(tf_quat).getRPY(roll, pitch, yaw);

      // 打印结果
      RCLCPP_INFO(this->get_logger(), "base_link→target_point 变换：");
      RCLCPP_INFO(this->get_logger(), "平移：x=%.3f, y=%.3f, z=%.3f", trans.x, trans.y, trans.z);
      RCLCPP_INFO(this->get_logger(), "旋转（欧拉角）：roll=%.3f, pitch=%.3f, yaw=%.3f", roll, pitch, yaw);
    } catch (tf2::TransformException &e) {
      RCLCPP_WARN(this->get_logger(), "查询失败：%s", e.what());
    }
  }

  std::shared_ptr<tf2_ros::Buffer> buffer_;
  std::shared_ptr<tf2_ros::TransformListener> listener_;
  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<TFListener>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}