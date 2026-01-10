#include "rclcpp/rclcpp.hpp"

class PersonNode : public rclcpp::Node
{
private:
    std::string name_;
    int age_;

public:
    PersonNode(const std::string &node_name,const std::string &_name,const int _age):Node(node_name)
    {//使用引用，避免不必要的数据拷贝
        this->name_=_name;
        this->age_=_age;
    }

    void eat(const std::string &eat_name){
        RCLCPP_INFO(this->get_logger(),"_name : %s,age_ : %d, eat:%s",
        this->name_.c_str(),this->age_,eat_name.c_str());
    }
};

int main(int argc,char **argv){
    rclcpp::init(argc,argv);
    auto person_node = std::make_shared<PersonNode>("node","zs",18);
    RCLCPP_INFO(person_node->get_logger(),"c++");
    person_node->eat("fish");
    rclcpp::spin(person_node);
    rclcpp::shutdown();
    return 0;
}