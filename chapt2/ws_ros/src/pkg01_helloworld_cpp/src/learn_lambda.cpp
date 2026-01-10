#include <iostream>
#include <algorithm>

int main(int argc,char **argv){
    auto a=[](int x,int y)->int{
        return x+y;
    };
    auto b=[a](){
        std::cout<<"x+y = "<<a<<std::endl;
    };
    b();
    return 0;
}

