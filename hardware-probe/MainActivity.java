package org.lineageos.gta3xlwifi.hardwareprobe;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CaptureRequest;
import android.media.AudioAttributes;
import android.media.AudioFormat;
import android.media.AudioTrack;
import android.media.Image;
import android.media.ImageReader;
import android.graphics.ImageFormat;
import android.util.Log;
import android.util.Size;
import android.widget.TextView;
import java.util.Arrays;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicReference;
import java.net.URL;
import javax.net.ssl.HttpsURLConnection;

public final class MainActivity extends Activity {
    private static final String TAG = "Gta3xlwifiHardwareProbe";
    private TextView status;
    private void pass(String message) {
        Log.i(TAG, "PASS " + message);
        runOnUiThread(() -> status.append("PASS " + message + "\n"));
    }
    private static void require(boolean result, String reason) {
        if (!result) throw new IllegalStateException(reason);
    }
    @Override public void onCreate(Bundle state) {
        super.onCreate(state);
        status = new TextView(this);
        status.setText("SM-T510 hardware check\nNo images or recordings are saved.\n");
        setContentView(status);
        new Thread(() -> {
            HandlerThread callbacks = new HandlerThread("HardwareCallbacks");
            callbacks.start();
            Handler handler = new Handler(callbacks.getLooper());
            try {
                sensor(handler);
                network();
                audio();
                cameras(handler);
                pass("ALL");
            } catch (Exception error) {
                Log.e(TAG, "FAIL " + error, error);
                runOnUiThread(() -> status.append("FAIL " + error + "\n"));
            } finally {
                callbacks.quitSafely();
            }
        }, "HardwareProbe").start();
    }
    private void network() throws Exception {
        HttpsURLConnection connection=(HttpsURLConnection)new URL("https://example.com/").openConnection();
        connection.setConnectTimeout(10000);
        connection.setReadTimeout(10000);
        try {
            require(connection.getResponseCode()==200,"HTTPS response failed");
            try (java.io.InputStream input=connection.getInputStream()) {
                require(input.read()!=-1,"HTTPS response empty");
            }
            pass("HTTPS status=200");
        } finally { connection.disconnect(); }
    }
    private void sensor(Handler handler) throws Exception {
        SensorManager manager = getSystemService(SensorManager.class);
        Sensor sensor = manager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        require(sensor != null, "No accelerometer");
        CountDownLatch samples = new CountDownLatch(3);
        SensorEventListener listener = new SensorEventListener() {
            @Override public void onAccuracyChanged(Sensor s, int accuracy) {}
            @Override public void onSensorChanged(SensorEvent event) {
                double norm = Math.sqrt(event.values[0]*event.values[0] +
                    event.values[1]*event.values[1] + event.values[2]*event.values[2]);
                if (norm > 6 && norm < 14) samples.countDown();
            }
        };
        require(manager.registerListener(listener, sensor, SensorManager.SENSOR_DELAY_NORMAL, handler),
            "Accelerometer registration failed");
        try { require(samples.await(8, TimeUnit.SECONDS), "No plausible accelerometer samples"); }
        finally { manager.unregisterListener(listener); }
        pass("ACCELEROMETER samples=3");
    }
    private void audio() throws Exception {
        int rate = 22050, frames = rate*2;
        short[] tone = new short[frames];
        for (int i=0; i<frames; i++) {
            double envelope = Math.min(1.0, Math.min(i,frames-1-i)/(rate*0.05));
            tone[i] = (short)(Math.sin(2*Math.PI*440*i/rate)*4000*envelope);
        }
        AudioTrack track = new AudioTrack.Builder()
            .setAudioAttributes(new AudioAttributes.Builder().setUsage(AudioAttributes.USAGE_MEDIA)
                .setContentType(AudioAttributes.CONTENT_TYPE_SONIFICATION).build())
            .setAudioFormat(new AudioFormat.Builder().setSampleRate(rate)
                .setEncoding(AudioFormat.ENCODING_PCM_16BIT).setChannelMask(AudioFormat.CHANNEL_OUT_MONO).build())
            .setTransferMode(AudioTrack.MODE_STATIC).setBufferSizeInBytes(frames*2).build();
        try {
            require(track.getState()==AudioTrack.STATE_NO_STATIC_DATA,"AudioTrack allocation failed");
            require(track.write(tone,0,frames)==frames,"AudioTrack write incomplete");
            require(track.getState()==AudioTrack.STATE_INITIALIZED,"AudioTrack initialization failed");
            track.setVolume(0.3f);
            track.play();
            long deadline = android.os.SystemClock.elapsedRealtime()+6000;
            while (track.getPlaybackHeadPosition()<frames && android.os.SystemClock.elapsedRealtime()<deadline)
                Thread.sleep(50);
            require(track.getPlaybackHeadPosition()>=frames,"Audio output did not advance");
            pass("AUDIO frames="+frames+" route="+track.getRoutedDevice().getType());
        } finally { track.release(); }
    }
    private void cameras(Handler handler) throws Exception {
        CameraManager manager = getSystemService(CameraManager.class);
        boolean front=false, back=false;
        for (String id:manager.getCameraIdList()) {
            CameraCharacteristics properties = manager.getCameraCharacteristics(id);
            Integer facing = properties.get(CameraCharacteristics.LENS_FACING);
            if (facing==null || (facing!=CameraCharacteristics.LENS_FACING_FRONT &&
                                facing!=CameraCharacteristics.LENS_FACING_BACK)) continue;
            Size[] sizes = properties.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP)
                .getOutputSizes(ImageFormat.YUV_420_888);
            require(sizes!=null && sizes.length>0,"No YUV camera output");
            Size size = sizes[0];
            for (Size candidate:sizes)
                if ((long)candidate.getWidth()*candidate.getHeight()<(long)size.getWidth()*size.getHeight())
                    size=candidate;
            ImageReader reader = ImageReader.newInstance(size.getWidth(),size.getHeight(),ImageFormat.YUV_420_888,3);
            CountDownLatch frames = new CountDownLatch(6);
            long[] lastTimestamp = {0};
            reader.setOnImageAvailableListener(source -> {
                try (Image image=source.acquireNextImage()) {
                    if (image!=null && image.getTimestamp()>lastTimestamp[0]) {
                        lastTimestamp[0]=image.getTimestamp(); frames.countDown();
                    }
                } catch (IllegalStateException closedReader) {
                    // A queued callback can arrive after the stream is closed.
                }
            },handler);
            AtomicReference<CameraDevice> device = new AtomicReference<>();
            AtomicReference<CameraCaptureSession> session = new AtomicReference<>();
            AtomicInteger error = new AtomicInteger(0);
            CountDownLatch opened = new CountDownLatch(1), configured = new CountDownLatch(1);
            try {
                manager.openCamera(id,new CameraDevice.StateCallback() {
                    @Override public void onOpened(CameraDevice camera) { device.set(camera); opened.countDown(); }
                    @Override public void onDisconnected(CameraDevice camera) { error.set(-1); camera.close(); opened.countDown(); }
                    @Override public void onError(CameraDevice camera,int code) { error.set(code); camera.close(); opened.countDown(); }
                },handler);
                require(opened.await(10,TimeUnit.SECONDS) && device.get()!=null && error.get()==0,"Camera open failed");
                device.get().createCaptureSession(Arrays.asList(reader.getSurface()),new CameraCaptureSession.StateCallback() {
                    @Override public void onConfigured(CameraCaptureSession capture) { session.set(capture); configured.countDown(); }
                    @Override public void onConfigureFailed(CameraCaptureSession capture) { error.set(-2); configured.countDown(); }
                },handler);
                require(configured.await(10,TimeUnit.SECONDS) && session.get()!=null && error.get()==0,"Camera configuration failed");
                CaptureRequest.Builder request=device.get().createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
                request.addTarget(reader.getSurface());
                session.get().setRepeatingRequest(request.build(),null,handler);
                require(frames.await(12,TimeUnit.SECONDS),"Camera produced fewer than six frames");
                pass("CAMERA facing="+(facing==CameraCharacteristics.LENS_FACING_FRONT ? "front" : "back")+" frames=6");
                front |= facing==CameraCharacteristics.LENS_FACING_FRONT;
                back |= facing==CameraCharacteristics.LENS_FACING_BACK;
            } finally {
                if (session.get()!=null) session.get().close();
                if (device.get()!=null) device.get().close();
                reader.setOnImageAvailableListener(null,null);
                reader.close();
            }
        }
        require(front && back,"Both cameras must stream frames");
    }
}
