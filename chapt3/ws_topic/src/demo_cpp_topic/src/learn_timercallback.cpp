#include "rclcpp/rclcpp.hpp"
#include "rclcpp/rclcpp.hpp"  //添加rclcpp库
#include "chrono"

using namespace std::chrono_literals;
//1.创建一个类公有继承rclcpp
//2.私有创建一个定时器的共享指针以及回调函数
//3.在构造函数中创建一个定时器，每隔一秒执行一次，给timer_赋值，create_wall_timer(时间，回调函数),然后再创建一个输出宏RCLCPP_INFO(this->get_logger(),"输出语句")
class SimpleTimerNode: public rclcpp::Node
{
    public:
        SimpleTimerNode(const std::string &node_name):rclcpp::Node(node_name)
        {
                timer_=this->create_wall_timer(1000ms,std::bind(&SimpleTimerNode::timer_callback,this));
                RCLCPP_INFO(this->get_logger(),"定时器已创建，每次一秒");
        };
    private:
        rclcpp::TimerBase::SharedPtr timer_;
        int cont = 0;
        void timer_callback()
        {   
            cont++;
            RCLCPP_INFO(this->get_logger(),"回调函数执行第%d次",cont);
        };
};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    
    // 创建节点
    auto node = std::make_shared<SimpleTimerNode>("timer_node");
    
    // 让节点运行（定时器会持续执行）
    RCLCPP_INFO(node->get_logger(), "节点开始运行");
    rclcpp::spin(node);
    
    RCLCPP_INFO(node->get_logger(), "节点结束");
    rclcpp::shutdown();
    return 0;
}