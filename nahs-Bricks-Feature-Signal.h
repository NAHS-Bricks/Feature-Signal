#ifndef NAHS_BRICKS_FEATURE_SIGNAL_H
#define NAHS_BRICKS_FEATURE_SIGNAL_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <nahs-Bricks-Lib-CoIC.h>
#include <nahs-Bricks-Feature-BaseClass.h>
#include <nahs-Bricks-Lib-RTCmem.h>
#include <nahs-Bricks-Lib-FSmem.h>

class NahsBricksFeatureSignal : public NahsBricksFeatureBaseClass {
    private:  // Variables
        static const uint16_t version = 1;
        typedef struct {
            bool signalCountRequested;
        } _RTCdata;
        _RTCdata* RTCdata = RTCmem.registerData<_RTCdata>();
        JsonObject FSdata = FSmem.registerData("i");
        NahsBricksLibCoIC_Expander* _expanders[16];
        uint8_t _expandersPins[16];
        uint8_t _expandersPinsCount = 0;
        static const uint8_t MAX_WRITE_CACHES = 6;
        NahsBricksLibCoIC_Expander* _writeCacheExpanders[MAX_WRITE_CACHES];
        uint8_t _writeCache[MAX_WRITE_CACHES];
        uint8_t _writeCacheCount = 0;

    public: // BaseClass implementations
        NahsBricksFeatureSignal();
        String getName();
        uint16_t getVersion();
        void begin();
        void start();
        void deliver(JsonDocument* out_json);
        void feedback(JsonDocument* in_json);
        void end();
        void printRTCdata();
        void printFSdata();
        void brickSetupHandover();

    public:  // Brick-Specific setter
        void assignExpanderPin(NahsBricksLibCoIC_Expander &expander, uint8_t pinIndex);

    private:  // internal Helpers
        void _initSignals();
        void _writeCacheAdd(NahsBricksLibCoIC_Expander *expander, uint8_t pinIndex, uint8_t state);
        void _writeCacheFlush();

    private:  // BrickSetup Helpers
        void _printMenu();
        void _setSignalDefault();
        void _blinkSignal();
};

#if !defined(NO_GLOBAL_INSTANCES)
extern NahsBricksFeatureSignal FeatureSignal;
#endif

#endif // NAHS_BRICKS_FEATURE_SIGNAL_H
