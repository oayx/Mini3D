package cn.oayx.engine;

import android.app.Activity;
import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;

import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.IOException;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class VRSurfaceView extends GLSurfaceView implements GLSurfaceView.Renderer {
    static final String TAG = "VRSurfaceView";

    Activity mActivity;
    JNI jni;
    boolean isEngineCreated = false;
    volatile boolean isPaused = false;

    public VRSurfaceView(Context context) {
        super(context);
        mActivity = (Activity) context;
        jni = new JNI(mActivity, this);

        // 配置GLSurfaceView
        setEGLContextClientVersion(3); // 根据实际使用的OpenGL ES版本设置
        setEGLConfigChooser(true);
        setEGLConfigChooser(8, 8, 8, 8, 24, 8); // RGB888 + 深度缓冲
        setRenderer(this);
        setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
        setPreserveEGLContextOnPause(true);//避免EGL资源释放导致需要重建
    }

    // region Renderer接口实现
    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        Log.i(TAG, "onSurfaceCreated");
        if(!isEngineCreated){
            isEngineCreated = true;
            jni.engineCreate(getHolder().getSurface(),
                    getWidth(),
                    getHeight(),
                    mActivity.getAssets());
            jni.engineCreateed();
        }
        jni.engineSurfaceCreated(getHolder().getSurface(), getWidth(), getHeight());
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        Log.i(TAG, "onSurfaceChanged");
        if (isEngineCreated) {
            jni.engineSurfaceResize(getHolder().getSurface(), width, height);
        }
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        if (!isPaused && isEngineCreated) {
            jni.engineDraw();
        }
    }
    // endregion

    // region 生命周期管理
    public void onPause() {
        Log.i(TAG, "onPause");
        super.onPause();
        queueEvent(() -> {
            isPaused = true;
            if (isEngineCreated) {
                jni.enginePause();
            }
        });
    }

    public void onResume() {
        Log.i(TAG, "onResume");
        super.onResume();
        queueEvent(() -> {
            isPaused = false;
            if (isEngineCreated) {
                jni.engineResume();
            }
        });
    }

    @Override
    public void onDetachedFromWindow() {
        super.onDetachedFromWindow();
        queueEvent(() -> {
            if (isEngineCreated) {
                jni.engineDestroy();
                isEngineCreated = false;
            }
        });
    }
    // endregion

    // region 输入事件处理
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        final int code = event.getKeyCode();
        final int ch = event.getUnicodeChar();
        queueEvent(() -> {
            if (isEngineCreated) {
                jni.engineKeyDown(code, ch);
            }
        });
        return super.onKeyDown(keyCode, event);
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        final int code = event.getKeyCode();
        final int ch = event.getUnicodeChar();
        queueEvent(() -> {
            if (isEngineCreated) {
                jni.engineKeyUp(code, ch);
            }
        });
        return super.onKeyUp(keyCode, event);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        int action = event.getAction();
        int count = event.getPointerCount();
        int index = (action & 0xff00) >> 8;
        int id = event.getPointerId(index);
        long time = event.getEventTime();
        int act = action & 0xff;

        try {
            ByteArrayOutputStream baos = new ByteArrayOutputStream();
            DataOutputStream dos = new DataOutputStream(baos);

            dos.writeInt(act);
            dos.writeInt(index);
            dos.writeInt(id);
            dos.writeInt(count);
            dos.writeLong(time);
            for (int i = 0; i < count; i++) {
                dos.writeFloat(event.getX(i));
                dos.writeFloat(event.getY(i));
            }

            queueEvent(() -> {
                if (isEngineCreated) {
                    jni.engineTouch(baos.toByteArray());
                }
            });
        } catch (IOException e) {
            e.printStackTrace();
        }

        return super.onTouchEvent(event);
    }
    // endregion
}