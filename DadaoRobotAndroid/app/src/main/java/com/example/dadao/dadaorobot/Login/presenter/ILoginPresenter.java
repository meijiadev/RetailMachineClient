package com.example.dadao.dadaorobot.Login.presenter;

/**
 * mvp模式的presenter层接口被presenter类实现，该接口用于和View层通信
 * 当业务多了之后只需在这里 添加相应的接口方法
 */
public interface ILoginPresenter {
     void doLogin(String account, String password, boolean isLocalNetwork);
}
