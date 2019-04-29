#ifndef __ROBOT_WHEEL_MODULE_H_INCLUDED__
#define __ROBOT_WHEEL_MODULE_H_INCLUDED__

#include "CommonDef/Types.h"

namespace DDRDevice
{
	class IMUWheelConverter
	{	// velocities are in m/s and rad/s
	public:
		IMUWheelConverter();
		void Init(double leftRadius, double rightRadius,
			      double wheelBase, double reading2AR_wheel,
				  double reading2AR_gyro);
		bool IsInitialized() const { return m_bInitialized; }
		double GetLinVel(int16_t leftReading, int16_t rightReading) const {
			// new standard for Kinco chassis: readings are RPM*100 (2017/01/06)
			return ((leftReading * __LReading2LinVel
				+ rightReading * __RReading2LinVel) * 0.5);
		}
		double GetAngVel_IMU(float gyroZReading, double zero_off) const {
			return ((gyroZReading - zero_off) / __ROBOT_IMU_GYROZ_SCALE);
		}
		inline double GetAngVel_wheel(int16_t leftReading, int16_t rightReading) const {
			// past standard	- Dabai, reversal freq. * 10 (132 reversals per cycle)
			//					- shEriff, reversal freq. * 10 (660 reversals per cycle with a 55:1 gear)
			// new standard for Kinco chassis: readings are RPM*100 (2017/01/06)
			return ((-leftReading * __LReading2LinVel
				+ rightReading * __RReading2LinVel) / __ROBOT_SIZE_WHEEL_BASE);
		}
		void ComputeControlSignal(double desiredLinVel, double desiredAngVel,
			int16_t &leftCommand, int16_t &rightCommand) const;
		void ComputeVel(int16_t leftCommand, int16_t rightCommand,
			double &linVel, double &angVel) const;

	protected:
		bool m_bInitialized;
		double __ROBOT_SIZE_LEFT_WHEEL_RADIUS;
		double __ROBOT_SIZE_RIGHT_WHEEL_RADIUS;
		double __ROBOT_SIZE_WHEEL_BASE;
		double __ROBOT_SIZE_READING_TO_WHEEL_ANGULAR_VEL;
		double __ROBOT_IMU_GYROZ_SCALE;

		double __LReading2LinVel;
		double __RReading2LinVel;
	};
}

#endif
