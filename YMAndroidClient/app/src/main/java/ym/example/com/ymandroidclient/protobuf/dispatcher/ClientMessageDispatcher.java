package ym.example.com.ymandroidclient.protobuf.dispatcher;

import DDRCommProto.BaseCmd;
import ym.example.com.ymandroidclient.protobuf.processor.FileAddressProcessor;
import ym.example.com.ymandroidclient.protobuf.processor.NotifyBaseStatusExProcessor;
import ym.example.com.ymandroidclient.protobuf.processor.NotifyBaseStatusProcessor;
import ym.example.com.ymandroidclient.protobuf.processor.NotifyEnvInfoProcessor;
import ym.example.com.ymandroidclient.protobuf.processor.RspCmdMoveProcessor;
import ym.example.com.ymandroidclient.protobuf.processor.RspCmdSetWorkPathProcessor;
import ym.example.com.ymandroidclient.protobuf.processor.RspLoginProcessor;
import ym.example.com.ymandroidclient.protobuf.processor.ServerInformationProcessor;
import ym.example.com.ymandroidclient.utility.Logger;


public class ClientMessageDispatcher extends BaseMessageDispatcher {
    static ClientMessageDispatcher clientMessageDispatcher;

    public static ClientMessageDispatcher getInstance(){
        if (clientMessageDispatcher==null){
            clientMessageDispatcher=new ClientMessageDispatcher();
        }
        return clientMessageDispatcher;
    }

    public ClientMessageDispatcher(){
        BaseCmd.rspLogin rspLogin=BaseCmd.rspLogin.newBuilder().build();
        Logger.e("------"+rspLogin.getClass().toString());
        m_ProcessorMap.put(rspLogin.getClass().toString(),new RspLoginProcessor());

        BaseCmd.bcLSAddr bcLSAddr=BaseCmd.bcLSAddr.newBuilder().build();
        m_ProcessorMap.put(bcLSAddr.getClass().toString(),new ServerInformationProcessor());


        BaseCmd.rspFileAddress rspFileAddress=BaseCmd.rspFileAddress.newBuilder().build();
        m_ProcessorMap.put(rspFileAddress.getClass().toString(),new FileAddressProcessor());

        BaseCmd.notifyBaseStatus notifyBaseStatus=BaseCmd.notifyBaseStatus.newBuilder().build();
        m_ProcessorMap.put(notifyBaseStatus.getClass().toString(),new NotifyBaseStatusProcessor());

        BaseCmd.rspCmdMove rspCmdMove=BaseCmd.rspCmdMove.newBuilder().build();
        m_ProcessorMap.put(rspCmdMove.getClass().toString(),new RspCmdMoveProcessor());


        BaseCmd.rspCmdSetWorkPath rspCmdSetWorkPath=BaseCmd.rspCmdSetWorkPath.newBuilder().build();
        m_ProcessorMap.put(rspCmdSetWorkPath.getClass().toString(),new RspCmdSetWorkPathProcessor());

        BaseCmd.notifyBaseStatusEx notifyBaseStatusEx=BaseCmd.notifyBaseStatusEx.newBuilder().build();
        m_ProcessorMap.put(notifyBaseStatusEx.getClass().toString(),new NotifyBaseStatusExProcessor());

        BaseCmd.notifyEnvInfo notifyEnvInfo=BaseCmd.notifyEnvInfo.newBuilder().build();
        m_ProcessorMap.put(notifyEnvInfo.getClass().toString(),new NotifyEnvInfoProcessor());


    }
}
