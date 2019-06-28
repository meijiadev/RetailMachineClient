package ym.example.com.ymandroidclient.entity;

/**
 *BaseCmd.notifyBaseStatus
 *  机器人状态信息
 */
public class RobotStatusEntity {
     static public RobotStatusEntity robotStatusEntity;

    /**
     * 单例模式 只提供一个对象给外部使用保证set和get到的数据一致
     * @return
     */
     public static RobotStatusEntity getInstance(){
         if (robotStatusEntity==null){
             synchronized (RobotStatusEntity.class){
                 if (robotStatusEntity==null){
                     robotStatusEntity=new RobotStatusEntity();
                 }
             }
         }
         return robotStatusEntity;
     }

     private int model;    // 机器人模式：0-待命；1-采集；2-自动动态；3-自动静态；4-充电；5-盲走动态；6-盲走静态
     private int sonmodel; // 0-初始化；1-暂停/返回充电桩；2-/行走/充电中；3-返回原始地址
     private float batt=0;   //电量 0~100
     private float temp=0;   //温度 0~100
     private float hum=0;    //湿度 0~100
     private float pm25=0;   //PM2.5浓度 0~500
     private int stopstat; //0x01-超声；0x02-红外；0x04-急停；0x08-手柄
     private long funcstat; //各模块的工作状态
     private int voiceloop; //当前是否有循环音频 0-否
     private String curroute; //当前行走的路径名
     private String currpath; //当前使用的路线
     private float currspeed; //当前默认的速度
     private String currwalkstyle; //当前的循环模式
     private float posx;           //当前坐标x
     private float posy;           //当前坐标y
     private float poslinespeed=0;   //当前线速度
     private float posangulauspeed=0; //当前角速度
     private double longitude;      //GPS经线
     private double latitude;       //Gps纬度
     private long  timestamp;        //时间戳
     private int thermalmaxX;      //当前最高温度坐标X
     private int thermalmaxY;      //当前最高温度坐标Y
     private float thermalmaxvalue;  //当前最高温度数值

    public int getModel() {
        return model;
    }

    public void setModel(int model) {
        this.model = model;
    }

    public int getSonmodel() {
        return sonmodel;
    }

    public void setSonmodel(int sonmodel) {
        this.sonmodel = sonmodel;
    }

    public float getBatt() {
        return batt;
    }

    public void setBatt(float batt) {
        if (batt!=0xffff&&batt<=100&&batt>=0){
            this.batt = batt;
        }else {
            this.batt=0;
        }

    }

    public float getTemp() {
        return temp;
    }

    public void setTemp(float temp) {
        if (temp!=0xffff&&temp<=100&&temp>=0) {
            this.temp = temp;
        }else {
            this.temp=0;
        }
    }

    public float getHum() {
        return hum;
    }

    public void setHum(float hum) {
        if (hum!=0xffff&&hum<=100&&hum>=0) {
            this.hum = hum;
        }else {
            this.hum=0;
        }
    }

    public float getPm25() {
        return pm25;
    }

    public void setPm25(float pm25) {
        if (pm25!=0xffff&&pm25<=500&&batt>=0) {
            this.pm25 = pm25;
        }else {
            this.pm25=0;
        }
    }

    public int getStopstat() {
        return stopstat;
    }

    public void setStopstat(int stopstat) {
        this.stopstat = stopstat;
    }

    public long getFuncstat() {
        return funcstat;
    }

    public void setFuncstat(long funcstat) {
        this.funcstat = funcstat;
    }

    public int getVoiceloop() {
        return voiceloop;
    }

    public void setVoiceloop(int voiceloop) {
        this.voiceloop = voiceloop;
    }

    public String getCurroute() {
        return curroute;
    }

    public void setCurroute(String curroute) {
        this.curroute = curroute;
    }

    public String getCurrpath() {
        return currpath;
    }

    public void setCurrpath(String currpath) {
        this.currpath = currpath;
    }

    public float getCurrspeed() {
        return currspeed;
    }

    public void setCurrspeed(float currspeed) {
        this.currspeed = currspeed;
    }

    public String getCurrwalkstyle() {
        return currwalkstyle;
    }

    public void setCurrwalkstyle(String currwalkstyle) {
        this.currwalkstyle = currwalkstyle;
    }

    public float getPosx() {
        return posx;
    }

    public void setPosx(float posx) {
        this.posx = posx;
    }

    public float getPosy() {
        return posy;
    }

    public void setPosy(float posy) {
        this.posy = posy;
    }

    public float getPoslinespeed() {
        return poslinespeed;
    }

    public void setPoslinespeed(float poslinespeed) {
        this.poslinespeed = poslinespeed;
    }

    public float getPosangulauspeed() {
        return posangulauspeed;
    }

    public void setPosangulauspeed(float posangulauspeed) {
        if (posangulauspeed!=0xffff&&posangulauspeed>=0){
            this.posangulauspeed = posangulauspeed;
        }else {
            this.posangulauspeed =0;
        }

    }

    public double getLongitude() {
        return longitude;
    }

    public void setLongitude(double longitude) {
        this.longitude = longitude;
    }

    public double getLatitude() {
        return latitude;
    }

    public void setLatitude(double latitude) {
        this.latitude = latitude;
    }

    public long getTimestamp() {
        return timestamp;
    }

    public void setTimestamp(long timestamp) {
        this.timestamp = timestamp;
    }

    public int getThermalmaxX() {
        return thermalmaxX;
    }

    public void setThermalmaxX(int thermalmaxX) {
        this.thermalmaxX = thermalmaxX;
    }

    public int getThermalmaxY() {
        return thermalmaxY;
    }

    public void setThermalmaxY(int thermalmaxY) {
        this.thermalmaxY = thermalmaxY;
    }

    public float getThermalmaxvalue() {
        return thermalmaxvalue;
    }

    public void setThermalmaxvalue(float thermalmaxvalue) {
        this.thermalmaxvalue = thermalmaxvalue;
    }

    public void setNull(){
        robotStatusEntity=null;
    }

}
