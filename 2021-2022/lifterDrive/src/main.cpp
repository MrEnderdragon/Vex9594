/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       henryzhao                                                 */
/*    Created:      Fri Aug 28 2020                                           */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// Controller1          controller                    
// DriveBL              motor         1               
// DriveBR              motor         2               
// DriveFL              motor         3               
// DriveFR              motor         4               
// midL                 motor         6               
// midR                 motor         7               
// backArm              motor         9               
// smallSwitch          limit         A               
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"
#include <cmath>

struct asyncParams {
  int timeDelay;
  motor *toStop;
};

using namespace vex;
competition Competition;

# define mPi 3.14159265358979323846
# define tile 23.5
# define alignTile 3

// ---- USER INPUT VARIABLES ----
int j = 1; //time per blink
int m = 60; // time per meow
int driveThreshold = 10;
int turnThreshold = 10;
double moveStuckThresh = 0.01; // in degrees
double moveStuckReps = 50;
int moveAcc = 5; // in millisecs per rep
int revTime = 300;

float slowMult = 0.8;
float normMult = 1.9;

int revAm = 1;

double wheelDiam = 2.5; //inches
double chassisWidth = 18.5; //inches

int abortTime = 10000;

// ---- PROGRAM VARIABLES ----

float multiplier = normMult*revAm;

bool blinked = false; //whether is blinking
bool opened = false; //whether is meowing

bool slowmode = false;
bool revmode = true;
bool convMode = false;
bool aPressed = false;
bool bPressed = false;
bool xPressed = false;
// int isBlue = -1;
int isBlue = 0;
int returnState = -1;
long cycles = 0;

int revCounter = 0;

// ---- FORWARD DRIVE MATH ----

double wheelCirc = wheelDiam * mPi;
double distPerWheelDeg =  wheelCirc / 360;

// ---- TURN DRIVE MATH ----

double turnCirc = chassisWidth * mPi;
double distPerTurnDeg = turnCirc / 360;
double wheelDegPerTurnDeg = distPerTurnDeg/distPerWheelDeg-1;

void driveDist(int speed, int dist){
  DriveFL.spinFor(dist/distPerWheelDeg, deg,speed, rpm, false);
  DriveBL.spinFor(dist/distPerWheelDeg, deg,speed, rpm, false);
  DriveFR.spinFor(dist/distPerWheelDeg, deg,speed, rpm, false);
  DriveBR.spinFor(dist/distPerWheelDeg, deg,speed, rpm);
}

void turnDegsCW(int speed, int degs){
  DriveFL.spinFor(degs*wheelDegPerTurnDeg, deg, speed, rpm, false);
  DriveBL.spinFor(degs*wheelDegPerTurnDeg, deg, speed, rpm, false);
  DriveFR.spinFor(-degs*wheelDegPerTurnDeg, deg, speed, rpm, false);
  DriveBR.spinFor(-degs*wheelDegPerTurnDeg, deg, speed, rpm);
}

void driveTime (int speed, double amTime){

  amTime *= 1000;

  int start = vex::timer::system();

  DriveBL.spin(forward, speed, rpm);
  DriveBR.spin(forward, speed, rpm);
  DriveFL.spin(forward, speed, rpm);
  DriveFR.spin(forward, speed, rpm);

  while(vex::timer::system() - start <= amTime){
    vex::task::sleep(moveAcc);
  }

  DriveFL.stop();
  DriveFR.stop();
  DriveBL.stop();
  DriveBR.stop();
}

void driveTime2 (int speedL, int speedR, double amTime){

  amTime *= 1000;

  int start = vex::timer::system();

  DriveBL.spin(forward, speedL, rpm);
  DriveBR.spin(forward, speedR, rpm);
  DriveFL.spin(forward, speedL, rpm);
  DriveFR.spin(forward, speedR, rpm);

  while(vex::timer::system() - start <= amTime){
    vex::task::sleep(moveAcc);
  }

  DriveFL.stop();
  DriveFR.stop();
  DriveBL.stop();
  DriveBR.stop();
}

