package ym.example.com.ymandroidclient.entity;

/**
 * 机器人的Id ip port
 */
public class RobotIDEntity {
    private  String ID;
    private  String Ip;
    private  int port;
    public static RobotIDEntity robotIDEntity;
    public static RobotIDEntity getInstance(){
        if (robotIDEntity==null){
            robotIDEntity=new RobotIDEntity();
        }
        return robotIDEntity;
    }
    public String getID() {
        return ID;
    }

    public void setID(String ID) {
        this.ID = ID;
    }

    public String getIp() {
        return Ip;
    }

    public void setIp(String ip) {
        Ip = ip;
    }

    public int getPort() {
        return port;
    }

    public void setPort(int port) {
        this.port = port;
    }

}
