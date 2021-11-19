/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       henryzhao                                                 */
/*    Created:      Thu Jan 14 2021                                           */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// Optical11            optical       11              
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"

using namespace vex;

void mainLoop() {
  Brain.Screen.clearScreen();
  Brain.Screen.setCursor( 10, 10);
  Brain.Screen.print( Optical11.hue() );

  vex::task::sleep(10);

  mainLoop();
}

int main() {
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();
  
  mainLoop();
}
