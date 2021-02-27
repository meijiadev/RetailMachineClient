package com.example.retailmachineclient.mcuSdk;


/**
 * 串口通信总控
 */
public class MCU {
    public static final int PORT_RATE=9600;             //串口波特率
    public static final byte[] PUSH_ROD_BACK=new byte[]{0x11,0x14,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //收回推杆
    public static final byte[] PUSH_ROD=new byte[]{0x11,0x14,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};           //推出推杆
    public static final byte[] QUERY_ROD=new byte[]{0x11,0x15,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};   //查询推杆状态
    public static final byte[] QUERY_LIFTER=new byte[]{0x11,0x13,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};//查询升降机状态

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
