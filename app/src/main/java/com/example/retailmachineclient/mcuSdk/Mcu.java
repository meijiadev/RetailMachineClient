package com.example.retailmachineclient.mcuSdk;


/**
 * 串口通信总控
 */
public class Mcu {
    public static final int PORT_RATE=9600;             //串口波特率

    /**
     * 串口枚举，
     */
    public enum PortMCU{
        //出货串口
        MCU1("/dev/ttyS1",0x11, "MCU1"),
        //按键串口
        MCU2("/dev/ttyS0",0x11, "MCU2"),
        //会员卡串口
        MCU3("/dev/ttyS2", 0x11,"MCU3"),
        //pos机串口
        MCU4("/dev/ttyS3",0x11, "MCU4");
        private String path;
        private String name;
        private int flag;

        @Override
        public String toString() {
            return name+"["+path+"]";
        }
        PortMCU(String path,int flag,String  name){
            this.path = path;
            this.name=name;
            this.flag=flag;
        }

        public String getName() {
            return name;
        }

        public String getPath() {
            return path;
        }

        public int getFlag() {
            return flag;
        }
    }

}
