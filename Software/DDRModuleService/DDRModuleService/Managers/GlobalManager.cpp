#include "GlobalManager.h"


#include "../../../Shared/src/Utility/XmlLoader.h"
#include "../../../Shared/src/Utility/CommonFunc.h"
#include "../Servers/LocalServerUdpDispatcher.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <fstream>

#include "../../../Shared/proto/BaseCmd.pb.h"
using namespace DDRCommProto;
using namespace std;

GlobalManager::GlobalManager():m_LocalServerConfig("Config/LocalServer/LocalServerConfig.xml")
{
	std::string dbfile = m_LocalServerConfig.GetValue("DatabaseFile");

	if (!dbfile.empty())
	{

		DBManager::Instance()->Open(dbfile);
	}
	else
	{
		DebugLog("DBFile Load Error");
	}


}
GlobalManager::~GlobalManager()
{

}
void GlobalManager::StartTcpServer()
{
	try
	{
		int port = m_LocalServerConfig.GetValue<int>("TcpPort");
		std::string servername = m_LocalServerConfig.GetValue("ServerName");
		std::string threadCount = m_LocalServerConfig.GetValue("ServerThreadCount");

		//loader.SetValue(std::string("ServerName"), std::string("LocalServerV2"));
		//loader.DoSave();

		m_spTcpServer = std::make_shared<LocalTcpServer>(port);
		m_spTcpServer->Start(std::stoi(threadCount));
	}
	catch (std::exception& e)
	{

		DebugLog("StartTcpServer Error");
	}



}
void GlobalManager::StopTcpServer()
{
	if (m_spTcpServer)
	{
		m_spTcpServer->Stop();
	}

}



void GlobalManager::StartUdpServer()
{
	try
	{
		std::string port = m_GlobalConfig.GetValue("UdpPort");
		std::string tcpport = m_LocalServerConfig.GetValue("TcpPort");
		std::string servername = m_LocalServerConfig.GetValue("ServerName");


		auto bc = std::make_shared<bcLSAddr>();




		auto plsinfo = bc->add_lsinfos();
		for (auto addr : DDRFramework::GetLocalIPV4())
		{
			plsinfo->add_ips(addr);
		}
		plsinfo->set_name(servername);
		plsinfo->set_stype(bcLSAddr_eServiceType::bcLSAddr_eServiceType_LocalServer);
		plsinfo->set_port(std::stoi(tcpport));
		plsinfo->set_robotid(GetRobotID());



		m_spUdpServer = std::make_shared<UdpSocketBase>();

		m_spUdpServer->Start();
		m_spUdpServer->GetSerializer()->BindDispatcher(std::make_shared<LocalServerUdpDispatcher>());

		m_spUdpServer->BindOnDisconnect(std::bind(&GlobalManager::OnUdpDisconnect, this, std::placeholders::_1));
		m_spUdpServer->StartBroadcast(std::stoi(port), bc, 2000);
	}
	catch (std::exception& e)
	{
		DebugLog("StartUdpServer Error");
	}



}
void GlobalManager::StopUdpServer()
{
	if (m_spUdpServer)
	{
		m_spUdpServer->StopBroadcast();
		m_spUdpServer->Stop();
		//m_spUdpServer.reset();donot reset here cause Stop is async ,it will release when OnDisconnect is called

	}
}
void GlobalManager::OnUdpDisconnect(UdpSocketBase& container)
{
	if (m_spUdpServer)
	{
		m_spUdpServer.reset();
	}
}

bool GlobalManager::CheckRemoteNetwork()
{
	std::string b = m_LocalServerConfig.GetValue("CheckRemote");
	if (b == "TRUE")
	{
		return true;
	}
	else
	{
		return false;
	}
}

DBManager::DBManager()
{

}
DBManager::~DBManager()
{

}

bool DBManager::Open(std::string& filename)
{
	m_FileName = filename;
	std::ifstream ifs(filename.c_str());
	if (!ifs.good())
	{
		std::ofstream outfile(filename);
		outfile.close();

		m_DB.open(m_FileName.c_str());
		CreateUserTable();
		m_DB.close();
	}

	try
	{
		int i, fld;
		time_t tmStart, tmEnd;
		DebugLog("SQLite Version: %s", m_DB.SQLiteVersion());
		m_DB.open(m_FileName.c_str());

	}
	catch (CppSQLite3Exception& e)
	{
		cerr << e.errorCode() << ":" << e.errorMessage() << endl;
		return false;
	}
	return true;
}
bool DBManager::Remove(std::string& filename)
{
	remove(filename.c_str());
	return true;
}


