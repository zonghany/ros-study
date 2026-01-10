#include <iostream>
#include <functional>

void save_with_free_fun(const std::string &filename){
    std::cout<<"自由函数："<<filename<<std::endl;
}

class FileSave
{
private:
    /* data */
public:
    FileSave()=default;

    ~FileSave()=default;

    void save_with_member_fun(const std::string &filename){
        std::cout<<"成员方法"<<filename<<std::endl;
    }
};
int main(int argc,char **argv){
    FileSave s;
    auto p=[](std::string &filename){
        std::cout<<"lambda方法"<<filename<<std::endl;
    };
    std::string str="aaaaaaafile";
    
    // p(str);
    // s.save_with_member_fun(str);
    // save_with_free_fun(str);
    std::function<void(std::string &)> save1=save_with_free_fun;
    std::function<void(std::string &)> save2=p;
    std::function<void(std::string &)> save3=std::bind(&FileSave::save_with_member_fun,&s,std::placeholders::_1);
    save1(str);
    save2(str);
    save3(str);
    return 0;
}