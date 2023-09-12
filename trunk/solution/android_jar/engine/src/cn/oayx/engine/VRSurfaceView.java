
package cn.oayx.engine;

import android.app.Activity;
import android.content.Context;
import android.graphics.PixelFormat;
import android.graphics.Rect;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.util.LinkedList;

public class VRSurfaceView extends SurfaceView {
    static final String TAG = "VRSurfaceView";

    Activity mActivity;
    JNI jni;
    Thread mRenderThread;
    boolean mEngineCreated = false;
    volatile boolean mStopRenderThread = false;
    volatile boolean mPaused = false;
    final Object mLock = new Object();
    LinkedList<Runnable> mActions = new LinkedList<Runnable>();

    public VRSurfaceView(Context context) {
        super(context);
        mActivity = (Activity) context;
        jni = new JNI(mActivity, this);
        mRenderThread = new Thread(new Runnable() {
            @Override
            public void run() {
                while (!mStopRenderThread) {
                    synchronized (mLock) {
                        for (int i = 0; i < mActions.size(); ++i) {
                            Runnable r = mActions.get(i);
                            if (r != null) {
                                r.run();
                            }
                        }
                        mActions.clear();
                    }

                    if (!mPaused) {
                        if (mEngineCreated) {
                            jni.engineDraw();
                        }
                    }
                }

                if (mEngineCreated) {
                    jni.engineDestroy();
                }
            }
        });
        mRenderThread.start();

        this.getHolder().setFormat(PixelFormat.RGB_888);
        this.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder surfaceHolder) {
                Log.i(TAG, "surfaceCreated");
            }

            @Override
            public void surfaceChanged(SurfaceHolder surfaceHolder, int format, int width, int height) {
                Log.i(TAG, "surfaceChanged");

                queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        SurfaceHolder holder = VRSurfaceView.this.getHolder();
                        Rect rect = holder.getSurfaceFrame();

                        if (!mEngineCreated) {
                            mEngineCreated = true;
                            jni.engineCreate(holder.getSurface(), rect.width(), rect.height(), mActivity.getAssets());
                            jni.engineCreateed();
                        } else {
                            jni.engineSurfaceResize(holder.getSurface(), rect.width(), rect.height());
                        }
                    }
                });
            }

            @Override
            public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
                Log.i(TAG, "surfaceDestroyed");

                queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        if (mEngineCreated) {
                            jni.engineSurfaceDestroy();
                        }
                    }
                });
            }
        });
    }

    public void queueEvent(Runnable r) {
        synchronized (mLock) {
            mActions.addLast(r);
        }
    }

    public void onPause() {
        Log.i(TAG, "onPause");
        this.queueEvent(new Runnable() {
            @Override
            public void run() {
                mPaused = true;
                if (mEngineCreated) {
                    jni.enginePause();
                }
            }
        });
    }

    public void onResume() {
        Log.i(TAG, "onResume");
        this.queueEvent(new Runnable() {
            @Override
            public void run() {
                mPaused = false;
                if (mEngineCreated) {
                    jni.engineResume();
                }
            }
        });
    }

    public void onDestroy() {
        Log.i(TAG, "onDestroy");
        mStopRenderThread = true;
    }

    public boolean onKeyDown(int keyCode, final KeyEvent event) {
        final int code = event.getKeyCode();
        final int ch = event.getUnicodeChar();
        this.queueEvent(new Runnable() {
            @Override
            public void run() {
                if (mEngineCreated) {
                    jni.engineKeyDown(code,ch);
                }
            }
        });

        return super.onKeyDown(keyCode, event);
    }

    public boolean onKeyUp(int keyCode, final KeyEvent event) {
        final int code = event.getKeyCode();
        final int ch = event.getUnicodeChar();
        this.queueEvent(new Runnable() {
            @Override
            public void run() {
                if (mEngineCreated) {
                    jni.engineKeyUp(code,ch);
                }
            }
        });

        return super.onKeyUp(keyCode, event);
    }

    public boolean onTouchEvent(MotionEvent event) {
        int action = event.getAction();
        int count = event.getPointerCount();
        int index = (action & 0xff00) >> 8;
        int id = event.getPointerId(index);
        long time = event.getEventTime();
        int act = action & 0xff;

        try {
            final ByteArrayOutputStream baos = new ByteArrayOutputStream();
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

            this.queueEvent(new Runnable() {
                @Override
                public void run() {
                    if (mEngineCreated) {
                        jni.engineTouch(baos.toByteArray());
                    }
                }
            });
        } catch (IOException e) {
            e.printStackTrace();
        }

        return super.onTouchEvent(event);
    }
}
