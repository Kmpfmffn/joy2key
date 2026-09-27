#include <string.h>
#include <iostream>
#include <vector>

#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <linux/uinput.h>


#define TM_BUTTON_1 304
#define TM_BUTTON_2 305
#define TM_BUTTON_3 306
#define TM_BUTTON_4 307
#define TM_BUTTON_5 308
#define TM_BUTTON_6 309
#define TM_BUTTON_7 310
#define TM_BUTTON_8 311
#define TM_BUTTON_LEFT 312
#define TM_BUTTON_RIGHT 313
#define TM_BUTTON_JOY1 314
#define TM_BUTTON_JOY2 315

#define TM_DPAD_HORIZONTAL 16
#define TM_DPAD_VERTICAL 17
#define TM_JOY1_HORIZONTAL 0
#define TM_JOY1_VERTICAL 1
#define TM_JOY2_HORIZONTAL 5
#define TM_JOY2_VERTICAL 6


#define MOUSE_STEP 5
#define SCROLL_STEP 10


bool isButtonPressed(input_event event, unsigned short keycode) {
    if (event.code == keycode && event.value == 1) {
        return true;
    }
    return false;
}

bool isButtonReleased(input_event event, unsigned short keycode) {
    if (event.code == keycode && event.value == 0) {
        return true;
    }
    return false;
}

bool isAxisPos(input_event event, unsigned short keycode) {
    if (event.code == keycode && event.value > 0) {
        return true;
    }
    return false;
}

bool isAxisNeg(input_event event, unsigned short keycode) {
    if (event.code == keycode && event.value < 0) {
        return true;
    }
    return false;
}

bool isAxisZero(input_event event, unsigned short keycode) {
    if (event.code == keycode && event.value == 0) {
        return true;
    }
    return false;
}

bool isAxisNonZero(input_event event, unsigned short keycode) {
    if (event.code == keycode && event.value != 0) {
        return true;
    }
    return false;
}



// Send a key input to the kernel
void sendInput(int fd, int type, int code, int val) {
    struct input_event ie;
    ie.type = type;
    ie.code = code;
    ie.value = val;
    ie.time.tv_sec = 0;
    ie.time.tv_usec = 0;
    write(fd, &ie, sizeof(ie));
}

// Simulate the press of a key
void simKeyPress(int fd, int code) {
    sendInput(fd, EV_KEY, code, 1);
    sendInput(fd, EV_SYN, SYN_REPORT, 0);
}

// Simulate the release of a key
void simKeyRelease(int fd, int code) {
    sendInput(fd, EV_KEY, code, 0);
    sendInput(fd, EV_SYN, SYN_REPORT, 0);
}

// Simulate the press and release of a key
void simKeyClick(int fd, int code) {
    simKeyPress(fd, code);
    simKeyRelease(fd, code);
}

// Simulate horizontal mouse movement
void moveMouseH(int fd, int val) {
    sendInput(fd, EV_REL, REL_X, val);
    sendInput(fd, EV_SYN, SYN_REPORT, 0);
}

// Simulate vertical mouse movement
void moveMouseV(int fd, int val) {
    sendInput(fd, EV_REL, REL_Y, val);
    sendInput(fd, EV_SYN, SYN_REPORT, 0);
}

// Wrapper function for mouse movement
void simMoveMouse(int fd, int valH, int valV) {
    if (valH != 0) {
        moveMouseH(fd, valH);
    }
    if (valV != 0) {
        moveMouseV(fd, valV);
    }
}

// Simulate vertical mouse scrolling
void simScroll(int fd, int val) {
    sendInput(fd, EV_REL, REL_WHEEL_HI_RES, val);
    sendInput(fd, EV_SYN, SYN_REPORT, 0);
}

// Simulate horizontal mouse scrolling
void simScrollH(int fd, int val) {
    sendInput(fd, EV_REL, REL_HWHEEL_HI_RES, val);
    sendInput(fd, EV_SYN, SYN_REPORT, 0);
}


class Mouse {
  public:
    Mouse() : m_MouseSpeedH(0), m_MouseSpeedV(0) {
    }
    void move(int fd) {
        simMoveMouse(fd, m_MouseSpeedH, m_MouseSpeedV);
    }
    void scroll(int fd) {
        simScroll(fd, m_ScrollSpeed);
        simScrollH(fd, m_ScrollSpeedH);
    }
    void setMouseSpeedH(int m) { m_MouseSpeedH = m; }
    void setMouseSpeedV(int m) { m_MouseSpeedV = m; }
    void setScrollSpeed(int m) { m_ScrollSpeed = m; }
    void setScrollSpeedH(int m) { m_ScrollSpeedH = m; }
  private:
    int m_MouseSpeedH;
    int m_MouseSpeedV;
    int m_ScrollSpeed;
    int m_ScrollSpeedH;
};

