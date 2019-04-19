#include "FileManager.h"

#include <iostream>
#include <string>
#include <regex>
#include <cppfs/fs.h>
#include <cppfs/FileHandle.h>
#include <cppfs/FileIterator.h>
#include <cppfs/FilePath.h>

#ifdef _WINDOWS
#include <cppfs/windows/LocalFileSystem.h>
#endif

#include "GlobalManager.h"

#include "../../../Shared/src/Utility/CommonFunc.h"
#include "../../../Shared/src/Utility/DDRMacro.h"
#include "../../../Shared/src/Utility/Logger.h"
using namespace cppfs;

FileManager::FileManager()
{

	m_RootPath = GlobalManager::Instance()->GetLocalServerConfig().GetValue("FileCacheBaseDir");
	CheckFiles();

}
FileManager::~FileManager()
{

}