void turnTime (int speed, double amTime){

  amTime *= 1000;

  int start = vex::timer::system();

  DriveBL.spin(forward, speed, rpm);
  DriveBR.spin(forward, -speed, rpm);
  DriveFL.spin(forward, speed, rpm);
  DriveFR.spin(forward, -speed, rpm);

  while(vex::timer::system() - start <= amTime){
    vex::task::sleep(moveAcc);
  }

  DriveFL.stop();
  DriveFR.stop();
  DriveBL.stop();
  DriveBR.stop();
}

int controlAxis (int axis){
  switch(axis){
    case 1: { return Controller1.Axis1.position();}
    case 2: { return Controller1.Axis2.position();}
    case 3: { return Controller1.Axis3.position();}
    case 4: { return Controller1.Axis4.position();}
    default: return 0;
  }
}

bool controlButton (char button, bool top = false){
  switch(button){
    case 'A':{ return Controller1.ButtonA.pressing();}
    case 'B':{ return Controller1.ButtonB.pressing();}
    case 'X':{ return Controller1.ButtonX.pressing();}
    case 'Y':{ return Controller1.ButtonY.pressing();}

    case 'U':{ return Controller1.ButtonUp.pressing();}
    case 'D':{ return Controller1.ButtonDown.pressing();}
    case 'L':{ return Controller1.ButtonLeft.pressing();}
    case 'R':{ return Controller1.ButtonRight.pressing();}

    case 'l':{
      if(top) return Controller1.ButtonL1.pressing();
      else return Controller1.ButtonL2.pressing();
    }
    case 'r':{
      if(top) return Controller1.ButtonR1.pressing();
      else return Controller1.ButtonR2.pressing();
    }

    default:
      return false;
  }
}

void stopDrive(int motorr){
  //stops driving
  switch(motorr){
    case 1: DriveBL.stop(); break;
    case 2: DriveBR.stop(); break;

    case 3: DriveFL.stop(); break;
    case 4: DriveFR.stop(); break;
  }
}

