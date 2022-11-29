#include <nahs-Bricks-Feature-Signal.h>
#include <nahs-Bricks-Lib-SerHelp.h>

NahsBricksFeatureSignal::NahsBricksFeatureSignal() {
}

/*
Returns name of feature
*/
String NahsBricksFeatureSignal::getName() {
    return "signal";
}

/*
Returns version of feature
*/
uint16_t NahsBricksFeatureSignal::getVersion() {
    return version;
}

/*
Configures FSmem und RTCmem variables (prepares feature to be fully operational)
*/
void NahsBricksFeatureSignal::begin() {
    if (!FSdata.containsKey("iState")) {
        FSdata.createNestedArray("iState");  // array with initial states of signals
        for (uint8_t i = 0; i < _expandersPinsCount; ++i) FSdata["iState"].add(0);
    }
    if(!RTCmem.isValid()) {
        RTCdata->signalCountRequested = false;
        _initSignals();
    }
}

/*
Starts background processes like fetching data from other components
*/
void NahsBricksFeatureSignal::start() {
}

/*
Adds data to outgoing json, that is send to BrickServer
*/
void NahsBricksFeatureSignal::deliver(JsonDocument* out_json) {
    // check if signal count is requested
    if (RTCdata->signalCountRequested) {
        RTCdata->signalCountRequested = false;
        out_json->getOrAddMember("s").set(_expandersPinsCount);
    }
}

/*
Processes feedback coming from BrickServer
*/
void NahsBricksFeatureSignal::feedback(JsonDocument* in_json) {
    // evaluate requests
    if (in_json->containsKey("r")) {
        for (JsonVariant value : in_json->getMember("r").as<JsonArray>()) {
            switch(value.as<uint8_t>()) {
                case 7:
                    RTCdata->signalCountRequested = true;
                    break;
            }
        }
    }

    //set new signal states if delivered
    if (in_json->containsKey("o")) {
        uint8_t i = 0;
        for (JsonVariant state : in_json->getMember("o").as<JsonArray>()) {
            if (i < _expandersPinsCount) {
                _writeCacheAdd(_expanders[i], _expandersPins[i], state.as<uint8_t>());
            } else break;
            ++i;
        }
        _writeCacheFlush();
    }
}

/*
Finalizes feature (closes stuff)
*/
void NahsBricksFeatureSignal::end() {
}

/*
Prints the features RTCdata in a formatted way to Serial (used for brickSetup)
*/
void NahsBricksFeatureSignal::printRTCdata() {
    Serial.print("  signalCountRequested: ");
    SerHelp.printlnBool(RTCdata->signalCountRequested);
}

/*
Prints the features FSdata in a formatted way to Serial (used for brickSetup)
*/
void NahsBricksFeatureSignal::printFSdata() {
    Serial.println("  Initial States: ");
    for (uint8_t i = 0; i < _expandersPinsCount; ++i) {
        Serial.print("    ");
        Serial.print(i);
        Serial.print(": ");
        if (FSdata["iState"][i] == 0) Serial.println("off");
        else Serial.println("on");
    }
}

/*
BrickSetup hands over to this function, when features-submenu is selected
*/
void NahsBricksFeatureSignal::brickSetupHandover() {
    _printMenu();
    while (true) {
        Serial.println();
        Serial.print("Select: ");
        uint8_t input = SerHelp.readLine().toInt();
        switch(input) {
            case 1:
                _setSignalDefault();
                break;
            case 2:
                _blinkSignal();
                break;
            case 9:
                Serial.println("Returning to MainMenu!");
                return;
                break;
            default:
                Serial.println("Invalid input!");
                _printMenu();
                break;
        }
    }
}

