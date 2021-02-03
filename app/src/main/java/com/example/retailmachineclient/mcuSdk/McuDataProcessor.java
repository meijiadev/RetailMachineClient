package com.example.retailmachineclient.mcuSdk;
import android.content.Context;

import com.example.retailmachineclient.util.Logger;



import java.util.ArrayDeque;

/**
 * desc:处理从MCU接收过来的数据
 * time:2021/1/29
 */
public class McuDataProcessor  {
	private Context mContext;

	private Object mLock = new Object();
	private boolean mCmdAckRst = false;
	public McuDataProcessor(Context context) {
		mContext = context;
	}
	private DataProtocol.RecDataCls recData;
	/**
	 * 锁定当前线种,等待结果
	 */
	public void waitResult() {
		synchronized (mLock) {
			try {
				mLock.wait(800);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
	}


	/**
	 * 取消锁定,通知被锁的线程
	 */
	public void notifyResult() {
		synchronized (mLock) {
			mLock.notify();
		}
	}

	public DataProtocol.RecDataCls getResult(){
		Logger.e("---------:"+recData.toString());
		return recData;
	}



	public boolean getCmdResult() {
		return mCmdAckRst;
	}

	public boolean findCmdAndProc(byte[] buf) {
		if (buf==null){
			return false;
		}
		int bufLen=buf.length;
		if (bufLen<DataProtocol.MIN_CMD_LEN){
			return false;
		}
		//检查校验码是否正确
		if (DataProtocol.isCheckRight(buf)){
			recData=DataProtocol.createRecDataCls(buf);
			Logger.e("-------ReceiveData:"+recData.toString());
			notifyResult();
		}
		return true;
	}
}
