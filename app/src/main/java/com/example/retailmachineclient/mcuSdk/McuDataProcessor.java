package com.example.retailmachineclient.mcuSdk;
import android.content.Context;
import com.hardware.SerialPort.DataProcBase;


import java.util.ArrayDeque;

/**
 * desc:处理从MCU接收过来的数据
 * time:2021/1/29
 */
public class McuDataProcessor extends DataProcBase {
	private Context mContext;

	private Object mLock = new Object();
	private boolean mCmdAckRst = false;
	public McuDataProcessor(Context context) {
		mContext = context;
	}

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

	public boolean getCmdResult() {
		return mCmdAckRst;
	}

	@Override
	public boolean findCmdAndProc(ArrayDeque<Byte> queueByte) {


		return false;
	}
}
