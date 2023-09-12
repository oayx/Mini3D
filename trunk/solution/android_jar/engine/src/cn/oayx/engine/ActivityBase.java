
package cn.oayx.engine;

import android.app.Activity;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.ViewGroup;
import android.widget.RelativeLayout;

public class ActivityBase extends Activity {
    static final String TAG = "ActivityBase";

    RelativeLayout mLayout;
    VRSurfaceView mSurfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        mLayout = new RelativeLayout(this);

        mSurfaceView = new VRSurfaceView(this);
        RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT);
        mLayout.addView(mSurfaceView, params);

        this.setContentView(mLayout);
    }

    protected void onPause() {
        mSurfaceView.onPause();
        super.onPause();
    }

    protected void onResume() {
        super.onResume();
        mSurfaceView.onResume();
    }

    protected void onDestroy() {
        mSurfaceView.onDestroy();
        super.onDestroy();
    }

    public boolean onKeyDown(int keyCode, KeyEvent event) {
        return mSurfaceView.onKeyDown(keyCode, event);
    }

    public boolean onKeyUp(int keyCode, KeyEvent event) {
        return mSurfaceView.onKeyUp(keyCode, event);
    }

    public boolean onTouchEvent(MotionEvent event) {
        return mSurfaceView.onTouchEvent(event);
    }
}
