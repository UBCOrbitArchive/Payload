// import library to access android sensors
import ketai.camera.*;

KetaiCamera cam;

void setup() {
  // choose P2D renderer
  
  // 1280x768 is the resolution my genymotion simulation runs on
  size(1280, 768, P2D);
  orientation(LANDSCAPE);
  imageMode(CENTER);
  textSize(45);
}

void draw() {
    if(cam != null && cam.isStarted())
        image(cam, width/2, height/2, width, height);
    else {
        background(128);
        text("Waiting for camera....touch to activate", 100, height/2);
    }
}

void onCameraPreviewEvent() {
    cam.read();
}

// start/stop camera preview by tapping the screen
void mousePressed() {
    //HACK: Instantiate camera once we are in the sketch itself
    if(cam == null) cam = new KetaiCamera(this, 640, 480, 24);
    if (cam.isStarted()) cam.stop();
    else cam.start();
}
void keyPressed() {
    if(cam == null) return;  
    if (key == CODED) {
        if (keyCode == MENU) {
            if (cam.isFlashEnabled()) cam.disableFlash();
            else cam.enableFlash();
        }
    }
}