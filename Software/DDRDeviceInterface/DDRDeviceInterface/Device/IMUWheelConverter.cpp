//#include "stdafx.h"

#include "IMUWheelConverter.h"

#include <math.h>

#define UPPERLIMIT_LINVEL 1.5f
#define UPPERLIMIT_ANGVEL 1.0f

namespace DDRDevice {

IMUWheelConverter::IMUWheelConverter() {
	m_bInitialized = false;
}

void IMUWheelConverter::Init(double leftRadius, double rightRadius,
	                         double wheelBase, double reading2AR_wheel,
							 double reading2AR_gyro) {
	__ROBOT_SIZE_LEFT_WHEEL_RADIUS = leftRadius;
	__ROBOT_SIZE_RIGHT_WHEEL_RADIUS = rightRadius;
	__ROBOT_SIZE_WHEEL_BASE = wheelBase;
	__ROBOT_SIZE_READING_TO_WHEEL_ANGULAR_VEL = reading2AR_wheel;
	__ROBOT_IMU_GYROZ_SCALE = reading2AR_gyro;
	__LReading2LinVel = __ROBOT_SIZE_READING_TO_WHEEL_ANGULAR_VEL *
		__ROBOT_SIZE_LEFT_WHEEL_RADIUS;
	__RReading2LinVel = __ROBOT_SIZE_READING_TO_WHEEL_ANGULAR_VEL *
		__ROBOT_SIZE_RIGHT_WHEEL_RADIUS;
	m_bInitialized = true;
}

void IMUWheelConverter::ComputeControlSignal(double desiredLinVel, double desiredAngVel,
	                                         int16_t &leftCommand, int16_t &rightCommand) const {
	if (m_bInitialized) {
		if (desiredLinVel < -UPPERLIMIT_LINVEL) {
			desiredLinVel = -UPPERLIMIT_LINVEL;
		} else if (desiredLinVel > UPPERLIMIT_LINVEL) {
			desiredLinVel = UPPERLIMIT_LINVEL;
		}
		if (desiredAngVel < -UPPERLIMIT_ANGVEL) {
			desiredAngVel = -UPPERLIMIT_ANGVEL;
		} else if (desiredAngVel > UPPERLIMIT_ANGVEL) {
			desiredAngVel = UPPERLIMIT_ANGVEL;
		}
		leftCommand = (int16_t)round((desiredLinVel - 0.5 * desiredAngVel * __ROBOT_SIZE_WHEEL_BASE) / __LReading2LinVel);
		rightCommand = (int16_t)round((desiredLinVel + 0.5 * desiredAngVel * __ROBOT_SIZE_WHEEL_BASE) / __RReading2LinVel);
	} else {
		leftCommand = rightCommand = (int16_t)0;
	}
}

void IMUWheelConverter::ComputeVel(int16_t leftCommand, int16_t rightCommand,
	                               double &linVel, double &angVel) const
{
	linVel = 0.5 * (leftCommand * __LReading2LinVel + rightCommand * __RReading2LinVel);
	angVel = (rightCommand * __RReading2LinVel - leftCommand * __LReading2LinVel) / __ROBOT_SIZE_WHEEL_BASE;
}

}