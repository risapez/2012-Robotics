#include "WPILib.h"
#include "math.h"


/*
* This is drivetrain code...
 */ 

class Robot : public IterativeRobot
{

	// Declare variable for the robot drive system
	RobotDrive *drivetrain; 
	
	//Compressor
	Compressor *compressor;
	
	// Declare a variable to use to access the driver station object
	DriverStation *m_ds;
	
	// Declare variables for the controllers being used (12 buttons per controller)
	//GetTwist is right joystick
	//GetY is left joystick
	/* Buttons...
	 * X=1
	 * A=2
	 * B=3
	 * Y=4
	 * LB=5
	 * RB=6
	 * LT=7
	 * RT=8
	 * Back=9
	 * Start=10
	 */
	Joystick *gamepad;
	
	
	
	// Declare an encoder variable
	//Encoder * right_encoder;
	//Encoder * left_encoder;
	
	//solenoids
	Solenoid *drivetrain_pressure;
	Solenoid *drivetrain_vent;

	
	//Declare global variables
	
	//DriveTrainValues
	float right;
	float left;
	float oldright;
	float oldleft;
	float useright;
	float useleft;
	float threshold;
	float leftchange;
	float rightchange;
	float leftdead; // left deadzone threshold
	float rightdead; // right deadzone threshold
	
	//Encoder
	float encoder_value;


public:
	
	//Constructor (initialize variables)
	Robot (void){
		
		// Create a robot using standard right/left robot drive on PWMS 1, 2, 9, and 10
		drivetrain = new RobotDrive(1, 2, 9, 10);
		// overrides the default expiration time (0.5 seconds) and sets to 15 seconds, the length of autonomous
		drivetrain->SetExpiration(15);
		
		
		//Declare compressor
		//(switch-GPIO, relay)
		compressor=new Compressor(3,1);
		
		// Acquire the Driver Station object
		// GetInstance gets a pointer to a driver station object
		m_ds = DriverStation::GetInstance();
		
		// Define joystick USB ports on the Drivers Station
		gamepad = new Joystick(1);
		
		//Define encoder (channel A, channel B, true, k4X)
		//true tells the encoder to not invert the counting direction
		//right_encoder = new Encoder(1, 2, true);
		//left_encoder = new Encoder(1, 2, true);
		
		//Solenoids
		drivetrain_pressure=new Solenoid(1);
		drivetrain_vent=new Solenoid(8);

		
		// Define global variables here
		
		//DriveTrainValues
		threshold=0.2;
		right=0;
		left=0;
		oldright=0;
		oldleft=0;
		useright=0;
		useleft=0;
		leftchange=0;
		rightchange=0;
		leftdead=0.04; //was 0.02
		rightdead=0.04; //was 0.02k
		
		//Encoder
		encoder_value =0;

	
	}

/******************* Inits ************************/
	
	// Actions which would be performed once (and only once) upon initialization of the robot would be put here.
	void RobotInit(void) 
	{
		//Start compressor
		compressor->Start();
	}
	
	
	//First time disabled mode is called
	void DisabledInit(void) 
	{
		
	}

	
	//First time disabled mode is called
	void AutonomousInit(void) 
	{

	}

	
	//First time Teleop is called
	void TeleopInit(void) 
	{
		
	}
	
 /******************* Periodic - run continuously  ****************************/
	
	void DisabledPeriodic(void) 
	{
		
	}
	
	
	void AutonomousPeriodic(void) 
	{
		//Start compressor
		compressor->Start();
		
		//Autonomous code goes here
	}
	
	
	void TeleopPeriodic(void ) 
	{
		 /* 
		 * Code placed in here will be called only when a new packet of information
		 * has been received by the Driver Station.  Any code which needs new information
		 * from the DS should go in here
		 */
		
		//Start compressor
		compressor->Start();
		
		driveTrainValues();
		deadzone();
		
		//Drivetrain.....
		//When button eight is pressed robot drives at 25% speed
		printf("right: %f and left: %f\n", useright, useleft);
		if (gamepad->GetRawButton(8)) 
		{
			drivetrain->TankDrive((-0.5*(useleft)), (-0.5*(useright)));
			//Negative for switched wires
		}
		else 
		{
			drivetrain->SetLeftRightMotorOutputs(-useleft, -useright);
			//Normal driving
			//Negative for switched wires
		}		
		
	}
	
/********************************** Miscellaneous Routines ****************************/
	
	void driveTrainValues(void) {
		
		//Testing the encoder...
		//Get the encoder value
		//encoder_value = right_encoder->Get();
		
		//Print encoder value
		//printf("right encoder value: %f ", encoder_value);
		
		
		//Shifting gears...
		//Low gear
		if (gamepad->GetRawButton(5))
		{
			printf("The left button is being pressed.\n");
			drivetrain_pressure->Set(false);
			drivetrain_vent->Set(true);
		}
		
		//Six is high gear
		if (gamepad->GetRawButton(6))
		{
			printf("The right button is being pressed.\n");
			drivetrain_pressure->Set(true);
			drivetrain_vent->Set(false);
		}
		
		

		//assign right joystick value to 'right'
		right=(gamepad->GetTwist());
		//find the change in joystick values
		rightchange=fabs(oldright-right);
		
		// if the change in joystick values is less than 0.2 then use the old right value vs the new one
		//Stops driver from accelerating the robot too quickly
		if(rightchange<=threshold) {
			useright=right;
		}
		//else... if the new right is greater than the old one use the old value plus the threshold (faster)
		//if the new right is less than the old right use the old value minue the threshold (slower)
		//so that really large movements of the joystick don't translate immediately into a really big acceleration
		else {
			if(oldright<right) {
				useright=oldright+threshold;
			}
			if(oldright>right) {
				useright=oldright-threshold;
			}
		}
		
		//Same as right, but on the left side
		left=(gamepad->GetY());
		leftchange=fabs(oldleft-left);

		if(leftchange<=threshold) {
			useleft=left;
		}
		else {
			if(oldleft<left) {
				useleft=oldleft+threshold;
			}
			if(oldleft>left) {
				useleft=oldleft-threshold;
			}
		}
		
		//printf("hello world");
		
		//make useright and useleft (the values sent to the robot) the old values for the next loop
		oldright=useright;
		oldleft=useleft;

	}
	
	void deadzone(void) {
		
		// If the joystick values, useleft or useright, are less 
		// than the small number "leftdead" or "rightdead" then do not move robot
		
		if((useleft <= leftdead) && (useleft >= -leftdead)) 
		{
			useleft = 0;
		}
		if((useright <= rightdead) && (useright >= -rightdead)) 
		{
			useright = 0;
		}
	}
}; //End of class Robot

//Run the robot class
START_ROBOT_CLASS(Robot);
