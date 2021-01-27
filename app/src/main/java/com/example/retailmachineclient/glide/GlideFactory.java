package com.example.retailmachineclient.glide;

import android.content.Context;
import android.graphics.drawable.Drawable;

import com.bumptech.glide.Glide;
import com.example.retailmachineclient.R;

import androidx.core.content.ContextCompat;


/**
 *    time   : 2018/12/27
 *    desc   : Glide 加工厂
 */
public final class GlideFactory implements ImageFactory<GlideStrategy> {

    @Override
    public GlideStrategy createImageStrategy() {
        return new GlideStrategy();
    }

    @Override
    public Drawable createPlaceholder(Context context) {
        return ContextCompat.getDrawable(context, R.mipmap.image_loading);
    }

    @Override
    public Drawable createError(Context context) {
        return ContextCompat.getDrawable(context, R.mipmap.image_load_err);
    }

    @Override
    public void clearMemoryCache(Context context) {
        // 清除内存缓存（必须在主线程）
        Glide.get(context).clearMemory();
    }

    @Override
    public void clearDiskCache(final Context context) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                // 清除本地缓存（必须在子线程）
                Glide.get(context).clearDiskCache();
            }
        }).start();
    }
}