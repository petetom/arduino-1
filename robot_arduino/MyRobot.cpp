#include "WPILib.h"
#include <math.h>

/**
 * This is a demo program showing the use of the RobotBase class.
 * The SimpleRobot class is the base of a robot application that will automatically call your
 * Autonomous and OperatorControl methods at the right time as controlled by the switches on
 * the driver station or the field controls.
 */ 
class RobotDemo : public SimpleRobot
{
	RobotDrive myRobot; // robot drive system
	Joystick stick; // only joystick
	Talon vac;
	//DigitalInput comp_sw;
	//Compressor comp;
	//Solenoid sol1,sol2;
	I2C *wire_i2c;
	//I2C *wire_read;
	

public:
	RobotDemo():
		myRobot(1, 2),	// these must be initialized in the same order
		stick(1),		// as they are declared above.
		vac(3)
		//comp_sw(1),
		//comp(1,1),
		//sol1(1),
		//sol2(2)

		
	{
		myRobot.SetExpiration(0.5);
		wire_i2c = DigitalModule::GetInstance(1)->GetI2C(4);
		//wire_read = DigitalModule::GetInstance(1)->GetI2C(4);  //was 4
		// set addr to 4 or 5 makes no difference
		//wire_read->SetCompatibilityMode(true);
	}

	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous()
	{
		myRobot.SetSafetyEnabled(false);
		myRobot.Drive(-0.5, 0.0); 	// drive forwards half speed
		Wait(2.0); 				//    for 2 seconds
		myRobot.Drive(0.0, 0.0); 	// stop robot
	}

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl()
	{
		//float v_cont;
		//myRobot.SetSafetyEnabled(false);
		//comp.Start();
		Timer(t1);
		t1.Reset();
		t1.Start();
		double t0;
		double t6,t7 = t1.Get();
		//printf("%f  %f\n",t6,t7);
		int stat;
		unsigned char i2c_d[3];
		unsigned char i2c_s[1] = {0x74};

		
		while (IsOperatorControl())
		{
			t0 = t1.Get();
			myRobot.ArcadeDrive(stick); // drive with arcade style (use right stick)
			Wait(0.005);				// wait for a motor update time
			/*
			if(stick.GetTrigger())
			{	sol1.Set(true);
				sol2.Set(false);
				
			}else
			{
				sol1.Set(false);
				sol2.Set(true);
			}*/
			
			if(stick.GetRawButton(6) and ((t1.Get() - t6) > 1))
			{ t6 = t1.Get();
			  printf("button 6 ");
			  printf("  %d\n",wire_i2c->Write(3,0x67));				//send char 'g'
			}
			
			if(stick.GetRawButton(7) and ((t1.Get() - t7) > 1))
			{ t7 = t1.Get();
			  printf("button 7");
			  printf("  %d\n",wire_i2c->Write(0,0x72));				//send char 'r'
			}
			
			if(stick.GetRawButton(8) and ((t1.Get() - t7) > 1))		//read i2c data temperature
			{	t7 = t1.Get();
				wire_i2c->Write(0,0x74);							//send char 't' - send of this byte 
						//is not require but in this the arduino uses to determine what data to send back
				stat = wire_i2c->Transaction(i2c_s,0,i2c_d,2);		//read 2 bytes
				printf("stat = %d\n",stat);
				printf("value %d   %d\n",i2c_d[0],i2c_d[1]);
				
			}
			
			if(stick.GetRawButton(9) and ((t1.Get() - t7) > 1))		//read i2c data RPM
			{	t7 = t1.Get();
				wire_i2c->Write(0,0x73);							//send char 's' - send RPM
				stat = wire_i2c->Transaction(i2c_s,0,i2c_d,1);		//read 1 byte
				printf("stat = %d\n",stat);
				printf("value %d\n",i2c_d[0]);
							
			}
			
		/*
			v_cont = stick.GetThrottle();

			if(v_cont<0)
				v_cont = -1 * v_cont;
			printf("value = %f\n",v_cont);
			vac.Set(v_cont);
		*/	
			Wait(.01);
			//printf("time in loop %f\n",t1.Get()-t0);
			
		}
	}
	
	/**
	 * Runs during test mode
	 */
	void Test() {

	}
};

START_ROBOT_CLASS(RobotDemo);

