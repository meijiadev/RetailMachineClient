20190420 Young
这里是导航模组中对硬件的封装。主要有：
1）嵌入式：
2）激光雷达：

以下是对目录的说明：
1. CommonDef：各种类型定义、平台、系统相关配置、系统相关函数
2. CommLib：Socket通信库，包括结构化数据格式及协议(SDProto)、UDP短帧拆分和组合、常用SOCKET相关函数
3. LocalNetworkNode 局域网节点通信库，用于使用名称来进行订阅和发布的去中心化局域网通信
4. Embedded：嵌入式访问相关代码
5. LidarSDK：雷达操作相关代码。
6. Geometry：几何位姿类辅助功能源代码

