# processing

## Arduino
```c
int led = 13;
int incomingByte = 0;

void setup() {
    pinMode(led, OUTPUT);

    //Setup Serial Port with baud rate of 9600
    Serial.begin(9600);
    Serial.println("Press H to turn LED ON");
    Serial.println("Press L to turn LED OFF");
}

void loop() {
    if (Serial.available() > 0) {
        // read the incoming byte:
        incomingByte = Serial.read();

        if(incomingByte == 'H') {
            digitalWrite(led, HIGH);
            Serial.println("LED ON");
        } else if(incomingByte == 'L') {
            digitalWrite(led, LOW);
            Serial.println("LED OFF");
        } else {
            Serial.println("invalid!");
        }

    }
}
```

## Processing code
```c
import processing.serial.*;

Serial myArduinoPort;

int buttonA_x = 300;
int buttonA_y = 50;
int buttonA_width = 50;
int buttonA_height = 50;
int buttonA_colorR = 0xff;
int buttonA_colorG = 0x00;
int buttonA_colorB = 0x00;

int buttonB_x = 300;
int buttonB_y = 150;
int buttonB_width = 50;
int buttonB_height = 50;
int buttonB_r = 10;
int buttonB_color = 200;

int defaultColor = 150;

boolean buttonA_clicked = false;
boolean buttonB_clicked = false;

void setup() {  // setup() runs once
    size(400, 300);
    background(255);

    //Get serial port for Arduino
    String arduinoPort = Serial.list()[0];
    println(arduinoPort);
    myArduinoPort = new Serial(this, arduinoPort, 9600);

}

void draw() {

    if(buttonA_clicked) {
        fill(buttonA_colorR, buttonA_colorG, buttonA_colorB);
    } else if(buttonB_clicked) {
        fill(buttonB_color);
    } else {
        fill(defaultColor);
    }
    ellipse(width/2, height/2, 100, 100);

    fill(buttonA_colorR, buttonA_colorG, buttonA_colorB);
    rect(buttonA_x, buttonA_y, buttonA_width, buttonA_height);

    fill(buttonB_color);
    rect(buttonB_x, buttonB_y, buttonB_width, buttonB_height, buttonB_r);
}

/* The mouseClicked() function is called once after a mouse button
 * has been pressed and then released.
 */
void mouseClicked() {
    // mouseX = x of mouse click position
    // mouseY = y of mouse click position

    if (mouseX >= buttonA_x && mouseX <= buttonA_x + buttonA_width &&
            mouseY >= buttonA_y && mouseY <= buttonA_y + buttonA_height) {
        buttonA_clicked = true;

        myArduinoPort.write("H");
    } else {
        buttonA_clicked = false;
    }

    if (mouseX >= buttonB_x && mouseX <= buttonB_x + buttonB_width &&
            mouseY >= buttonB_y && mouseY <= buttonB_y + buttonB_height) {
        buttonB_clicked = true;

        myArduinoPort.write("L");
    } else {
        buttonB_clicked = false;
    }
}
```



