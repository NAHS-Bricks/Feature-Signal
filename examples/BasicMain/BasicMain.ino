#include <Arduino.h>
#include <nahs-Bricks-OS.h>
// include all features of brick
#include <nahs-Bricks-Feature-Signal.h>

void setup() {
  // Now register all the features under All
  // Note: the order of registration is the same as the features are handled internally by FeatureAll
  FeatureAll.registerFeature(&FeatureSignal);

  // Set Brick-Specific stuff
  BricksOS.setSetupPin(D5);
  FeatureAll.setBrickType(3);

  // Set Brick-Specific (feature related) stuff
  // For this Feature you have to start the Wire connection, connect the Expander (or LatchExpander)
  //   on it's address and assign the one to be used to the Feature (with the corresponding pins to be used)
  Wire.begin();
  Expander.begin(45);
  FeatureSignal.assignExpanderPin(Expander, 0);
  FeatureSignal.assignExpanderPin(Expander, 1);
  FeatureSignal.assignExpanderPin(Expander, 2);

  // Finally hand over to BrickOS
  BricksOS.handover();
}

void loop() {
  // Not used on Bricks
}