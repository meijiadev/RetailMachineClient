package com.example.dadao.dadaorobot.entity;

public class NotifyEnvInfo {
    public static NotifyEnvInfo notifyEnvInfo;

    public static NotifyEnvInfo getInstance(){
        if (notifyEnvInfo==null){
            synchronized (NotifyEnvInfo.class){
                if (notifyEnvInfo==null){
                    notifyEnvInfo=new NotifyEnvInfo();
                }
            }
        }
        return notifyEnvInfo;
    }
    private float batt;
    private float temp;
    private float hum;
    private float pm25;
    private float co2;
    private float ch2o;

    public static NotifyEnvInfo getNotifyEnvInfo() {
        return notifyEnvInfo;
    }

    public static void setNotifyEnvInfo(NotifyEnvInfo notifyEnvInfo) {
        NotifyEnvInfo.notifyEnvInfo = notifyEnvInfo;
    }

    public float getBatt() {
        return batt;
    }

    public void setBatt(float batt) {
        this.batt = batt;
    }

    public float getTemp() {
        return temp;
    }

    public void setTemp(float temp) {
        this.temp = temp;
    }

    public float getHum() {
        return hum;
    }

    public void setHum(float hum) {
        this.hum = hum;
    }

    public float getPm25() {
        return pm25;
    }

    public void setPm25(float pm25) {
        this.pm25 = pm25;
    }

    public float getCo2() {
        return co2;
    }

    public void setCo2(float co2) {
        this.co2 = co2;
    }

    public float getCh2o() {
        return ch2o;
    }

    public void setCh2o(float ch2o) {
        this.ch2o = ch2o;
    }
}
