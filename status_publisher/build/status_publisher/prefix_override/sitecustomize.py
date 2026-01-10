import sys
if sys.prefix == '/usr':
    sys.real_prefix = sys.prefix
    sys.prefix = sys.exec_prefix = '/home/yzh/桌面/project-ros/ros-study/status_publisher/install/status_publisher'
