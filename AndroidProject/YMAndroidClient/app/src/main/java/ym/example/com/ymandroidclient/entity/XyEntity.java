package ym.example.com.ymandroidclient.entity;

/**
 * Path文件中的坐标和速度
 * 发送给服务端的点信息
 */
public class XyEntity {
    private int index;
    private float x;
    private float y;
    private float speed;
    private int ePathPointType=0;   //0 1 2 3
    private String pointInfo="无";


    public XyEntity(float x,float y,float speed) {
        this.x=x;
        this.y=y;
        this.speed=speed;
    }

    public XyEntity(int index,float x,float y){
        this.index=index;
        this.x=x;
        this.y=y;
    }

    public XyEntity(float x,float y){
        this.x=x;
        this.y=y;
    }

    public float getX() {
        return x;
    }

    public void setX(float x) {
        this.x = x;
    }

    public float getY() {
        return y;
    }

    public void setY(float y) {
        this.y = y;
    }

    public float getSpeed() {
        return speed;
    }

    public void setSpeed(float speed) {
        this.speed = speed;
    }

    public int getePathPointType() {
        return ePathPointType;
    }

    public void setePathPointType(int ePathPointType) {
        this.ePathPointType = ePathPointType;
    }

    public int getIndex() {
        return index;
    }

    public void setIndex(int index) {
        this.index = index;
    }

    public String getPointInfo() {
        return pointInfo;
    }

    public void setPointInfo(String pointInfo) {
        this.pointInfo = pointInfo;
    }
}

