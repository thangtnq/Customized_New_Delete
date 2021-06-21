//-----------------------------------------------------------------------------
// Copyright Ed Keenan 2018
//----------------------------------------------------------------------------- 
// UnitTest 
//    v.2.1.0
//    v.2.1.1 - fixed C5038 warning
//    v.2.2   - GetStats()
//    v.2.3   - CHECK_EQUAL() bug fix
//    v.2.4   - Print output to console and debug window
//    v.2.5   - Add to End of list
//    v.2.5.1 - Teardown - rework
//    v.2.6   - Baseline
//    v.2.6.1 - fixed warning TestRegistry
//    v.2.7   - Baseline
//    v.2.8   - warning 5039 windows.h
//    v.2.9   - fence
//    v.2.10  - msc_ver 1914, win 10.0.17
//    v.2.11  - warning fixes 
//    v.2.12  - mutex on all trace::out
//    v.2.13  - fixed warnings on mutex and default copy const / assignment
//----------------------------------------------------------------------------- 
//	todo - move print statements to bottom of output
//----------------------------------------------------------------------------- 

#ifndef CPP_UNIT_H
#define CPP_UNIT_H

#include <assert.h>
#include <stdio.h>

//---------------------------------------------------------------------------------------
// UnitTrace - output to debug window
//             isolated from other systems
//
// NOTE: You need to set your project settings
//       Character Set -> Use Multi-Byte Character Set
//
//---------------------------------------------------------------------------------------

// Windows.h include
// many warnings - need to wrap for Wall warnings
#pragma warning( push )
	#pragma warning( disable : 4820 )
	#pragma warning( disable : 4668 )
	#pragma warning( disable : 5039 )
	#include <Windows.h>
#pragma warning( pop ) 

#pragma warning( push )
	#pragma warning( disable : 4365 )
	#pragma warning( disable : 4623 )
	#pragma warning( disable : 4625 )
	#pragma warning( disable : 4626 )
	#pragma warning( disable : 4820 )
	#pragma warning( disable : 5026 )
	#pragma warning( disable : 5027 )
	#pragma warning( disable : 5039 )
	#include <mutex>
#pragma warning( pop ) 

#define UnitTraceBuffSize 256

// Singleton helper class
class UnitTrace
{
public:
	// displays a printf to the output window
	static void out(const char* fmt, ...)
	{
		UnitTrace *pTrace = UnitTrace::privGetInstance();
		assert(pTrace);

		pTrace->mtx.lock();

			va_list args;
			va_start(args, fmt);

			vsprintf_s(pTrace->privBuff, UnitTraceBuffSize, fmt, args);
			OutputDebugString(pTrace->privBuff);

			va_end(args);

		pTrace->mtx.unlock();
	}

	// Big four
	UnitTrace() = default;
	UnitTrace(const UnitTrace &) = delete;
	UnitTrace & operator = (const UnitTrace &) = delete;
	~UnitTrace() = default;

private:
	static UnitTrace *privGetInstance()
	{
		// This is where its actually stored (BSS section)
		static UnitTrace helper;
		return &helper;
	}
	char privBuff[UnitTraceBuffSize];
	std::mutex mtx;
};

//---------------------------------------------------------------------------------------
// Utility class - for testing floats
//
// Needs <cmath> - fabs()
//
//---------------------------------------------------------------------------------------
#include <cmath>

class UnitUtility
{
public:
	static bool AreEqual(float a, float b, float epsilon = 0.001f)
	{
		return (fabs(a - b) < epsilon);
	}

	static bool AreEqual(double a, double b, double epsilon = 0.001f)
	{
		return (fabs(a - b) < epsilon);
	}
};

//---------------------------------------------------------------------------------------
// Unit Tests
//
// Needs this header
//
//---------------------------------------------------------------------------------------
struct UnitStats
{
	UnitStats()
		:testCount(0),
		testPass(0),
		testFail(0), 
		indvAsserts(0)
	{
	}

	// Big four
	UnitStats(const UnitStats &) = default;
	UnitStats & operator = (const UnitStats &) = default;
	~UnitStats() = default;

	// data: ------------------
	int testCount;
	int testPass;
	int testFail;
	int indvAsserts;
};
struct UnitData
{
	UnitData()
		: pMemberName(0),
		pSourceFilePath(0),
		sourceLineNumber(0),
		result(false),
		pad0(0),
		pad1(0),
		pad2(0)
	{
	}

