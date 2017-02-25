// import library to access android sensors
import ketai.camera.*;

KetaiCamera cam;
void setup() {
  // choose P2D renderer

  // 1280x768 is the resolution my genymotion simulation runs on
  size(1280, 768, P2D);
  orientation(LANDSCAPE);
  noStroke();
  imageMode(CENTER);
  textSize(45);
}

float indicator = 0;
int time = millis();

void draw() {
    if(cam != null && cam.isStarted()) {
        //image(cam, width/2, height/2, width, height);
        background(0);
        fill(255);
        text("Screenshot every 2 seconds", 100, height / 2);
        
        // action every 2 seconds
        if (millis() > time + 2000) {
            saveImage();
            time = millis();
        }

        fill(255, 0, 0, indicator);
        indicator = indicator <= 2 ? 0 : indicator * 0.8;
        ellipse(50, 50, 20, 20);
    }

    else {
        background(0);
        fill(255);
        text("Waiting for camera....touch to activate", 100, height/2);
    }
}

void onCameraPreviewEvent() {
    cam.read();
}

// start/stop camera preview by tapping the screen
void mousePressed() {
    //HACK: Instantiate camera once we are in the sketch itself
    if(cam == null) cam = new KetaiCamera(this, width, height, 24);
    if (cam.isStarted()) cam.stop();
    else cam.start();
}
void keyPressed() {
    if(cam == null) return;
    if (key == CODED) {
        if (keyCode == MENU) {
        }
    }
}

void saveImage() {
  cam.save("/sdcard/DCIM/Camera/camera2_" + str(millis() / 1000) + ".jpg");
  indicator = 255;
}