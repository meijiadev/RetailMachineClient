package com.example.retailmachineclient.util;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;
import java.util.TimeZone;

public class TimeUtils {

    /**
     * 时间戳转固定格式的时间
     * @return
     */
    public static String longToDate(long longTime){
        SimpleDateFormat dff = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss", Locale.getDefault());
        String time=dff.format(longTime);
        return time;
    }

    /**
     * 时间戳转固定格式的时间
     * @return
     */
    public static String longToDate1(long longTime){
        SimpleDateFormat dff = new SimpleDateFormat("yyyy-MM-dd HH:mm", Locale.getDefault());
        String time=dff.format(longTime);
        return time;
    }




    /**
     * 获取当前时分秒
     * @return
     */
    public static String getCurrentTime1(){
        SimpleDateFormat dff = new SimpleDateFormat("HH:mm:ss");
        dff.setTimeZone(TimeZone.getTimeZone("GMT+08"));
        String ee = dff.format(new Date());
       // Logger.e("-----当前时间："+ee);
        return ee;
    }

    public static String getCurrentTime(){
        SimpleDateFormat dff = new SimpleDateFormat("HH:mm");
        dff.setTimeZone(TimeZone.getTimeZone("GMT+08"));
        String ee = dff.format(new Date());
        // Logger.e("-----当前时间："+ee);
        return ee;
    }

    /**
     * 如果时间1早于时间2则返回true
     * @param time1
     * @param time2
     * @return
     * @throws ParseException
     */
    public static boolean compareTime(String time1,String time2) throws ParseException {
        SimpleDateFormat dff = new SimpleDateFormat("HH:mm");
        Date a=dff.parse(time1);
        Date b=dff.parse(time2);
        return a.before(b);
    }

    /**
     * 获取当前年月日
     * @return
     */
    public static String getCurrentTime2(){
        SimpleDateFormat dff = new SimpleDateFormat("yyyy-MM-dd");
        dff.setTimeZone(TimeZone.getTimeZone("GMT+08"));
        String ee = dff.format(new Date());
        return ee;
    }

    /**
     * 获取当前年月日时分秒
     * @return
     */
    public static String getCurrentTime3(){
        SimpleDateFormat dff = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        dff.setTimeZone(TimeZone.getTimeZone("GMT+08"));
        String ee = dff.format(new Date());
        return ee;
    }

    /**
     * 获取当前年月日时分
     * @return
     */
    public static String getCurrentTime4(){
        SimpleDateFormat dff = new SimpleDateFormat("yyyy-MM-dd HH:mm");
        dff.setTimeZone(TimeZone.getTimeZone("GMT+08"));
        String ee = dff.format(new Date());
        return ee;
    }

    /**
     * string型时间转long
     * @param strTime
     * @param formatType
     * @return
     * @throws ParseException
     */
    public static long stringToLong(String strTime,String formatType) throws ParseException {
        Date date=stringToDate(strTime,formatType);
        if (date==null){
            return 0;
        }else {
            return date.getTime();
        }
    }

    // strTime要转换的string类型的时间，formatType要转换的格式yyyy-MM-dd HH:mm:ss//yyyy年MM月dd日
    // HH时mm分ss秒，
    // strTime的时间格式必须要与formatType的时间格式相同
    public static Date stringToDate(String strTime, String formatType)
            throws ParseException {
        SimpleDateFormat formatter = new SimpleDateFormat(formatType);
        Date date = null;
        date = formatter.parse(strTime);
        return date;
    }

}
