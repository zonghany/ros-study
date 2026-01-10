#include <iostream>
#include <thread>

// 线程要执行的函数
void print_numbers(int start, int end) {
    for (int i = start; i <= end; i++) {
        std::cout << "线程: " << i << std::endl;
    }
}

int main() {
    std::cout << "主线程开始" << std::endl;
    
    // 1. 创建第一个线程
    std::thread t1(print_numbers, 1, 5);
    
    // 2. 创建第二个线程
    std::thread t2(print_numbers, 6, 10);
    
    // 3. 等待两个线程都完成
    t1.join();
    t2.join();
    
    std::cout << "主线程结束" << std::endl;
    return 0;
}