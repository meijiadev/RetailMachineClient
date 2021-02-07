package com.example.retailmachineclient.mcuSdk;

public interface IDataProcessor {
    void onDataReceive(byte[] buf);
}
