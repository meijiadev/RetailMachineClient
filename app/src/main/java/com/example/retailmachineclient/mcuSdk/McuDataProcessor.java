package com.example.retailmachineclient.mcuSdk;
import android.content.Context;

import com.example.retailmachineclient.util.Logger;



import java.util.ArrayDeque;

/**
 * desc:处理从MCU接收过来的数据
 * time:2021/1/29
 */
public class McuDataProcessor implements IDataProcessor  {

	public McuDataProcessor( ) {

	}


	@Override
	public void onDataReceive(byte[] buf) {
		if (buf==null){
			return ;
		}
		int bufLen=buf.length;
		if (bufLen<DataProtocol.MIN_CMD_LEN){
			return ;
		}
		//检查校验码是否正确
		if (DataProtocol.isCheckRight(buf)){
			DataProtocol.RecDataCls recData=DataProtocol.createRecDataCls(buf);
			Logger.e("-------ReceiveData:"+recData.toString());
			doCmdResult(recData);
		}

	}

	/**
	 * 处理分发命令
	 * @param recDataCls
	 */
	private void doCmdResult(DataProtocol.RecDataCls recDataCls){
		switch(recDataCls.directiveId){
			case 0x12:   //升降机回复指令

				break;
			case 0x13:   //查询升降机执行状态

				break;
			case 0x14:   //控制舱门执行结果

				break;
			case 0x15:    //查询推杆指令执行状态

				break;


		}
	}
}
