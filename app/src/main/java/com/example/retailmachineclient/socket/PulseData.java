package com.example.retailmachineclient.socket;

import com.xuhao.didi.core.iocore.interfaces.IPulseSendable;

/**
 * 心跳包发送
 */
public class PulseData implements IPulseSendable {
    private byte pulse[];

    public PulseData(byte[] pulse) {
        this.pulse=pulse;
    }

    @Override
    public byte[] parse() {
        return pulse;
    }

}