	// Big four
	UnitData(const UnitData &) = default;
	UnitData & operator = (const UnitData &) = default;
	~UnitData() = default;

	// data: -----------------
	const char *pMemberName;
	const char *pSourceFilePath;
	int sourceLineNumber;
	bool result;
	char pad0;
	char pad1;
	char pad2;
};

class UnitSLink
{
public:
	UnitSLink()
	{
		this->Clear();
	}
	// Big four
	UnitSLink(const UnitSLink &) = default;
	UnitSLink & operator = (const UnitSLink &) = default;
	~UnitSLink() = default;

	void Clear()
	{
		this->pNext = 0;
	}

	static void AddToFront(UnitSLink *&pRoot, UnitSLink *pNode )
	{
		if (pRoot == 0)
		{		
			pRoot = pNode;
			assert(pNode->pNext == 0);
		}
		else
		{
			UnitSLink *pTmp = pRoot;
			pRoot = pNode;
			pNode->pNext = pTmp;
		}
	}

	static void AddToEnd(UnitSLink *&pRoot, UnitSLink *pNode)
	{
		if (pRoot == 0)
		{
			pRoot = pNode;
			assert(pNode->pNext == 0);
		}
		else
		{
			UnitSLink *pTmp = pRoot;
			
			while (pTmp != 0)
			{
				if (pTmp->pNext == 0)
				{
					// at the end
					pTmp->pNext = pNode;
					pNode->pNext = 0;
				}

				pTmp = pTmp->pNext;
			}
		}
	}

public:
	// Data
	UnitSLink *pNext;
};

class Test : public UnitSLink
{
public:
	Test(const char * pTestName);

	// Big four
	Test() = delete;
	Test(const Test &) = default;
	Test & operator = (const Test &) = default;
	~Test() = default;


	virtual void run(UnitData &, UnitStats &) = 0;
	// For Tests with NO Teardowns... do nothing
	virtual void teardown() {};

public:
	const char * pName;
	Test *testFunc;
};

class TestRegistry
{
public:
	// Big four
	TestRegistry(const TestRegistry &) = delete;
	TestRegistry & operator = (const TestRegistry &) = delete;
	~TestRegistry() = default;


	static void AddTest(UnitSLink *pTest)
	{
		//UnitTrace::out("TestRegistry:AddTest(%s) \n", pTest->pName);
		assert(pTest != 0);

		TestRegistry *pRegistry = TestRegistry::privGetInstance();

		// add to End
		UnitSLink::AddToEnd((UnitSLink *&)pRegistry->pRoot, pTest);
	}
	static void RunTests()
	{
		UnitTrace::out("\n");
#ifdef _DEBUG
		    UnitTrace::out("------------------- Testing DEBUG ------------------------\n");
		const char * const mode = "Debug";
#else
		const char * const mode = "Release";
	#ifdef MRFAST
			UnitTrace::out("---- Testing MR_FAST ----\n");
	#else
			UnitTrace::out("------------------- Testing RELEASE ----------------------\n");
	#endif
#endif

		TestRegistry *pRegistry = TestRegistry::privGetInstance();
		UnitSLink *pTmp = (UnitSLink *)pRegistry->pRoot;

		while (pTmp != 0)
		{
			pRegistry->_UnitStats.testCount++;

			// downcast to the test
			Test *pTest = (Test *)pTmp;

			// Needed to be added - for fencing issues between tests
			// Release rearranges.. and affects timing
			// This simple fence - fixes all!
			printf("Test:%s \n", pTest->pName);

			// run the test
			pRegistry->_UnitData.result = true;
			
			pTest->testFunc->run(pRegistry->_UnitData, pRegistry->_UnitStats);

			// Always call the teardown
			pTest->testFunc->teardown();
		
			if (pRegistry->_UnitData.result == true)
			{
				pRegistry->_UnitStats.testPass++;
			}
			else
			{
				pRegistry->_UnitStats.testFail++;
			}

			// next test
			pTmp = pTmp->pNext;
		}

		UnitTrace::out("\n");
		UnitTrace::out(" (%s) testPass: %d\n",mode, pRegistry->_UnitStats.testPass);
		UnitTrace::out(" (%s) testFail: %d\n",mode, pRegistry->_UnitStats.testFail);
		UnitTrace::out("\n");
		UnitTrace::out(" testCount: %d\n", pRegistry->_UnitStats.testCount);
		UnitTrace::out("indvChecks: %d\n", pRegistry->_UnitStats.indvAsserts);
		UnitTrace::out("\n");
		UnitTrace::out("-----------------\n");

		printf("\n");
		printf(" (%s) testPass: %d\n", mode, pRegistry->_UnitStats.testPass);
		printf(" (%s) testFail: %d\n", mode, pRegistry->_UnitStats.testFail);
		printf("\n");
		printf(" testCount: %d\n", pRegistry->_UnitStats.testCount);
		printf("indvChecks: %d\n", pRegistry->_UnitStats.indvAsserts);
		printf("\n");
		printf("-----------------\n");

	}

