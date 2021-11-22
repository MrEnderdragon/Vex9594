using namespace vex;

extern brain Brain;

// VEXcode devices
extern controller Controller1;
extern motor DriveBL;
extern motor DriveBR;
extern motor DriveFL;
extern motor DriveFR;
extern motor midL;
extern motor midR;
extern motor backArm;
extern limit smallSwitch;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void  vexcodeInit( void );