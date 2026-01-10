
# 响应（Response）：返回检测结果
int16 number             # 修正：确保是int16（16位整数，若需32位可改为int32）
float32 use_time         # 检测耗时
int32[] top              # 修正：数组格式（int32[] 无空格）- 人脸顶部坐标数组
int32[] right            # 人脸右侧坐标数组
int32[] bottom           # 人脸底部坐标数组
int32[] left             # 人脸左侧坐标数组