	static UnitStats GetStats()
	{
		TestRegistry *pRegistry = TestRegistry::privGetInstance();
		assert(pRegistry);
		return pRegistry->_UnitStats;
	}

private:
	TestRegistry()
	{
		this->pRoot = 0;
	}
	static TestRegistry * privGetInstance()
	{
		static TestRegistry tRegistry;
		return &tRegistry;
	}

// Data: ------------------------
	UnitData _UnitData;
	UnitStats _UnitStats;
	UnitSLink *pRoot;
};


// a trick to create a c-string
#define STRING_ME(s) #s

// Allow conditional expressions that are constant.
#pragma warning( disable : 4127 )

// Increments total check count
// Creates a clickable format in the output window for failure
// Abort test on first fail
#define CHECK( condition ) \
{ \
	_UnitStats.indvAsserts++;\
	if( !( condition ) ) \
	{ \
		_UnitData.result = false;  \
		_UnitData.pMemberName = this->pName; \
		_UnitData.pSourceFilePath = __FILE__; \
		_UnitData.sourceLineNumber = __LINE__; \
		UnitTrace::out("%s(%d): %s \n", _UnitData.pSourceFilePath, _UnitData.sourceLineNumber, _UnitData.pMemberName ); \
		printf("%s(%d): %s \n", _UnitData.pSourceFilePath, _UnitData.sourceLineNumber, _UnitData.pMemberName ); \
		/*this->teardown();*/ \
		return; \
	} \
	else\
	{\
	}\
}


#define CHECK_EQUAL( value1, value2 ) \
{ \
	_UnitStats.indvAsserts++;\
	if( !( (value1) == (value2) ) ) \
	{ \
		_UnitData.result = false;  \
		_UnitData.pMemberName = this->pName; \
		_UnitData.pSourceFilePath = __FILE__; \
		_UnitData.sourceLineNumber = __LINE__; \
		UnitTrace::out("%s(%d): %s\n", _UnitData.pSourceFilePath, _UnitData.sourceLineNumber, _UnitData.pMemberName  ); \
		printf("%s(%d): %s\n", _UnitData.pSourceFilePath, _UnitData.sourceLineNumber, _UnitData.pMemberName  ); \
		/*this->teardown();*/ \
		return; \
	} \
	else\
	{\
	}\
}

// Here is the magic
//
// 1) it creates a class, <TestName>_Test
//    it is derived from Test class
// 2) it defines the constructor and calls the base constructor with the name
//    it registers the name of the test to a single class (TestRegistry)
// 3) It also overloads the Test::run() method with the body of the macro

#define TEST( TestName, GroupName ) \
class TestName##_##GroupName##_Test : public Test \
{ \
	public: \
		TestName##_##GroupName##_Test(): \
		Test( STRING_ME(TestName##_##GroupName##_Test)) \
		{ \
		}; \
	\
	void run(UnitData &_UnitData, UnitStats &_UnitStats);\
} TestName##_##GroupName##_instance; \
\
void TestName##_##GroupName##_Test::run( UnitData &_UnitData, UnitStats &_UnitStats ) 


#define TEST_WITH_TEARDOWN( TestName, GroupName ) \
class TestName##_##GroupName##_Test : public Test \
{ \
	public: \
		TestName##_##GroupName##_Test(): \
		Test( STRING_ME(TestName##_##GroupName##_Test)) \
		{ \
		}; \
	\
	void run(UnitData &_UnitData, UnitStats &_UnitStats);\
	virtual void teardown();\
} TestName##_##GroupName##_instance; \
\
void TestName##_##GroupName##_Test::run( UnitData &_UnitData, UnitStats &_UnitStats ) 



#define TEST_TEARDOWN( TestName, GroupName ) \
void TestName##_##GroupName##_Test::teardown() 


#endif

// ---  End of File ---------------