void drawFace (const char * bg, const char * linee) {
  int v1 = rand() % 10;
  int v2 = rand() % 60;
  
  if (v1 == 0 && !blinked){
      
    blinked = true;
    
    Brain.Screen.setPenColor(bg);
    
    Brain.Screen.drawRectangle(1,1,400,75,bg);
    
    for (int i = 0; i < 3; i++){
      Brain.Screen.setPenColor(linee);
      
      Brain.Screen.drawLine(80,50+i,120,50+i);
      Brain.Screen.drawLine(280,50+i,320,50+i);
    }
    
    j = 2;
      
  }else if (blinked){
    if (j <= 0){
      Brain.Screen.setPenColor(bg);
      
      Brain.Screen.drawRectangle(1,1,400,75,bg);
      Brain.Screen.drawCircle(100,50,20,linee);
      Brain.Screen.drawCircle(300,50,20,linee);

      blinked = false;
    }else {
      j--;
    }
  }
  
  if (v2 == 0 && !opened){
      
    opened = true;
    
    Brain.Screen.setPenColor(bg);
    
    Brain.Screen.drawRectangle(130,80,271-130,170-80,bg);
    
    for(int i = 0; i < 10; i ++) {
      Brain.Screen.setPenColor(linee);

      Brain.Screen.drawLine(130,100 + i,165,150 + i);
      Brain.Screen.drawLine(165,150 + i,200,120 + i);
      Brain.Screen.drawLine(200,120 + i,235,150 + i);
      Brain.Screen.drawLine(235,150 + i,270,100 + i);
    }
    Brain.Screen.setPenColor(linee);
    Brain.Screen.printAt(280,125,"- Meow");

    m = 4;
      
  }else if (opened){
    if (m <= 0){
      Brain.Screen.setPenColor(bg);
      
      Brain.Screen.drawRectangle(130,80,271,170,bg);
      for(int i = 0; i < 10; i ++) {
        Brain.Screen.setPenColor(linee);

        Brain.Screen.drawLine(130,100 + i,165,150 + i);
        Brain.Screen.drawLine(165,150 + i,200,120 + i);
        Brain.Screen.drawLine(200,120 + i,235,150 + i);
        Brain.Screen.drawLine(235,150 + i,270,100 + i);
      }
      opened = false;
    }else {
      if (m % 1 == 0 && m > 2){
        Brain.Screen.setPenColor(bg);
        
        Brain.Screen.drawRectangle(130,80,271-130,170-80,bg);
        int n = (4 - m)*3;
        
        for(int i = 0; i < 5; i ++) {
          Brain.Screen.setPenColor(linee);

          Brain.Screen.drawLine(130,100 + i,165,150 + i - n);
          Brain.Screen.drawLine(165,150 + i - n,200,120 + i - ceil(n/2));
          Brain.Screen.drawLine(200,120 + i - ceil(n/2),235,150 + i - n);
          Brain.Screen.drawLine(235,150 + i - n,270,100 + i);
        }
        for(int i = 5; i < 10; i ++) {
          Brain.Screen.setPenColor(linee);

          Brain.Screen.drawLine(130,100 + i,165,150 + i);
          Brain.Screen.drawLine(165,150 + i,200,120 + i);
          Brain.Screen.drawLine(200,120 + i,235,150 + i);
          Brain.Screen.drawLine(235,150 + i,270,100 + i);
        }
      }else if (m % 1 == 0 && m <= 2){
        Brain.Screen.setPenColor(bg);
        
        Brain.Screen.drawRectangle(130,80,271-130,170-80,bg);
        
        int n = m *3;
        
        for(int i = 0; i < 5; i ++) {
          Brain.Screen.setPenColor(linee);

          Brain.Screen.drawLine(130,100 + i,165,150 + i - n);
          Brain.Screen.drawLine(165,150 + i - n,200,120 + i - ceil(n/2));
          Brain.Screen.drawLine(200,120 + i - ceil(n/2),235,150 + i - n);
          Brain.Screen.drawLine(235,150 + i - n,270,100 + i);
        }
        for(int i = 5; i < 10; i ++) {
          Brain.Screen.setPenColor(linee);

          Brain.Screen.drawLine(130,100 + i,165,150 + i);
          Brain.Screen.drawLine(165,150 + i,200,120 + i);
          Brain.Screen.drawLine(200,120 + i,235,150 + i);
          Brain.Screen.drawLine(235,150 + i,270,100 + i);
        }
      }
      m--;
    }
  }
}

void drive(int speed1, int speed2){
  float res1 = fmax(fmin(speed1, 100),-100);
  float res2 = fmax(fmin(speed2, 100),-100);

  if(speed1 == 0) {
    stopDrive(1);
    stopDrive(3);
  }else {
    DriveBL.spin(forward,res1 * multiplier,rpm);
    DriveFL.spin(forward,res1 * multiplier,rpm);
  }
  
  if(speed2 == 0) {
    stopDrive(2);
    stopDrive(4);
  }else {
    DriveBR.spin(forward,res2 * multiplier,rpm);
    DriveFR.spin(forward,res2 * multiplier,rpm);
  }
}

int asyncStop (void* args) {
  asyncParams *syncs = (asyncParams *) args;
  vex::task::sleep(syncs->timeDelay);
  syncs->toStop->stop();
  return 0;
}

void initialize () {
  DriveFL.setBrake(brake);
  DriveFR.setBrake(brake);
  DriveBL.setBrake(brake);
  DriveBR.setBrake(brake);

  midL.setBrake(brake);
  midR.setBrake(brake);

  backArm.setBrake(brake);

  Brain.Screen.setPenColor("#FFFFFF");
  Brain.Screen.drawRectangle(0, 0, 500, 500, "#000000");
  Brain.Screen.drawCircle(100,50,20,"#FFFFFF");
  Brain.Screen.drawCircle(300,50,20,"#FFFFFF");

  for(int i = 0; i < 10; i ++) {
    Brain.Screen.drawLine(130,100 + i,165,150 + i);
    Brain.Screen.drawLine(165,150 + i,200,120 + i);
    Brain.Screen.drawLine(200,120 + i,235,150 + i);
    Brain.Screen.drawLine(235,150 + i,270,100 + i);
  }

  while(true){
    drawFace("#000000", "#FFFFFF");

    vex::task::sleep(100);
  }
}