enum InputType {
    KEY = 0,
    MOUSE
};
enum KeyAction {
    PRESS = 0,
    HOLD,
    RELEASE
};

// A key or mouse button
struct Key {
    int m_Keycode;
    InputType m_Type;
    KeyAction m_Action;
};

// Corresponding to a button on the Gamepad
class Button {
  public:
    Button(unsigned short code, std::string name) : m_Code(code), m_Name(name) { }
    void addKey(int keycode, InputType type, KeyAction action) {
        m_Keys.push_back(Key(keycode, type, action));
    }
  private:
    unsigned short m_Code;
    std::string m_Name;
    std::vector<Key> m_Keys;
};

class Gamepad {
  public:
    //void addButton(std::string name, int keycode) { m_Buttons[name] = keycode; }
    void newButton(unsigned short code, std::string name) {
        m_Buttons.push_back(Button(code, name));
    }
    void addKeyToButton(unsigned short code, Key key) {
        
    }
    void addKeyToButton(std::string name, Key key) {

    }
  private:
    std::vector<Button> m_Buttons;
    //std::unordered_map<std::string, int> m_Buttons; // stores custom names for gamepad buttons and the corresponding keycodes
};
/*
workflow:
    load json file -> open joy -> open uinput -> register ioctl input-codes -> ioctl register device -> main loop
    btnCode -> custom name -> keycode -> action(press, hold, release)
*/

