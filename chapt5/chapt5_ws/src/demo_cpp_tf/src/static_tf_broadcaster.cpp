#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "tf2/LinearMath/Quaternion.h"
#include "tf2_ros/static_transform_broadcaster.h"
#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp"

class StaticTFBroadcaster : public rclcpp::Node {
public:
  // 构造函数：初始化节点+静态TF广播器
  StaticTFBroadcaster() : Node("static_tf_broadcaster_node") {
    //静态广播发布器，this指的是当前节点的指针
    //StaticTransformBroadcaster：创建发布者
    broadcaster_ = std::make_shared<tf2_ros::StaticTransformBroadcaster>(this);
    publish_static_tf(); // 发布静态TF（仅需发布1次）
  }

private:
  void publish_static_tf() {
    geometry_msgs::msg::TransformStamped tf_msg;
    // 1. 设置时间戳（静态TF用当前时间）
    tf_msg.header.stamp = this->get_clock()->now();
    // 2. 父坐标系（地图）+ 子坐标系（目标点）
    tf_msg.header.frame_id = "map";
    tf_msg.child_frame_id = "target_point";
    // 3. 平移：target_point在map的x=5.0m、y=3.0m、z=0.0m处
    tf_msg.transform.translation.x = 5.0;
    tf_msg.transform.translation.y = 3.0;
    tf_msg.transform.translation.z = 0.0;
    // 4. 旋转：绕z轴转60°（欧拉角→四元数，60°=π/3 rad）
    tf2::Quaternion quat;
    //转成弧度制，再转成四元数
    quat.setRPY(0.0, 0.0, 3.1415/3); // roll=0, pitch=0, yaw=π/3
    tf_msg.transform.rotation = tf2::toMsg(quat);
    // 5. 发布静态TF（ROS2会自动缓存，无需循环）
    broadcaster_->sendTransform(tf_msg);//调用发布者
    RCLCPP_INFO(this->get_logger(), "静态TF发布：map→target_point");
  }

  // 静态TF广播器指针
  std::shared_ptr<tf2_ros::StaticTransformBroadcaster> broadcaster_;
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<StaticTFBroadcaster>();
  rclcpp::spin(node); // 保持节点运行
  rclcpp::shutdown();
  return 0;
}