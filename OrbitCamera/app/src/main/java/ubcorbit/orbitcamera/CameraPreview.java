package ubcorbit.orbitcamera;

import android.content.Context;
import android.hardware.Camera;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import java.io.IOException;
import java.util.List;

/** A basic Camera preview class */
@SuppressWarnings("deprecation")
public class CameraPreview extends SurfaceView implements SurfaceHolder.Callback {

    public String APP_NAME = "OrbitCamera";
    private SurfaceHolder holder;
    private Camera camera;

    // TODO: make dynamic and device independent
    // TODO: try 2448 x 3264
    private final static int PREVIEW_SIZE_MAX_WIDTH = 1080;
    private final static int PREVIEW_SIZE_MAX_HEIGHT = 1920;

    public CameraPreview(Context context) {
        super(context);
    }

    public CameraPreview(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public CameraPreview(Context context, Camera camera) {
        super(context);
        Log.d(APP_NAME, "CameraPreview.constructor");
        this.camera = camera;


        holder = getHolder();
        holder.addCallback(this);
        holder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
    }

    public void surfaceCreated(SurfaceHolder holder) {
        Log.d(APP_NAME, "CameraPreview:surfaceCreated");
        try {
            if(camera!=null){
                camera.setPreviewDisplay(holder);
                setupCamera();
                camera.startPreview();
            }
        } catch (IOException e) {
            Log.d(APP_NAME, "Error setting theCamera preview: " + e.getMessage());
        }
    }

    public void surfaceDestroyed(SurfaceHolder holder) {
        // do nothing
    }

    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
        Log.d(APP_NAME, "surfaceChanged");
        if (this.holder.getSurface() != null){
            // stop preview to make changes
            try {
                camera.stopPreview();
            } catch (Exception e){
                Log.d(APP_NAME, "error stopping preview");
            }

            // set preview size and make any resize, rotate or reformatting changes here

            // start preview with new settings
            try {
                camera.setPreviewDisplay(this.holder);
                camera.startPreview();
            } catch (Exception e){
                Log.d(APP_NAME, "Error starting theCamera preview: " + e.getMessage());
            }
        }
    }

    /**
     * Optimizes camera preview size given max screen resolution
     */
    public void setupCamera() {
        Camera.Parameters parameters = camera.getParameters();
        List<Camera.Size> sizes = parameters.getSupportedPreviewSizes();
        Camera.Size optimalSize = getOptimalPreviewSize(sizes, PREVIEW_SIZE_MAX_WIDTH, PREVIEW_SIZE_MAX_HEIGHT);
        parameters.setPictureSize(optimalSize.width, optimalSize.height);
        camera.setParameters(parameters);
    }


    private Camera.Size getOptimalPreviewSize(List<Camera.Size> sizes, int w, int h) {

        double ASPECT_TOLERANCE = 0.1;
        double targetRatio = (double) h / w;

        if (sizes == null)
            return null;

        Camera.Size optimalSize = null;
        double minDiff = Double.MAX_VALUE;

        for (Camera.Size size : sizes) {
            double ratio = (double) size.height / size.width;
            if (Math.abs(ratio - targetRatio) > ASPECT_TOLERANCE){
                continue;
            }
            if (Math.abs(size.height - h) < minDiff) {
                optimalSize = size;
                minDiff = Math.abs(size.height - h);
            }
        }

        if (optimalSize == null) {
            minDiff = Double.MAX_VALUE;
            for (Camera.Size size : sizes) {
                if (Math.abs(size.height - h) < minDiff) {
                    optimalSize = size;
                    minDiff = Math.abs(size.height - h);
                }
            }
        }

        return optimalSize;
    }

}