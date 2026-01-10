#include "rclcpp/rclcpp.hpp"
#include "chapt4_interfaces/srv/patrol.hpp"
#include <ctime>
#include <chrono>
using Patrol = chapt4_interfaces::srv::Patrol;
using namespace std::chrono_literals;

class PatrolClient : public rclcpp::Node
{
private:
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Client<Patrol>::SharedPtr patrol_client_;
public:
    PatrolClient(/* args */) : Node("turtle_controller")
    {
        //1.在patrolClinet的构造函数中创建客户端节点
        patrol_client_ = this->create_client<Patrol>("patrol");
        //2.创建定时器
        timer_ = this->create_wall_timer(10s,std::bind(&PatrolClient::timer_callback,this));
        //3.初始化随机种子
        srand(time(NULL));
    }
    void timer_callback(){
        //`随机生成目标点，请求至服务端
        //1.每秒等待服务，如果!rclcpp::ok,那么就是rclcpp挂了
        while(!this->patrol_client_->wait_for_service(1s))
        {  
            if(!rclcpp::ok()){
                RCLCPP_ERROR(this->get_logger(),"rclcpp在等待服务过程中挂了");
                return;
            }
            RCLCPP_INFO(this->get_logger(),"等待服务上线中...");
        }
        //2.构建请求对象，使用ctime产生随机数
        auto request = std::make_shared<Patrol::Request>();
        request->target_x = rand()%12;
        request->target_y = rand()%12;
        RCLCPP_INFO(this->get_logger(),"随机的目标点target_x = %lf,target_y = %lf",request->target_x,request->target_y);
        this->patrol_client_->async_send_request(request,[&]
            (rclcpp::Client<Patrol>::SharedFuture result_future) -> void {
                auto response =  result_future.get();
                if(response->result == Patrol::Response::SUCCESS){
                    RCLCPP_INFO(this->get_logger(),"请求巡逻目标点成功");
                }else if(response->result == Patrol::Response::FAIL){
                    RCLCPP_INFO(this->get_logger(),"请求巡逻目标点失败");
                } 
            });
    }
};


int main(int argc,char **argv)
{
    rclcpp::init(argc,argv);
    auto node = std::make_shared<PatrolClient>();        //节点的创建
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}