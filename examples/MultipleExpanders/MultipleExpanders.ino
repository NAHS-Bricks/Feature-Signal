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
  FeatureAll.setBrickType(4);

  // Set Brick-Specific (feature related) stuff
  // For this Feature you have to start the Wire connection, connect the Expander (or LatchExpander)
  //   on it's address and assign the one to be used to the Feature (with the corresponding pins to be used)
  Wire.begin();
  LatchExpander.begin(45);
  FeatureSignal.assignExpanderPin(LatchExpander, 1);
  FeatureSignal.assignExpanderPin(LatchExpander, 2);
  FeatureSignal.assignExpanderPin(LatchExpander, 3);

  // In this example we are using one additional Expander (but it could even be more)
  // Combined these are providing six Signals
  NahsBricksLibCoIC_Expander* BreakoutExpander = new NahsBricksLibCoIC_Expander();
  BreakoutExpander->begin(46);
  FeatureSignal.assignExpanderPin(*BreakoutExpander, 0);
  FeatureSignal.assignExpanderPin(*BreakoutExpander, 1);
  FeatureSignal.assignExpanderPin(*BreakoutExpander, 2);
  
  // Finally hand over to BrickOS
  BricksOS.handover();
}

void loop() {
  // Not used on Bricks
}