void driveLoop () {

  int driveAms[] = {0,0};

  // ---- DRIVING ----

  if(controlAxis(3) > driveThreshold || controlAxis(3) < -driveThreshold){
    driveAms[0] += controlAxis(3)*revAm;
    driveAms[1] += controlAxis(3)*revAm;
  }

  if(controlAxis(1) > turnThreshold || controlAxis(1) < -turnThreshold){
    driveAms[0] += controlAxis(1);
    driveAms[1] -= controlAxis(1);
  }

  drive(driveAms[0],driveAms[1]);

  // if(controlAxis(2) > driveThreshold || controlAxis(2) < -driveThreshold){
  //   convAms[0] += controlAxis(2);
  //   convAms[1] += controlAxis(2);
  // }

  // if(controlAxis(1) > turnThreshold || controlAxis(1) < -turnThreshold){
  //   convAms[0] += controlAxis(1);
  //   convAms[1] -= controlAxis(1);
  // }

  if(controlButton('r', true)){
    if(!smallSwitch.pressing()){
      midL.spin(forward,50,rpm);
      midR.spin(forward,50,rpm);
    }else {
      midL.stop();
      midR.stop();
    }
  }else if(controlButton('r', false)){
    midL.spin(reverse,50,rpm);
    midR.spin(reverse,50,rpm);
  }else {
    midL.stop();
    midR.stop();
  }

  if(controlButton('l', true)){
    backArm.spin(forward,50,rpm);
  }else if(controlButton('l', false)){
    backArm.spin(reverse,50,rpm);
  }else {

    if(controlButton('U', false)){
      backArm.spin(forward,100,rpm);
    }else if(controlButton('D', false)){
      backArm.spin(reverse,100,rpm);
    }else {
      backArm.stop();
    }
    
  }

  // ---- REVMODE ----  

  if(controlButton('B', true) && !bPressed){
    if(revmode){
      revAm = 1;
      revmode = false;
      Controller1.rumble("...");
    }else {
      revAm = -1;
      revmode = true;
      Controller1.rumble("--");
    }
    bPressed = true;
  }else if (!controlButton('B', true)){
    bPressed = false;
  }
  
  // ---- SLOWMODE ----

  if(controlButton('A', true) && !aPressed){
    if(slowmode){
      multiplier = normMult;
      slowmode = false;
      Controller1.rumble(".");
    }else {
      multiplier = slowMult;
      slowmode = true;
      Controller1.rumble("..");
    }
    aPressed = true;
  }else if (!controlButton('A', true)){
    aPressed = false;
  }

  cycles++;

  driveLoop();
}

void expand () {
  // midL.spinFor(reverse, 4*90, degrees, 100, rpm, false);
  // midR.spinFor(reverse, 4*90, degrees, 100, rpm, false);
  backArm.spinFor(reverse, 7*(110+160), degrees, 50, rpm);
}

void skillAutonomous() {
  expand();

  //driveTime(-100, 2.0);
  driveTime(-50, 2.0);

  // midL.spinFor(forward, 4*70, degrees, 100, rpm, false);
  // midR.spinFor(forward, 4*70, degrees, 100, rpm, false);
  backArm.spinFor(forward, 7*(40), degrees, 100, rpm, false);

  driveTime(-10, 0.3);

  driveTime(0, 1);

  driveTime(-10, 0.3);

  //driveTime(100, 0.7);
  driveTime2(100, 0, 3.5);
}

void driveAutonomous() {
  expand();
}

int main() {

  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();

  // Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(skillAutonomous);
  Competition.drivercontrol(driveLoop);

  // Run the pre-autonomous function.
  initialize();

  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(100, msec);
  }
}
