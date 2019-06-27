package com.example.dadao.dadaorobot.Login.view;

/**
 * mvp设计模式中的 view层接口被相关Activity实现，通过该接口与presenter层通信
 */
public interface ILoginView {
    void LoginResult(boolean isLogin);


}
