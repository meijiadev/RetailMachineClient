#ifndef FileManager_h__
#define FileManager_h__


#include <vector>
#include <string>
#include "src/Utility/Singleton.h"
#include "src/Utility/FileManagerBase.h"

using namespace DDRFramework;
class FileManager :public FileManagerBase, public CSingleton<FileManager>
{
public:
	FileManager();
	~FileManager();

};

#endif // FileManager_h__
