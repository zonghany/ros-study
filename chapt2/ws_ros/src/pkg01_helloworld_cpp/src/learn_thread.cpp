#include <iostream>
#include <thread>
#include <functional>
#include <chrono>
#include "cpp-httplib/httplib.h"

class Download
{
public:
    // 修正：回调函数参数改为2个
    void download(const std::string &host, const std::string &path,
                  const std::function<void(const std::string &, const std::string &)> &callback_word_count)
    {
        std::cout << "线程" << std::this_thread::get_id() << std::endl;
        httplib::Client client(host);
        auto response = client.Get(path.c_str());  // 注意：添加.c_str()
        
        if(response && response->status == 200){
            callback_word_count(path, response->body);  // 传递2个参数
        }
    }
    
    void start_download(const std::string &host, const std::string &path,
                       const std::function<void(const std::string &, const std::string &)> &callback_word_count)
    {
        // 使用lambda简化，避免bind的复杂问题
        std::thread thread([this, host, path, callback_word_count](){
            this->download(host, path, callback_word_count);
        });
        thread.detach();
    }
};

int main(){
    auto d = Download();
    auto word_count = [](const std::string &path, const std::string &result) -> void {
        std::cout << "下载完成" << path << ":" << result.length() 
                  << "->" << result.substr(0,5) << std::endl;
    };
    
    d.start_download("http://0.0.0.0:8000", "/novel1.txt", word_count);
    d.start_download("http://0.0.0.0:8000", "/novel2.txt", word_count);
    d.start_download("http://0.0.0.0:8000", "/novel3.txt", word_count);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(1000 * 10));
    return 0;
}