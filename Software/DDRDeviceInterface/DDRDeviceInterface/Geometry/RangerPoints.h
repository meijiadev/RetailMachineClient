#ifndef __DDR_RANGED_POINTS_H_INCLUDED
#define __DDR_RANGED_POINTS_H_INCLUDED

namespace DDRGeometry
{
#ifndef GEOMETRY_APOINT_DEFINED
#define GEOMETRY_APOINT_DEFINED
	typedef struct _A_PolarPt
	{
#pragma pack(1)
		float angle; // in DEGREE and COUNTER-CLOCKWISE (0 defined as directly forward)
		float distance; // in CENTIMETERS
#pragma pack()
	} APoint;
#endif
}

#endif // __DDR_RANGED_POINTS_H_INCLUDED