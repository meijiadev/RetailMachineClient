package com.example.dadao.dadaorobot.entity;

public class NotifyBaseStatusEx {
    public static NotifyBaseStatusEx notifyBaseStatusEx;


    public static NotifyBaseStatusEx getInstance(){
        if (notifyBaseStatusEx==null){
            synchronized (NotifyBaseStatusEx.class){
                if (notifyBaseStatusEx==null){
                    notifyBaseStatusEx=new NotifyBaseStatusEx();
                }
            }
        }
        return notifyBaseStatusEx;
    }

    private int mode;
    private int sonMode;
    private int stopStat;
    private String curroute;
    private String currpath;
    private float posX;
    private float posY;
    private float posDirection;
    private float posLinespeed;
    private float posAngulauspeed;


    public static NotifyBaseStatusEx getNotifyBaseStatusEx() {
        return notifyBaseStatusEx;
    }

    public static void setNotifyBaseStatusEx(NotifyBaseStatusEx notifyBaseStatusEx) {
        NotifyBaseStatusEx.notifyBaseStatusEx = notifyBaseStatusEx;
    }

    public int getMode() {
        return mode;
    }

    public void setMode(int mode) {
        this.mode = mode;
    }

    public int getSonMode() {
        return sonMode;
    }

    public void setSonMode(int sonMode) {
        this.sonMode = sonMode;
    }

    public int getStopStat() {
        return stopStat;
    }

    public void setStopStat(int stopStat) {
        this.stopStat = stopStat;
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

    public float getPosX() {
        return posX;
    }

    public void setPosX(float posX) {
        this.posX = posX;
    }

    public float getPosY() {
        return posY;
    }

    public void setPosY(float posY) {
        this.posY = posY;
    }

    public float getPosDirection() {
        return posDirection;
    }

    public void setPosDirection(float posDirection) {
        this.posDirection = posDirection;
    }

    public float getPosLinespeed() {
        return posLinespeed;
    }

    public void setPosLinespeed(float posLinespeed) {
        this.posLinespeed = posLinespeed;
    }

    public float getPosAngulauspeed() {
        return posAngulauspeed;
    }

    public void setPosAngulauspeed(float posAngulauspeed) {
        this.posAngulauspeed = posAngulauspeed;
    }
}
