#include "rclcpp/rclcpp.hpp"
#include "rcl_interfaces/msg/set_parameters_result.hpp"

class DynamicParamNode : public rclcpp::Node
{
public:
    DynamicParamNode() : Node("turtlectrl")
    {
        // 声明参数 + 默认值
        this->declare_parameter("k_", 1.0);
        this->declare_parameter("max_speed_", 1.0);

        // 初始化成员变量
        this->get_parameter("k_", k_);
        this->get_parameter("max_speed_", max_speed_);

        // 注册参数回调【核心：实现rqt动态修改】
        param_cb_handle = this->add_on_set_parameters_callback(
            std::bind(&DynamicParamNode::param_change_cb, this, std::placeholders::_1)
        );

        RCLCPP_INFO(this->get_logger(), "✅ 节点启动成功！初始参数：");
        RCLCPP_INFO(this->get_logger(), "k_ = %.2f, max_speed_ = %.2f", k_, max_speed_);
    }

private:
    // 要动态修改的C++成员变量
    double k_;
    double max_speed_;
    rclcpp::node_interfaces::OnSetParametersCallbackHandle::SharedPtr param_cb_handle;

    // 参数变更回调函数：rqt修改参数时自动执行，同步更新成员变量
    rcl_interfaces::msg::SetParametersResult param_change_cb(const std::vector<rclcpp::Parameter> & params)
    {
        rcl_interfaces::msg::SetParametersResult res;
        res.successful = true;
        for (const auto & p : params)
        {
            if (p.get_name() == "k_")
            {
                k_ = p.as_double();
                RCLCPP_INFO(this->get_logger(), "🔄 成员变量k_已更新 → %.2f", k_);
            }
            if (p.get_name() == "max_speed_")
            {
                if(p.as_double() < 0) { res.successful=false; res.reason="速度不能为负！"; return res; }
                max_speed_ = p.as_double();
                RCLCPP_INFO(this->get_logger(), "🔄 成员变量max_speed_已更新 → %.2f", max_speed_);
            }
        }
        return res;
    }
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<DynamicParamNode>());
    rclcpp::shutdown();
    return 0;
}