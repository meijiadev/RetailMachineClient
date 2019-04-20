#ifndef __DDR_COMMUNICATION_LIB_FACTORY_H_INCLUDED__
#define __DDR_COMMUNICATION_LIB_FACTORY_H_INCLUDED__

#include <memory>
#include "CommLibInterface.h"

namespace DDRCommLib {
	std::shared_ptr<FrameSliceGenerator> CreateFrameSliceGenerator() {
		return std::shared_ptr<FrameSliceGenerator>(_createFSG());
	}

	std::shared_ptr<FrameSliceConsumer> CreateFrameSliceConsumer() {
		return std::shared_ptr<FrameSliceConsumer>(_createFSC());
	}
}

#endif // __DDR_COMMUNICATION_LIB_FACTORY_H_INCLUDED__