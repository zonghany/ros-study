#include <iostream>
#include <memory>
int main(int argc, char **argv)
{
    auto p1 = std::make_shared<std::string>("this is a ptr");
    std::cout<<"p1的个数"<<p1.use_count()<<"p1指向的地址"<<p1.get()<<std::endl;
    auto p2=p1;
    std::cout<<"p1的个数"<<p1.use_count()<<"p1指向的地址"<<p1.get()<<std::endl;
    std::cout<<"p2的个数"<<p2.use_count()<<"p2指向的地址"<<p2.get()<<std::endl;

    p1.reset();
    std::cout<<"p1的个数"<<p1.use_count()<<"p1指向的地址"<<p1.get()<<std::endl;
    
    std::cout<<"p2的内容:"<<p2->c_str()<<std::endl;
    return 0;
}