package com.example.retailmachineclient.http;

import java.util.concurrent.TimeUnit;

import me.jessyan.retrofiturlmanager.RetrofitUrlManager;
import okhttp3.OkHttpClient;
import retrofit2.Retrofit;
import retrofit2.adapter.rxjava2.RxJava2CallAdapterFactory;
import retrofit2.converter.gson.GsonConverterFactory;

import static com.example.retailmachineclient.http.Api.APP_DEFAULT_DOMAIN;


/**
 * desc: http管理类,能够动态改变baseUrl
 * time：2020/7/1
 */
public class HttpManager {
    private HttpServer httpServer;
    private OkHttpClient okHttpClient;
    private Retrofit retrofit;
    private String baseUrl="http://192.168.0.95:8081/";            //获取版本信息接口

    private static class HttpManagerHolder{
        private static final HttpManager INSTANCE =new HttpManager();
    }

    public static HttpManager getInstance(){
        return HttpManagerHolder.INSTANCE;
    }

    private HttpManager(){
        this.okHttpClient= RetrofitUrlManager.getInstance().with(new OkHttpClient.Builder())
                .readTimeout(5, TimeUnit.SECONDS)
                .connectTimeout(5, TimeUnit.SECONDS)
                .build();
        this.retrofit=new Retrofit.Builder()
                .baseUrl(APP_DEFAULT_DOMAIN)                                  //默认的url
                .addConverterFactory(GsonConverterFactory.create())          //请求结果转换成实体类
                .addCallAdapterFactory(RxJava2CallAdapterFactory.create())  //适配Rxjava
                .client(okHttpClient)
                .build();
        this.httpServer=retrofit.create(HttpServer.class);
    }


    public HttpServer getHttpServer() {
        return httpServer;
    }

    public OkHttpClient getOkHttpClient() {
        return okHttpClient;
    }

    public Retrofit getRetrofit() {
        return retrofit;
    }

   /* public  void setBaseUrl(String IP) {
        this.baseUrl ="http://"+IP+":8081/";
        Logger.e("------baseUrl:"+baseUrl);
    }*/

    public String getBaseUrl() {
        return baseUrl;
    }
}
