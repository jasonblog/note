package net.macdidi.sensor01;

import android.content.Context;
import android.graphics.SurfaceTexture;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CameraMetadata;
import android.hardware.camera2.CaptureRequest;
import android.util.Log;
import android.util.Size;
import android.view.Surface;

import java.util.ArrayList;
import java.util.List;

public class FlashControl
{

    private CameraCaptureSession session;
    private CameraDevice cameraDevice;
    private CameraManager cameraManager;
    private CaptureRequest.Builder builder;

    private Context context;
    private ReadyCallBack callBack;

    public FlashControl(Context context, ReadyCallBack callBack)
    throws CameraAccessException {
        this.context = context;
        this.callBack = callBack;
        init();
    }

    // 開啟閃光燈
    public void turnOn()
    {
        builder.set(CaptureRequest.FLASH_MODE,
                    CameraMetadata.FLASH_MODE_TORCH);

        try {
            session.setRepeatingRequest(builder.build(), null, null);
        } catch (CameraAccessException e) {
            Log.d("FlashControl", e.toString());
        }
    }

    // 關閉閃光燈
    public void turnOff()
    {
        builder.set(CaptureRequest.FLASH_MODE,
                    CameraMetadata.FLASH_MODE_OFF);

        try {
            session.setRepeatingRequest(builder.build(), null, null);
        } catch (CameraAccessException e) {
            Log.d("FlashControl", e.toString());
        }
    }

    // 關閉與清除資源
    public void close()
    {
        if (cameraDevice == null || session == null) {
            return;
        }

        session.close();
        cameraDevice.close();

        cameraDevice = null;
        session = null;
    }

    private void init() throws CameraAccessException, SecurityException {
        cameraManager = (CameraManager)
        context.getSystemService(Context.CAMERA_SERVICE);
        String[] devices = cameraManager.getCameraIdList();

        if (devices == null || devices.length == 0)
        {
            throw new CameraAccessException(
                CameraAccessException.CAMERA_ERROR,
                "No camera device.");
        }

        CameraCharacteristics cameraCharacteristics =
        cameraManager.getCameraCharacteristics(devices[0]);
        boolean flashAvailable = cameraCharacteristics.get(
            CameraCharacteristics.FLASH_INFO_AVAILABLE);

        if (!flashAvailable)
        {
            throw new CameraAccessException(
                CameraAccessException.CAMERA_ERROR,
                "Flash not available.");
        }

        cameraManager.openCamera(devices[0],
                                 new MyCameraDeviceStateCallback(), null);
    }

    private SurfaceTexture surfaceTexture;
    private Surface surface;

    private class MyCameraDeviceStateCallback
        extends CameraDevice.StateCallback
    {

        @Override
        public void onOpened(CameraDevice camera)
        {
            cameraDevice = camera;

            try {
                builder = camera.createCaptureRequest(
                              CameraDevice.TEMPLATE_MANUAL);
                builder.set(CaptureRequest.FLASH_MODE,
                            CameraMetadata.FLASH_MODE_TORCH);

                List<Surface> list = new ArrayList<>();
                surfaceTexture = new SurfaceTexture(1);
                Size size = getSmallestSize(cameraDevice.getId());
                surfaceTexture.setDefaultBufferSize(
                    size.getWidth(), size.getHeight());
                surface = new Surface(surfaceTexture);
                list.add(surface);
                builder.addTarget(surface);

                camera.createCaptureSession(list,
                                            new MyCameraCaptureSessionStateCallback(), null);
            } catch (CameraAccessException e) {
                Log.d("FlashControl", e.toString());
            }
        }

        @Override
        public void onDisconnected(CameraDevice camera)
        {
            Log.d("FlashControl", camera.toString());
        }

        @Override
        public void onError(CameraDevice camera, int error)
        {
            Log.d("FlashControl", camera.toString() + ":" + error);
        }
    }

    private Size getSmallestSize(String cameraId)
    throws CameraAccessException {
        Size[] outputSizes = cameraManager
        .getCameraCharacteristics(cameraId)
        .get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP)
        .getOutputSizes(SurfaceTexture.class);

        if (outputSizes == null || outputSizes.length == 0)
        {
            throw new IllegalStateException(
                "Camera " + cameraId + "doesn't support any outputSize.");
        }

        Size chosen = outputSizes[0];

        for (Size s : outputSizes)
        {
            if (chosen.getWidth() >= s.getWidth() &&
                chosen.getHeight() >= s.getHeight()) {
                chosen = s;
            }
        }

        return chosen;
    }

    private class MyCameraCaptureSessionStateCallback
        extends CameraCaptureSession.StateCallback
    {

        @Override
        public void onConfigured(CameraCaptureSession session)
        {
            FlashControl.this.session = session;

            try {
                FlashControl.this.session.setRepeatingRequest(
                    builder.build(), null, null);
                // 通知Activity元件已經準備好了
                callBack.ready();
            } catch (CameraAccessException e) {
                Log.d("FlashControl", e.toString());
            }
        }

        @Override
        public void onConfigureFailed(CameraCaptureSession session)
        {
            Log.d("FlashControl", session.toString());
        }
    }

    // 提供給Activity元件實作的介面
    public interface ReadyCallBack
    {
        public void ready();
    }

}
