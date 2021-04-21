#include "M5Atom.h"
#include "WT2003S_Player.h"
#include "main.h"

int MUTE_BUTTON_PRESSED = 0;

const int MUTE = GPIO_NUM_22;
const int INA = GPIO_NUM_21;
const int INB = GPIO_NUM_25;

boolean isFirstCycle = true;
const int freq = 30000;
const int motorChannel = 0;
const int resolution = 8;

#ifdef ARDUINO_M5Stack_ATOM
    #define COMSerial Serial1
    #define ShowSerial Serial

    WT2003S<HardwareSerial> Mp3Player;
#endif

void setup() {
    M5.begin(true, false, false);
    initToys();
    initMusicPlayer();
    while (!ShowSerial);
    ShowSerial.begin(115200);
}

void loop() {
    keepPlayingSongs();
    spinToys(isFirstCycle ? 120 : 25);
    MUTE_BUTTON_PRESSED = playOrPause(MUTE_BUTTON_PRESSED);
    delay(isFirstCycle ? 500 : 0);
    M5.update();
    isFirstCycle = false;
}

void initToys() {
    ledcSetup(motorChannel, freq, resolution);
    ledcAttachPin(INA, motorChannel);
    pinMode(INB, OUTPUT);
}

void initMusicPlayer() {
    COMSerial.begin(9600, SERIAL_8N1, 32, 26);
    Mp3Player.init(COMSerial);
    Mp3Player.playMode(RANDOM);
    Mp3Player.stop();
    Mp3Player.pause_or_play();
    Mp3Player.volume(5);
}

void spinToys(int duty) {
    ledcWrite(motorChannel, duty);
    digitalWrite(INB, LOW);
}

int playOrPause(int wasButtonPressed) {
    int muteBtnPressed = digitalRead(MUTE);
    if (M5.Btn.wasPressed() || (muteBtnPressed && wasButtonPressed == 0)) {
        Mp3Player.pause_or_play();
    }
    return muteBtnPressed;
}

void keepPlayingSongs() {
    uint8_t status;
    status = Mp3Player.getStatus();
    if (status == 0x02) {
        Mp3Player.next();
    }
}
