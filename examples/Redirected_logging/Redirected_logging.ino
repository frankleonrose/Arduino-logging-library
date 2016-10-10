#include <Logging.h>

/**
* This example shows how to use your own print function.
*/

#define LOGLEVEL LOG_LEVEL_DEBUG

// This is our print function that outputs a single character.
// You could do other things in here, like print output to two or more
// destinations. For instance, this capability was developed to send log
// messages over BLE to a listening iPhone app.
void conditionalLog(const char c) {
    // If you disconnect a USB cable, Serial becomes inoperative and on some
    // boards will hangs. In order to freely disconnect a running board,
    // we check whether Serial is active before trying to print.
    if (Serial) { // NOTE: This check is sloooooow because of delay here https://github.com/arduino/Arduino/blob/2bfe164b9a5835e8cb6e194b928538a9093be333/hardware/arduino/sam/cores/arduino/USB/CDC.cpp#L314
      Serial.print(c);
    }
}

LogPrinter safePrinter(conditionalLog);

void setup() {
    // Since we're telling the Log library to call us back
    // when it wants to print, we need to take care of initializing Serial.
    Serial.begin(38400L);

    // Now we initialize the Log library with our own printer...
    Log.Init(LOGLEVEL, safePrinter);

    // And everything works normally...
    Log.Info("Setup complete."CR);
}

int counter = 0;
bool toggle = false;

void loop() {
    Log.Info("Loop counter: %d"CR, counter++);
    Log.Error("This is an error message? %T"CR, toggle = !toggle);
    Log.Debug("This is debug info: %x, %t"CR, counter, toggle);
    delay(5000);
}
