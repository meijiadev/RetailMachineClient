#ifndef DDRClientInterface_h__
#define DDRClientInterface_h__


#include <google/protobuf/message.h>


#if !defined(USE_DDRSDK_DLL) // inside DLL
#   define DDRSDK_API   __declspec(dllexport)
#else // outside DLL
#   define DDRSDK_API   __declspec(dllimport)
#endif 


namespace DDRSDK
{

	class  DDRSDK_API DDRClientInterface
	{
	public:	 
		DDRClientInterface(){};
		~DDRClientInterface(){};


		bool Init();
		void Deinit();

		bool AddDispatchProcessor()
		{
			return false;
		}

	};
}



#endif // DDRClientInterface_h__
