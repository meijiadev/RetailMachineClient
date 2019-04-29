#ifndef __DDR_DEVICE_LIB_FACTORY_H_INCLUDED__
#define __DDR_DEVICE_LIB_FACTORY_H_INCLUDED__

#include <vector>
#include <memory>
#include "DDRDeviceInterface.h"

namespace DDRDevice {
	inline std::shared_ptr<DDRDeviceInterface> CreateDDRDeviceModule() {
		return std::shared_ptr<DDRDeviceInterface>(_createDDRDeviceModule());
	}
}

#endif // __DDR_PPOAC_LIB_FACTORY_H_INCLUDED__