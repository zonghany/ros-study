import rclpy
from rclpy.node import Node
from status_interfaces.msg import SystemStatus  # 导入消息接口
import psutil
import platform

class SysStatusPub(Node):
    def __init__(self, node_name):  # 修复：__init__ 不是 _init_
        super().__init__(node_name)  # 修复：__init__ 不是 _init_
        self.status_publisher_ = self.create_publisher(SystemStatus, 'sys_status', 10)  # 修复：SystemStatus 不是 Systemstatus
        self.timer = self.create_timer(1, self.timer_callback)
    
    def timer_callback(self):
        cpu_percent = psutil.cpu_percent()
        memory_info = psutil.virtual_memory()
        net_io_counters = psutil.net_io_counters()
        
        msg = SystemStatus()  # 修复：SystemStatus 不是 Systemstatus
        msg.stamp = self.get_clock().now().to_msg()
        msg.host_name = platform.node()
        msg.cpu_percent = cpu_percent
        msg.memory_percent = memory_info.percent
        msg.memory_total = memory_info.total / 1024 / 1024
        msg.memory_available = memory_info.available / 1024 / 1024
        msg.net_sent = net_io_counters.bytes_sent / 1024 / 1024
        msg.net_recv = net_io_counters.bytes_recv / 1024 / 1024
        
        self.get_logger().info(f'发布：{str(msg)}')  # 修复：括号不匹配
        self.status_publisher_.publish(msg)

def main():  # 修复：缩进错误，应该在类外面
    rclpy.init()
    node = SysStatusPub('sys_status_pub')  # 修复：SysStatusPub 不是 SysstatusPub
    rclpy.spin(node)
    rclpy.shutdown()

if __name__ == '__main__':  # 添加这行
    main()