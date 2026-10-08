package org.lineageos.gta3xlwifi.smoke;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.LinearLayout;
import android.widget.TextView;

public final class MainActivity extends Activity implements SurfaceHolder.Callback {
    static { System.loadLibrary("gta3xlwifi_abi_probe"); }
    private static native String probe(Surface surface);
    private TextView status;
    private Thread worker;

    @Override public void onCreate(Bundle state) {
        super.onCreate(state);
        LinearLayout layout = new LinearLayout(this);
        layout.setOrientation(LinearLayout.VERTICAL);
        status = new TextView(this);
        status.setPadding(24, 24, 24, 24);
        status.setText("Checking native ABI and Mali EGL/GLES…");
        layout.addView(status);
        SurfaceView view = new SurfaceView(this);
        view.getHolder().addCallback(this);
        layout.addView(view, new LinearLayout.LayoutParams(-1, 0, 1));
        setContentView(layout);
    }

    @Override public synchronized void surfaceCreated(SurfaceHolder holder) {
        worker = new Thread(() -> {
            String result = probe(holder.getSurface());
            Log.i("Gta3xlwifiAbiProbe", getPackageName() + " " + result);
            runOnUiThread(() -> status.setText(result));
        }, "MaliEglCheck");
        worker.start();
    }

    @Override public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {}

    @Override public synchronized void surfaceDestroyed(SurfaceHolder holder) {
        if (worker != null) {
            try { worker.join(); } catch (InterruptedException e) { Thread.currentThread().interrupt(); }
            worker = null;
        }
    }
}
