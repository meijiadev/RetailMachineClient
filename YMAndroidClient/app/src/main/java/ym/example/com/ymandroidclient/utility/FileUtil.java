package ym.example.com.ymandroidclient.utility;

import android.os.Environment;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;

/**
 *作用：文件操作，将文件保存到本地
 *
 */
public class FileUtil {
    private String sdPath;
    public  int MaxLength=1024*2;

    public FileUtil(String path){
        sdPath=Environment.getExternalStorageDirectory().getPath()+"/"+"机器人"+"/"+path+"/";
        createSdDir(sdPath);
    }


    /**
     * 在Sd卡创建目录
     * @return
     */
    public File createSdDir(String sdPath){
        File dir=new File(sdPath);
        if (dir.exists()){
            //  Logger.e("文件夹已存在，无须创建");
        }else {
            Logger.e("创建文件");
            dir.mkdirs();
        }
        return dir;
    }

    /**
     * 在Sd卡创建文件
     * @return
     */
    public File createSdFile(String fileName) throws IOException {
        String finalFileName=sdPath+fileName;
        //Logger.e("最终的文件名和路径："+finalFileName);
        File file=new File(finalFileName);
        file.createNewFile();
        return file;
    }

    /**
     * 复制文件
     *
     * @param source 输入文件
     * @param target 输出文件
     */
    public void copy(File source, File  target) {
        FileInputStream fileInputStream = null;
        FileOutputStream fileOutputStream = null;
        try {
            fileInputStream = new FileInputStream(source);   //字节读取流
            fileOutputStream = new FileOutputStream(target);  //字节输入流
            byte[] buffer = new byte[1024];
            while (fileInputStream.read(buffer) > 0) {
                fileOutputStream.write(buffer);
            }
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                if (fileInputStream != null) {
                    fileInputStream.close();
                }
                if (fileOutputStream != null) {
                    fileOutputStream.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public void writeToSdFromInput(String urlstr,String fileName) throws IOException {
        URL url=new URL(urlstr);
        byte[] downloadBytes;
        File file=createSdFile(fileName);
        OutputStream outputStream=new FileOutputStream(file);
        HttpURLConnection httpURLConnection= (HttpURLConnection) url.openConnection();  //打开一个HttpURLConnection 的连接
        httpURLConnection.setUseCaches(true);
        InputStream inputStream=httpURLConnection.getInputStream();
        int len=httpURLConnection.getContentLength();
        if (len>MaxLength){
            downloadBytes=new byte[MaxLength];
            while (inputStream.read(downloadBytes)!=-1){
                outputStream.write(downloadBytes);
            }
            outputStream.close();
        }else {
            downloadBytes=new byte[len];
            inputStream.read(downloadBytes);
            outputStream.write(downloadBytes);
            outputStream.flush();
        }

    }

}