bool DBManager::CreateUserTable()
{
	try
	{
		m_DB.execDML("create table user(username string, userpwd string,priority int);");
		int nRows = 0;
		nRows = m_DB.execDML("insert into user values ('admin', 'admin',0);");
		nRows = m_DB.execDML("insert into user values ('admin_pc', 'admin_pc',0);");
		nRows = m_DB.execDML("insert into user values ('admin_android', 'admin_android',0);");
		/*nRows = m_DB.execDML("insert into user values ('administrator', 'admin',99);");
		nRows = m_DB.execDML("update user set priority = 99 where username = 'admin';");
		nRows = m_DB.execDML("delete from user where username = 'administrator';");*/
	}
	catch (CppSQLite3Exception& e)
	{
		DebugLog("%s", e.errorMessage());
		return false;
	}

	return true;
}

bool DBManager::VerifyUser(std::string username, std::string pwd)
{
	CppSQLite3Buffer bufSQL;
	bufSQL.format("select * from user where username = '%s'and userpwd = '%s';", username.c_str(),pwd.c_str());
	CppSQLite3Query q = m_DB.execQuery(bufSQL);
	if (q.eof())
	{
		return false;
	}
	return true;

}

int DBManager::GetUserPriority(std::string username)
{
	CppSQLite3Buffer bufSQL;
	bufSQL.format("select priority from user where username = '%s';", username.c_str());
	CppSQLite3Query q = m_DB.execQuery(bufSQL);
	if (q.eof())
	{
		return -1;
	}
	else
	{
		int priority = q.getIntField("priority");
		return priority;
	}
}



