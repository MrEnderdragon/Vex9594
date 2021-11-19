/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       henryzhao                                                 */
/*    Created:      Tue Sep 21 2021                                           */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// MotorName            motor         1               
// Controller1          controller                    
// Motor1               motor         2               
// Motor2               motor         3               
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"

using namespace vex;

#define pi 3.14159265358979323846

float diag = 25; //diagonal distance from wheel to wheel (diameter)
float wheelDia = 10.5; //diameter of a wheel

float turnCirc = diag * pi; //circumference of 360 turn
float turnPerDeg = turnCirc / 360; //distance per one degree of turn
float wheelCirc = wheelDia * pi; //circumference of wheel
float distPerDeg = wheelCirc / 360; //distance wheel drives per degree
float degsPerDeg = turnPerDeg / distPerDeg; //degrees wheel needs to rotate for 1 degree turn of robot


void driveCent(int speed, float cents){
  //drive the robot forward with speed for <cent> centimeters
  Motor1.rotateFor(cents / distPerDeg, degrees, speed, rpm, false);
  Motor2.rotateFor(cents / distPerDeg, degrees, speed, rpm);
}

void turnDegs(int speed, float degs){
  //turns the robot clockwise with speed for <degs> degrees
  Motor1.rotateFor(degs * degsPerDeg, degrees ,speed, rpm, false);
  Motor2.rotateFor(-degs * degsPerDeg, degrees, speed, rpm);
}

int main() {
 vexcodeInit();

  // while(true){
  //   int axis3Percent = Controller1.Axis3.position(percent);
  //   int axis2Percent = Controller1.Axis3.position(percent);

  //   if(axis3Percent > 10 || axis3Percent < -10){
  //     Motor1.spin(forward, axis3Percent * 2, rpm);
  //   }else {
  //     Motor1.stop();
  //   }

  //   if(axis2Percent > 10 || axis2Percent < -10){
  //     Motor2.spin(forward, axis2Percent * 2, rpm);
  //   }else {
  //     Motor2.stop();
  //   }
  // }

//   while(true){
//    if(Controller1.ButtonA.pressing()){
//      Motor1.spin(forward, 100, rpm);
//    }else {
//      Motor1.stop();
//    }
//  }

while(true){
    int axis3Percent = Controller1.Axis3.position(percent);
    if(axis3Percent > 10 || axis3Percent < -10){
      Motor1.spin(forward, axis3Percent * 2, rpm);
    }else {
      Motor1.stop();
    }
  }

}
