#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "tf2/LinearMath/Quaternion.h"
#include "tf2_ros/transform_broadcaster.h"
#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp"
using namespace std::chrono_literals; // 时间字面量（如10ms）

class DynamicTFBroadcaster : public rclcpp::Node {
public:
  DynamicTFBroadcaster() : Node("dynamic_tf_broadcaster_node") {
    // 1. 初始化动态TF广播器
    tf_broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>(this);
    // 2. 定时器：10ms发布1次（动态TF需持续更新）
    timer_ = this->create_wall_timer(10ms, std::bind(&DynamicTFBroadcaster::publish_tf, this));
  }

private:
  void publish_tf() {
  // 1. 发布map→base_link（原有逻辑）
  geometry_msgs::msg::TransformStamped tf_msg1;
  tf_msg1.header.stamp = this->get_clock()->now();
  tf_msg1.header.frame_id = "map";
  tf_msg1.child_frame_id = "base_link";
  tf_msg1.transform.translation.x = 2.0;
  tf_msg1.transform.translation.y = 3.0;
  tf_msg1.transform.translation.z = 0.0;
  tf2::Quaternion quat1;
  quat1.setRPY(0.0, 0.0, 3.1415/6);
  tf_msg1.transform.rotation = tf2::toMsg(quat1);
  tf_broadcaster_->sendTransform(tf_msg1);

  // 2. 新增：发布base_link→target_point
  geometry_msgs::msg::TransformStamped tf_msg2;
  tf_msg2.header.stamp = this->get_clock()->now();
  tf_msg2.header.frame_id = "base_link";  // 父坐标系
  tf_msg2.child_frame_id = "target_point";// 子坐标系（监听器要查的源坐标系）
  tf_msg2.transform.translation.x = 1.0;  // target_point在base_link的x=1m处
  tf_msg2.transform.translation.y = 0.0;
  tf_msg2.transform.translation.z = 0.5;  // z=0.5m处
  tf2::Quaternion quat2;
  quat2.setRPY(0.0, 0.0, 0.0);            // 无旋转
  tf_msg2.transform.rotation = tf2::toMsg(quat2);
  tf_broadcaster_->sendTransform(tf_msg2);
}

  // 动态TF广播器指针 + 定时器指针
  std::shared_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<DynamicTFBroadcaster>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}