/*
Brick-Specific helper to assign the ExpanderPins (or LatchExpanderPins) to be used by feature
The usage of multiple Expanders (or LatchExpanders or a mix of both) is also possible
*/
void NahsBricksFeatureSignal::assignExpanderPin(NahsBricksLibCoIC_Expander &expander, uint8_t pinIndex) {
    if (_expandersPinsCount < 16 && pinIndex < expander.expanderCount()) {
        _expanders[_expandersPinsCount] = &expander;
        _expandersPins[_expandersPinsCount] = pinIndex;
        ++_expandersPinsCount;
    }
}

/*
Internal helper to set used ExpanderPins as Output and write initial State
*/
void NahsBricksFeatureSignal::_initSignals() {
    for (uint8_t i = 0; i < _expandersPinsCount; ++i) {
        _expanders[i]->setOutput(_expandersPins[i]);
        _writeCacheAdd(_expanders[i], _expandersPins[i], FSdata["iState"][i].as<uint8_t>());
    }
    _writeCacheFlush();
}

/*
Internal helper to cache write statements to Expanders, this is to reduce write-operations and make Expanders less errorprone
*/
void NahsBricksFeatureSignal::_writeCacheAdd(NahsBricksLibCoIC_Expander *expander, uint8_t pinIndex, uint8_t state) {
    if (pinIndex > 7) return;
    int8_t cacheIndex = -1;
    for (int8_t i = 0; i < _writeCacheCount; ++i) {
        if (_writeCacheExpanders[i]->getAddr() == expander->getAddr()) {
            cacheIndex = i;
            break;
        }
    }
    if (cacheIndex == -1) {
        if (_writeCacheCount >= MAX_WRITE_CACHES) return;
        cacheIndex = _writeCacheCount;
        _writeCacheExpanders[cacheIndex] = expander;
        _writeCache[cacheIndex] = 0;
        ++_writeCacheCount;
    }
    if (state > 0) {
        _writeCache[cacheIndex] |= (1<<pinIndex);
    }
}

/*
Internal helper to do the actual wirtes to Expanders, the should result in just one write operation per Expander
*/
void NahsBricksFeatureSignal::_writeCacheFlush() {
    for (uint8_t i = 0; i < _writeCacheCount; ++i) {
        _writeCacheExpanders[i]->writeOutputs(_writeCache[i]);
    }
}

/*
Helper to print Feature submenu during BrickSetup
*/
void NahsBricksFeatureSignal::_printMenu() {
    Serial.println("1) Set Signal default");
    Serial.println("2) Blink Signal");
    Serial.println("9) Return to MainMenu");
}

/*
BrickSetup function to set the default state of Signal
*/
void NahsBricksFeatureSignal::_setSignalDefault() {
    Serial.print("Enter ID of Signal: ");
    uint8_t signal = SerHelp.readLine().toInt();
    if (signal >= _expandersPinsCount) {
        Serial.println("Invalid Signal ID!");
        return;
    }
    Serial.print("Enter state (0/1): ");
    uint8_t state = SerHelp.readLine().toInt();
    if (state > 1) {
        Serial.println("Invalid state!");
        return;
    }
    FSdata["iState"][signal] = state;
    _initSignals();
    Serial.print("Set default of Signal ");
    Serial.print(signal);
    Serial.print(" to ");
    Serial.println(state);
}

/*
BrickSetup function to blink (test) a Signal pin
*/
void NahsBricksFeatureSignal::_blinkSignal() {
    Serial.print("Enter ID of Signal: ");
    uint8_t signal = SerHelp.readLine().toInt();
    if (signal >= _expandersPinsCount) {
        Serial.println("Invalid Signal ID!");
        return;
    }
    for (uint8_t i = 0; i < 4; ++i) {
        delay(500);
        _expanders[signal]->toggleOutput(_expandersPins[signal]);
        Serial.print('.');
    }
    Serial.println(" done");
}


//------------------------------------------
// globally predefined variable
#if !defined(NO_GLOBAL_INSTANCES)
NahsBricksFeatureSignal FeatureSignal;
#endif
