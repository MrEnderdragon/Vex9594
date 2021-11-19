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
// Drive1               motor         12              
// Turn1                motor         10              
// Drive2               motor         11              
// Turn2                motor         9               
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"
#include <cmath>
using namespace vex;
competition Competition;

// ---- USER INPUT VARIABLES ----
int j = 1; //time per blink
int m = 60; // time per meow
int driveThreshold = 10;
int turnThreshold = 10;
int moveAcc = 5; // in millisecs per rep
float accelGain = 0.0001;

// ---- PROGRAM VARIABLES ----

int isBlue = 0;

double curSpeed = 0;

bool blinked = false; //whether is blinking
bool opened = false; //whether is meowing

bool slowmode = false;
bool aPressed = false;

int controlAxis (int axis){
  switch(axis){
    case 1: {
      return Controller1.Axis1.position();
    }
    case 2: {
      return Controller1.Axis2.position();
    }
    case 3: {
      return Controller1.Axis3.position();
    }
    case 4: {
      return Controller1.Axis4.position();
    }

    default:
      return 0;
  }
}

bool controlButton (char button, bool top = false){
  switch(button){
    case 'A':{
      return Controller1.ButtonA.pressing();
    }
    case 'B':{
      return Controller1.ButtonB.pressing();
    }
    case 'X':{
      return Controller1.ButtonX.pressing();
    }
    case 'Y':{
      return Controller1.ButtonY.pressing();
    }

    case 'U':{
      return Controller1.ButtonUp.pressing();
    }
    case 'D':{
      return Controller1.ButtonDown.pressing();
    }
    case 'L':{
      return Controller1.ButtonLeft.pressing();
    }
    case 'R':{
      return Controller1.ButtonRight.pressing();
    }

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

void stopDrive(){
  //stops driving

  Drive1.stop();
  Drive2.stop();
}

void drawFace (const char * bg, const char * linee) {
  int v1 = rand() % 10;
  int v2 = rand() % 60;
  
  if (v1 == 0 && !blinked){
      
    blinked = true;
    
    Brain.Screen.setPenColor(bg);
    
    Brain.Screen.drawRectangle(1,1,400,75,bg);
    
    //Brain.Screen.drawCircle(100,50,20,"#FFFFFF");
    //Brain.Screen.drawCircle(300,50,20,"#FFFFFF");
    
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

void drive(int speed){
  float res = fmax(fmin(speed, 200),-200);

  if(speed == 0) stopDrive();
  else {
    Drive1.spin(vex::directionType::fwd,res,vex::velocityUnits::rpm);
    Drive2.spin(vex::directionType::fwd,res,vex::velocityUnits::rpm);
  }
}

void turnTo (int degrees) {
  float res = fmax(fmin(degrees, 45),-45);

  Turn1.startRotateTo(res, vex::degrees, 200, vex::rpm);
  Turn2.startRotateTo(res, vex::degrees, 200, vex::rpm);
}

void initialize () {
  int origRed1[] = {10,10}; 
  int origBlue1[] = {270,10}; 

  Drive1.setBrake(vex::brakeType::coast);
  Drive1.setBrake(vex::brakeType::coast);

  int size[] = {200,100};

  if(isBlue == 0){
    Brain.Screen.setPenColor("#FFFFFF");
    Brain.Screen.drawRectangle(0, 0, 500, 500, "#8c1e00");
    Brain.Screen.drawCircle(100,50,20,"#FFFFFF");
    Brain.Screen.drawCircle(300,50,20,"#FFFFFF");

    for(int i = 0; i < 10; i ++) {
      Brain.Screen.drawLine(130,100 + i,165,150 + i);
      Brain.Screen.drawLine(165,150 + i,200,120 + i);
      Brain.Screen.drawLine(200,120 + i,235,150 + i);
      Brain.Screen.drawLine(235,150 + i,270,100 + i);
    }
  }else if(isBlue == 1){
    
  }else {
    Brain.Screen.drawRectangle(0, 0, 500, 500, "#000000");

    Brain.Screen.setPenColor("#ff3700");
    Brain.Screen.drawRectangle(origRed1[0], origRed1[1], size[0], size[1], "#8c1e00");

    Brain.Screen.setPenColor("#00b7ff");
    Brain.Screen.drawRectangle(origBlue1[0], origBlue1[1], size[0], size[1], "#00719e");
  }

  // Brain.Screen.setPenColor("#ff3700");
  // Brain.Screen.drawRectangle(origRed1[0], origRed1[1], size[0], size[1], "#8c1e00");

  // Brain.Screen.setPenColor("#00b7ff");
  // Brain.Screen.drawRectangle(origBlue1[0], origBlue1[1], size[0], size[1], "#00719e");

  while(true){
    if(Brain.Screen.pressing() || controlButton('X') || controlButton('Y')){
      Controller1.rumble(".");
      int X = Brain.Screen.xPosition();//X pos of press
      int Y = Brain.Screen.yPosition();// Y pos of press

      //Checks if press is within boundaries of rectangle
      // red rect (X)
      if (controlButton('X') || ((X >= origRed1[0] && X <= origRed1[0] + size[0]) && (Y >= origRed1[1] && Y <= origRed1[1] + size[1]))){
        Brain.Screen.setPenColor("#FFFFFF");
        Brain.Screen.drawRectangle(0, 0, 500, 500, "#8c1e00");
        Brain.Screen.drawCircle(100,50,20,"#FFFFFF");
        Brain.Screen.drawCircle(300,50,20,"#FFFFFF");

        for(int i = 0; i < 10; i ++) {
            Brain.Screen.drawLine(130,100 + i,165,150 + i);
            Brain.Screen.drawLine(165,150 + i,200,120 + i);
            Brain.Screen.drawLine(200,120 + i,235,150 + i);
            Brain.Screen.drawLine(235,150 + i,270,100 + i);
        }
        isBlue = 0;
        
        //blue rect (Y)
      }else if (controlButton('Y') || ((X >= origBlue1[0] && X <= origBlue1[0] + size[0]) && (Y >= origBlue1[1] && Y <= origBlue1[1] + size[1]))){
        Brain.Screen.setPenColor("#FFFFFF");
        Brain.Screen.drawRectangle(0, 0, 500, 500, "#00719e");
        Brain.Screen.drawCircle(100,50,20,"#FFFFFF");
        Brain.Screen.drawCircle(300,50,20,"#FFFFFF");

        for(int i = 0; i < 10; i ++) {
            Brain.Screen.drawLine(130,100 + i,165,150 + i);
            Brain.Screen.drawLine(165,150 + i,200,120 + i);
            Brain.Screen.drawLine(200,120 + i,235,150 + i);
            Brain.Screen.drawLine(235,150 + i,270,100 + i);
        }
        
        isBlue = 1;
      }
    }

    if(isBlue == 0){
      drawFace("#8c1e00", "#FFFFFF");
      //  Brain.Screen.drawRectangle(0, 0, 500, 500, "#8c1e00");
    }else {
      drawFace("#00719e", "#FFFFFF");
      //  Brain.Screen.drawRectangle(0, 0, 500, 500, "#00719e");
    }

    vex::task::sleep(100);
  }
}

void driveLoop () {

  // int driveSpeed = 0;

  // ---- DRIVING ----

  

  if(controlAxis(3) > driveThreshold || controlAxis(3) < -driveThreshold){
    curSpeed = fmax(fmin(curSpeed + controlAxis(3)*accelGain, 200), -200);
  }else {
    curSpeed = 0;
  }

  drive(curSpeed);

  if(controlAxis(1) > driveThreshold || controlAxis(1) < -driveThreshold){
    turnTo(controlAxis(1)*45/100);
  }else {
    turnTo(0);
  }

  driveLoop();
}

int main() {

  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();

  Competition.drivercontrol(driveLoop);

  // Run the pre-autonomous function.
  initialize();

  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(100, msec);
  }
}
