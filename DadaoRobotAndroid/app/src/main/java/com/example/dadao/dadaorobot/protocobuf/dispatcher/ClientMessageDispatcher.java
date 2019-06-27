package com.example.dadao.dadaorobot.protocobuf.dispatcher;


import com.example.dadao.dadaorobot.protocobuf.processor.CmdStartActionModel;
import com.example.dadao.dadaorobot.protocobuf.processor.FileAddressProcessor;
import com.example.dadao.dadaorobot.protocobuf.processor.NotifyBaseStatusProcessor;
import com.example.dadao.dadaorobot.protocobuf.processor.RspAudioTalkProcess;
import com.example.dadao.dadaorobot.protocobuf.processor.RspCmdMoveProcessor;
import com.example.dadao.dadaorobot.protocobuf.processor.RspCmdSetWorkPathProcessor;
import com.example.dadao.dadaorobot.protocobuf.processor.RspRemoteLoginProcessor;
import com.example.dadao.dadaorobot.protocobuf.processor.RspRemoteServerListProcessor;
import com.example.dadao.dadaorobot.protocobuf.processor.RspSelectLSProcessor;
import com.example.dadao.dadaorobot.protocobuf.processor.ServerInformationProcessor;
import com.example.dadao.dadaorobot.protocobuf.processor.SigninProcessor;
import com.example.dadao.dadaorobot.protocobuf.processor.StreamAddrProcessor;
import com.example.dadao.dadaorobot.util.Logger;

import DDRCommProto.BaseCmd;
import DDRCommProto.RemoteCmd;

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
        m_ProcessorMap.put(rspLogin.getClass().toString(),new SigninProcessor());

        BaseCmd.bcLSAddr bcLSAddr=BaseCmd.bcLSAddr.newBuilder().build();
        m_ProcessorMap.put(bcLSAddr.getClass().toString(),new ServerInformationProcessor());

        BaseCmd.rspStreamAddr rspStreamAddr=BaseCmd.rspStreamAddr.newBuilder().build();
        m_ProcessorMap.put(rspStreamAddr.getClass().toString(),new StreamAddrProcessor());

        BaseCmd.rspFileAddress rspFileAddress=BaseCmd.rspFileAddress.newBuilder().build();
        m_ProcessorMap.put(rspFileAddress.getClass().toString(),new FileAddressProcessor());

        BaseCmd.notifyBaseStatus notifyBaseStatus=BaseCmd.notifyBaseStatus.newBuilder().build();
        m_ProcessorMap.put(notifyBaseStatus.getClass().toString(),new NotifyBaseStatusProcessor());

        BaseCmd.rspCmdStartActionMode rspCmdStartActionMode=BaseCmd.rspCmdStartActionMode.newBuilder().build();
        m_ProcessorMap.put(rspCmdStartActionMode.getClass().toString(),new CmdStartActionModel());

        BaseCmd.rspCmdMove rspCmdMove=BaseCmd.rspCmdMove.newBuilder().build();
        m_ProcessorMap.put(rspCmdMove.getClass().toString(),new RspCmdMoveProcessor());

        BaseCmd.rspAudioTalk rspAudioTalk=BaseCmd.rspAudioTalk.newBuilder().build();
        m_ProcessorMap.put(rspAudioTalk.getClass().toString(),new RspAudioTalkProcess());

        BaseCmd.rspCmdSetWorkPath rspCmdSetWorkPath=BaseCmd.rspCmdSetWorkPath.newBuilder().build();
        m_ProcessorMap.put(rspCmdSetWorkPath.getClass().toString(),new RspCmdSetWorkPathProcessor());

        RemoteCmd.rspRemoteLogin rspRemoteLogin=RemoteCmd.rspRemoteLogin.newBuilder().build();
        m_ProcessorMap.put(rspRemoteLogin.getClass().toString(),new RspRemoteLoginProcessor());

        RemoteCmd.rspRemoteServerList rspRemoteServerList=RemoteCmd.rspRemoteServerList.newBuilder().build();
        m_ProcessorMap.put(rspRemoteServerList.getClass().toString(),new RspRemoteServerListProcessor());

        RemoteCmd.rspSelectLS rspSelectLS=RemoteCmd.rspSelectLS.newBuilder().build();
        m_ProcessorMap.put(rspSelectLS.getClass().toString(),new RspSelectLSProcessor());

    }
}
