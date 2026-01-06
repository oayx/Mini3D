
package cn.oayx.engine;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.res.AssetManager;
import android.view.Surface;
import android.view.View;
import android.view.WindowManager;
import android.view.inputmethod.InputMethodManager;

import java.io.File;

public class JNI {
    private Activity mActivity;
    private View mView;
    JNI(Activity activity, View view) {
        mActivity = activity;
        mView = view;
    }

    public int backToHome()
    {
        Intent home = new Intent(Intent.ACTION_MAIN);
        home.addCategory(Intent.CATEGORY_HOME);
        mActivity.startActivity(home);
        return 0;
    }

    public String getPackagePath()
    {
        return mActivity.getPackageResourcePath();
    }

    public String getFilesDirPath()
    {
        File files_dir = mActivity.getExternalFilesDir(null);
        if(files_dir == null) {
            files_dir = mActivity.getFilesDir();
        }
        return files_dir.getAbsolutePath();
    }

    public int quitApplication()
    {
        mActivity.finish();
        return 0;
    }

    public int switchKeyboard()
    {
        InputMethodManager imm=(InputMethodManager) mActivity.getSystemService(Context.INPUT_METHOD_SERVICE);
        imm.toggleSoftInput(0, InputMethodManager.HIDE_NOT_ALWAYS);
        return 0;
    }
    public int showKeyboard()
    {
        if(mActivity.getCurrentFocus() == null)
            return -1;
        InputMethodManager imm=(InputMethodManager) mActivity.getSystemService(Context.INPUT_METHOD_SERVICE);
        imm.showSoftInput(mActivity.getCurrentFocus(),InputMethodManager.SHOW_FORCED);
        return 0;
    }
    public int hideKeyboard()
    {
        if(mActivity.getCurrentFocus() == null)
            return -1;
        InputMethodManager imm=(InputMethodManager) mActivity.getSystemService(Context.INPUT_METHOD_SERVICE);
        imm.hideSoftInputFromWindow(mActivity.getCurrentFocus().getWindowToken(),InputMethodManager.HIDE_NOT_ALWAYS);
        return 0;
    }
    public int isKeyboardOpen()
    {
        InputMethodManager imm = (InputMethodManager)mActivity.getSystemService(Context.INPUT_METHOD_SERVICE);
        boolean isOpen = imm.isActive();
        return isOpen ? 1 : 0;
    }

    public int keepScreenOn(final boolean enable)
    {
        mActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if(enable) {
                    mActivity.getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
                } else {
                    mActivity.getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
                }
            }
        });
        return 0;
    }

    public native void engineCreate(Surface surface, int width, int height, AssetManager assetManager);
    public native void engineCreateed();
    public native void engineDestroy();
    public native void engineSurfaceCreated(Surface surface, int width, int height);
    public native void engineSurfaceResize(Surface surface, int width, int height);
    public native void engineSurfceDestroy(Surface surface);
    public native void enginePause();
    public native void engineResume();
    public native void engineDraw();
    public native void engineKeyDown(int keyCode, int ch);
    public native void engineKeyUp(int keyCode, int ch);
    public native void engineTouch(byte[] touchData);

    static
    {
        System.loadLibrary("engine");
    }
}