int main() {
    Gamepad gamepad;
    // following will be executed from loaded config file
    gamepad.newButton(304, "TM_BUTTON_1");
    gamepad.addKeyToButton("TM_BUTTON_1", Key(KEY_SPACE, InputType::KEY, KeyAction::PRESS));

    gamepad.newButton(305, "TM_BUTTON_2");
    gamepad.newButton(306, "TM_BUTTON_3");
    gamepad.newButton(307, "TM_BUTTON_4");
    gamepad.newButton(308, "TM_BUTTON_5");
    gamepad.newButton(309, "TM_BUTTON_6");
    gamepad.newButton(310, "TM_BUTTON_7");
    gamepad.newButton(311, "TM_BUTTON_8");
    gamepad.newButton(312, "TM_BUTTON_LEFT");
    gamepad.newButton(313, "TM_BUTTON_RIGHT");
    gamepad.newButton(314, "TM_BUTTON_JOY1");
    gamepad.newButton(315, "TM_BUTTON_JOY2");
    
    
    
    
    
    // open joystick
    std::string joystickPath = "/dev/input/event2";
    int joystick = open(joystickPath.c_str(), O_RDONLY | O_NONBLOCK);
    if (joystick == -1){
        std::cerr << "Failed to open joystick" << std::endl;
        return -1;
    }
    std::cout << "Listening to input from joystick " << joystick << " at " << joystickPath << std::endl;

    
    struct uinput_setup usetup;

    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0) {
        std::cerr << "Unable to open uinput" << std::endl;
        return -1;
    }
    /* necessary keys
    mouse movements [done]
    scrolling       [done]
    mouse buttons   [done]
    space           [done]
    volume+         [done]
    volume-         [done]
    play/pause      [done]
    next            [done]
    prev            [done]
    alt-tab stuff   [done]
    */
    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_EVBIT, EV_SYN);
    ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);
    ioctl(fd, UI_SET_KEYBIT, BTN_RIGHT);
    ioctl(fd, UI_SET_KEYBIT, BTN_MIDDLE);
    ioctl(fd, UI_SET_KEYBIT, KEY_SPACE);
    ioctl(fd, UI_SET_KEYBIT, KEY_VOLUMEUP);
    ioctl(fd, UI_SET_KEYBIT, KEY_VOLUMEDOWN);
    ioctl(fd, UI_SET_KEYBIT, KEY_PLAYPAUSE);
    ioctl(fd, UI_SET_KEYBIT, KEY_NEXTSONG);
    ioctl(fd, UI_SET_KEYBIT, KEY_PREVIOUSSONG);
    ioctl(fd, UI_SET_KEYBIT, KEY_LEFTALT);
    ioctl(fd, UI_SET_KEYBIT, KEY_LEFTSHIFT);
    ioctl(fd, UI_SET_KEYBIT, KEY_TAB);

    ioctl(fd, UI_SET_EVBIT, EV_REL);
    ioctl(fd, UI_SET_RELBIT, REL_X);
    ioctl(fd, UI_SET_RELBIT, REL_Y);
    ioctl(fd, UI_SET_RELBIT, REL_WHEEL_HI_RES);
    ioctl(fd, UI_SET_RELBIT, REL_HWHEEL_HI_RES);


    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor = 0x1234;
    usetup.id.product = 0x5678;
    strcpy(usetup.name, "Joy2key virtual device");

    ioctl(fd, UI_DEV_SETUP, &usetup);
    ioctl(fd, UI_DEV_CREATE);
    sleep(1);

    Mouse mouse;

    struct input_event events[8];
    while (true) {
        mouse.move(fd);
        mouse.scroll(fd);
        int bytes = read(joystick, events, sizeof(events));
        if (bytes > 0) {
            for (int i = 0; i < bytes/sizeof(input_event); i++) {
                auto event = events[i];

                if (event.type == EV_KEY) {
                    //std::cout << "Button " << events[i].code << " pressed" << std::endl;
                    if (false) {
                    }
                    else if (isButtonPressed(event, TM_BUTTON_1)) {
                        simKeyClick(fd, KEY_SPACE); // Space bar
                    }
                    else if (isButtonPressed(event, TM_BUTTON_2)) {
                        simKeyPress(fd, BTN_LEFT); // Left mouse button press
                    }
                    else if (isButtonReleased(event, TM_BUTTON_2)) {
                        simKeyRelease(fd, BTN_LEFT); // Left mouse button release
                    }
                    else if (isButtonPressed(event, TM_BUTTON_3)) {
                        simKeyClick(fd, BTN_RIGHT); // Right mouse button
                    }
                    else if (isButtonPressed(event, TM_BUTTON_4)) {
                        simKeyClick(fd, BTN_MIDDLE); // Middle mouse button
                    }
                    else if (isButtonPressed(event, TM_BUTTON_5)) {
                        simKeyPress(fd, KEY_LEFTALT); // Alt+Tab only one to the left
                        simKeyPress(fd, KEY_LEFTSHIFT);
                        simKeyClick(fd, KEY_TAB);
                        simKeyRelease(fd, KEY_LEFTSHIFT);
                        simKeyRelease(fd, KEY_LEFTALT);
                    }
                    else if (isButtonPressed(event, TM_BUTTON_6)) {
                        
                    }
                    else if (isButtonPressed(event, TM_BUTTON_7)) {
                        simKeyPress(fd, KEY_LEFTALT); // Alt+Tab only one to the right
                        simKeyClick(fd, KEY_TAB);
                        simKeyRelease(fd, KEY_LEFTALT);
                    }
                    else if (isButtonPressed(event, TM_BUTTON_8)) {
                        simKeyClick(fd, KEY_PLAYPAUSE); // Play/Pause
                    }
                    else if (isButtonPressed(event, TM_BUTTON_LEFT)) {
                        simKeyPress(fd, KEY_LEFTALT); // Keep Alt+Tab open and move to the right
                        simKeyClick(fd, KEY_TAB);
                    }
                    else if (isButtonPressed(event, TM_BUTTON_RIGHT)) {
                        simKeyRelease(fd, KEY_LEFTALT); // Release Alt+Tab menu
                    }
                } // if key event

                else if (event.type == EV_ABS) {
                    //std::cout << "Axis " << events[i].code << " state: " << events[i].value << std::endl;
                    if (false) {
                    }
                    else if (isAxisNeg(event, TM_DPAD_HORIZONTAL)) { // D-Pad left
                        simKeyClick(fd, KEY_PREVIOUSSONG);
                    }
                    else if (isAxisPos(event, TM_DPAD_HORIZONTAL)) { // D-Pad right
                        simKeyClick(fd, KEY_NEXTSONG);
                    }
                    else if (isAxisNeg(event, TM_DPAD_VERTICAL)) { // D-Pad up
                        simKeyClick(fd, KEY_VOLUMEUP);
                    }
                    else if (isAxisPos(event, TM_DPAD_VERTICAL)) { // D-Pad down
                        simKeyClick(fd, KEY_VOLUMEDOWN);
                    }
                    
                    else if (isAxisZero(event, TM_JOY1_HORIZONTAL)) { // Joy1 horizontally neutral
                        mouse.setScrollSpeedH(0);
                    }
                    else if (isAxisNonZero(event, TM_JOY1_HORIZONTAL)) {
                        mouse.setScrollSpeedH(event.value/12);
                    }

                    else if (isAxisZero(event, TM_JOY1_VERTICAL)) { // Joy1 vertically neutral
                        mouse.setScrollSpeed(0);
                    }
                    else if (isAxisNonZero(event, TM_JOY1_VERTICAL)) {
                        mouse.setScrollSpeed(event.value/12);
                    }

                    else if (isAxisZero(event, TM_JOY2_HORIZONTAL)) { // Joy2 horizontally neutral
                        mouse.setMouseSpeedH(0);
                    }
                    else if (isAxisNonZero(event, TM_JOY2_HORIZONTAL)) {
                        mouse.setMouseSpeedH(event.value/18);
                    }

                    else if (isAxisPos(event, TM_JOY2_VERTICAL)) { // Joy2 vertical
                        mouse.setMouseSpeedV((event.value-128)/12);
                    }
                } // if axis event
            }
        }
        usleep(10000);
    }
    ioctl(fd, UI_DEV_DESTROY);
    close(fd);
    close(joystick);
    return 0;
}