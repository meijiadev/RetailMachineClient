package com.hardware.SerialPort;

import java.util.ArrayDeque;

public abstract class DataProcBase implements IDataProc {
    protected ArrayDeque<Byte> mDequeByte=new ArrayDeque<>(1024);

    public DataProcBase(){

    }

    public abstract boolean findCmdAndProc(ArrayDeque<Byte> queueByte);

    @Override
    public void onDataReceived(byte[] buffer) {
        for(int i=0; i<buffer.length; i++){
            mDequeByte.add(buffer[i]);
        }
        //
        boolean cmd=findCmdAndProc(mDequeByte);
        while(cmd){
            cmd=findCmdAndProc(mDequeByte);
        }
    }


    public void pollQueue(int element){
        for(int i=0;i<element;i++){
            mDequeByte.poll();
        }
    }

}
