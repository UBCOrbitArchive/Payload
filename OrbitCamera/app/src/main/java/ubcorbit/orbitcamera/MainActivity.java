package ubcorbit.orbitcamera;

import android.content.pm.PackageManager;
import android.hardware.Camera;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.Toast;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

@SuppressWarnings("deprecation")
public class MainActivity extends AppCompatActivity {

    public Button button;                          // image capture button
    public String APP_NAME = "OrbitCamera";
    public Camera theCamera;                            // reference to a device camera
    public CameraPreview preview;                       // view to display camera stream
    public FrameLayout previewLayout;                   // layout holding the camera preview

    /**
     * Gets called on image capture
     */
    public Camera.PictureCallback JpegPictureCallback = new Camera.PictureCallback() {

        @Override
        public void onPictureTaken(byte[] data, Camera camera) {
            Log.d(APP_NAME, "JpegPictureCallback:onPictureTaken()");

            // Get reference to file with timestamped name in the shared directory for this app
            File pictureFile = getOutputMediaFile();

            if (pictureFile!=null) {
                Log.d(APP_NAME, "onPictureTaken() : successfully got file");
                try {
                    FileOutputStream fos = new FileOutputStream(pictureFile);
                    fos.write(data);
                    Log.d(APP_NAME, "onPictureTaken() : successfully wrote to file");
                    fos.close();
                    Toast.makeText(getApplicationContext(), "OK!", Toast.LENGTH_SHORT).show();

                    // TODO: refactor to async task
                    camera.startPreview();

                } catch (FileNotFoundException e) {
                    Log.d(APP_NAME, "onPictureTaken() : file not found: " + e.getMessage());
                } catch (IOException e) {
                    Log.d(APP_NAME, "onPictureTaken() : error opening file: " + e.getMessage());
                } catch (Exception e) {
                    Log.d(APP_NAME, "onPictureTaken() : unexpected error accessing file: " + e.getMessage());
                }
            } else {
                Log.d(APP_NAME, "Error creating media file, check storage permissions");
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.activity_main);
        previewLayout = (FrameLayout) findViewById(R.id.camera_preview_layout);

        initTheCamera();
        initButton();
    }

    @Override
    protected void onResume(){
        super.onResume();
        initTheCamera();
    }

    @Override
    protected void onPause(){
        super.onPause();
        theCamera.stopPreview();
        releaseCamera();
    }

    @Override
    protected void onStop(){
        super.onStop();
        theCamera.stopPreview();
        releaseCamera();
    }

    private void initButton(){
        button = (Button) findViewById(R.id.camera_button);
        if(button != null) {
            button.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    captureImage(v);
                }
            });
        }
    }

    private void initTheCamera(){
        if(deviceHasCamera()){
            Log.d(APP_NAME, "Camera found");
            theCamera = getCamera();
            preview = new CameraPreview(getApplicationContext(), theCamera);
            preview.setVisibility(View.VISIBLE);
            previewLayout.addView(preview);
        } else {
            Log.d(APP_NAME, "No camera found on device");
        }
    }

    /**
     * Opens first camera on device and returns a reference to camera instance
     */
    private Camera getCamera(){
        Log.d(APP_NAME, "Main:getCamera():");
        Camera camera = null;
        try {
            camera = Camera.open();
            Log.d(APP_NAME, "Successfully opened camera");
        } catch (Exception e){
            Log.d("OrbitCamera", "Error opening camera: " + e.getMessage());
        }
        return camera;
    }

    private boolean deviceHasCamera(){
        Log.d(APP_NAME, "Main:deviceHasCamera()");
        return getApplicationContext().getPackageManager().hasSystemFeature(PackageManager.FEATURE_CAMERA);
    }

    /**
     * Releases the camera instance set for this activity if there is one
     */
    private void releaseCamera(){
        Log.d(APP_NAME, "Main:releaseCamera()");
        if (theCamera != null){
            theCamera.release();
            Log.d(APP_NAME, "successfully released camera");
            theCamera = null;
        }
    }

    /**
     *  Takes a picture using the activity camera instance if available
     *  Used as a callback for the image capture button
     * @param view : the clicked button
     */
    private void captureImage(View view){
        Log.d(APP_NAME, "Main:captureImage()");
        if(theCamera !=null) {
            try{
                theCamera.takePicture(null, null, JpegPictureCallback);
            } catch(Exception e) {
                Log.e(APP_NAME, e.getMessage());
            }
        } else {
            Log.d(APP_NAME, "Camera pointer is null");
        }
    }

    /**
     * @return : a timestamp-named file in device's shared picture directory for writing captured image
     */
    private static File getOutputMediaFile(){
        Log.d("UBCOrbitCameraApp", "Main:getOutputMediaFile");

        // get shared  application directory on device
        File mediaStorageDir = new File(Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES), "UBCOrbitCameraApp");

        // create the storage directory if it does not exist
        if (! mediaStorageDir.exists()){
            Log.d("OrbitCamera", "Media storage directory does not exist");
            if (! mediaStorageDir.mkdirs()){
                Log.d("OrbitCamera", "Failed to create media storage directory:");
                return null;
            }
        }

        // Create a media file name
        String timeStamp = new SimpleDateFormat("MMdd_HHmmss", Locale.CANADA).format(new Date());
        return new File(mediaStorageDir.getPath() + File.separator + "IMG_"+ timeStamp + ".jpg");
    }
}