bool DBManager::CodeSample()
{
	///////////////////////////////////////////////////////////////////
	//// Transaction Demo
	//// The transaction could just as easily have been rolled back
	///////////////////////////////////////////////////////////////////
	//int nRowsToCreate(50000);
	//cout << endl << "Transaction test, creating " << nRowsToCreate;
	//cout << " rows please wait..." << endl;
	//tmStart = time(0);
	//m_DB.execDML("begin transaction;");

	//for (i = 0; i < nRowsToCreate; i++)
	//{
	//	char buf[128];
	//	sprintf(buf, "insert into emp values (%d, 'Empname%06d');", i, i);
	//	m_DB.execDML(buf);
	//}

	//m_DB.execDML("commit transaction;");
	//tmEnd = time(0);

	//////////////////////////////////////////////////////////////////
	//// Demonstrate CppSQLiteDB::execScalar()
	//////////////////////////////////////////////////////////////////
	//cout << m_DB.execScalar("select count(*) from emp;")
	//	<< " rows in emp table in ";
	//cout << tmEnd - tmStart << " seconds (that was fast!)" << endl;

	//////////////////////////////////////////////////////////////////
	//// Re-create emp table with auto-increment field
	//////////////////////////////////////////////////////////////////
	//cout << endl << "Auto increment test" << endl;
	//m_DB.execDML("drop table emp;");
	//m_DB.execDML(
	//	"create table emp(empno integer primary key, empname char(20));");
	//cout << nRows << " rows deleted" << endl;

	//for (i = 0; i < 5; i++)
	//{
	//	char buf[128];
	//	sprintf(buf,
	//		"insert into emp (empname) values ('Empname%06d');", i + 1);
	//	m_DB.execDML(buf);
	//	cout << " primary key: " << m_DB.lastRowId() << endl;
	//}

	/////////////////////////////////////////////////////////////////////
	//// Query data and also show results of inserts into auto-increment field
	////////////////////////////////////////////////////////////////////
	//cout << endl << "Select statement test" << endl;
	//CppSQLite3Query q = m_DB.execQuery("select * from emp order by 1;");

	//for (fld = 0; fld < q.numFields(); fld++)
	//{
	//	cout << q.fieldName(fld) << "(" << q.fieldDataType(fld) << ")|";
	//}
	//cout << endl;

	//while (!q.eof())
	//{
	//	cout << q.fieldValue(0) << "|";
	//	cout << q.fieldValue(1) << "|" << endl;
	//	q.nextRow();
	//}

	/////////////////////////////////////////////////////////////////
	//// SQLite's printf() functionality. Handles embedded quotes and NULLs
	//////////////////////////////////////////////////////////////////
	//cout << endl << "SQLite sprintf test" << endl;
	//CppSQLite3Buffer bufSQL;
	//bufSQL.format("insert into emp (empname) values (%Q);", "He's bad");
	//cout << (const char*)bufSQL << endl;
	//m_DB.execDML(bufSQL);

	//bufSQL.format("insert into emp (empname) values (%Q);", NULL);
	//cout << (const char*)bufSQL << endl;
	//m_DB.execDML(bufSQL);

	//////////////////////////////////////////////////////////////////////
	//// Fetch table at once, and also show how to 
	//// use CppSQLiteTable::setRow() method
	////////////////////////////////////////////////////////////////////
	//cout << endl << "getTable() test" << endl;
	//CppSQLite3Table t = m_DB.getTable("select * from emp order by 1;");

	//for (fld = 0; fld < t.numFields(); fld++)
	//{
	//	cout << t.fieldName(fld) << "|";
	//}
	//cout << endl;
	//for (int row = 0; row < t.numRows(); row++)
	//{
	//	t.setRow(row);
	//	for (int fld = 0; fld < t.numFields(); fld++)
	//	{
	//		if (!t.fieldIsNull(fld))
	//			cout << t.fieldValue(fld) << "|";
	//		else
	//			cout << "NULL" << "|";
	//	}
	//	cout << endl;
	//}

	//////////////////////////////////////////////////////////////////////
	//// Test CppSQLiteBinary by storing/retrieving some binary data, checking
	//// it afterwards to make sure it is the same
	////////////////////////////////////////////////////////////////////
	//cout << endl << "Binary data test" << endl;
	//m_DB.execDML("create table bindata(desc char(10), data blob);");

	//unsigned char bin[256];
	//CppSQLite3Binary blob;

	//for (i = 0; i < sizeof bin; i++)
	//{
	//	bin[i] = i;
	//}

	//blob.setBinary(bin, sizeof bin);

	//bufSQL.format("insert into bindata values ('testing', %Q);",
	//	blob.getEncoded());
	//m_DB.execDML(bufSQL);
	//cout << "Stored binary Length: " << sizeof bin << endl;

	//q = m_DB.execQuery("select data from bindata where desc = 'testing';");

	//if (!q.eof())
	//{
	//	blob.setEncoded((unsigned char*)q.fieldValue("data"));
	//	cout << "Retrieved binary Length: "
	//		<< blob.getBinaryLength() << endl;
	//}

	//const unsigned char* pbin = blob.getBinary();
	//for (i = 0; i < sizeof bin; i++)
	//{
	//	if (pbin[i] != i)
	//	{
	//		cout << "Problem: i: ," << i << " bin[i]: "
	//			<< pbin[i] << endl;
	//	}
	//}

	///////////////////////////////////////////////////////////
	//// Pre-compiled Statements Demo
	///////////////////////////////////////////////////////////////
	//cout << endl << "Transaction test, creating " << nRowsToCreate;
	//cout << " rows please wait..." << endl;
	//m_DB.execDML("drop table emp;");
	//m_DB.execDML("create table emp(empno int, empname char(20));");
	//tmStart = time(0);
	//m_DB.execDML("begin transaction;");

	//CppSQLite3Statement stmt = m_DB.compileStatement(
	//	"insert into emp values (?, ?);");
	//for (i = 0; i < nRowsToCreate; i++)
	//{
	//	char buf[16];
	//	sprintf(buf, "EmpName%06d", i);
	//	stmt.bind(1, i);
	//	stmt.bind(2, buf);
	//	stmt.execDML();
	//	stmt.reset();
	//}

	//m_DB.execDML("commit transaction;");
	//tmEnd = time(0);

	//cout << m_DB.execScalar("select count(*) from emp;")
	//	<< " rows in emp table in ";
	//cout << tmEnd - tmStart << " seconds (that was even faster!)" << endl;
	//cout << endl << "End of tests" << endl;
	return false;
}