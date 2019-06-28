package ym.example.com.ymandroidclient.socket;

/**
 * tcp是否连接，以及断线重连的接口
 */
public interface ITcpClient {
    void tcpConnected();   // 连接成功
    void tcpDisConnected();  //连接非主动断开
    void tcpServerFinish();  //服务器关闭，有可能是服务器挂掉或者网络连接不对
    void connectFailure();   //连接失败
}
