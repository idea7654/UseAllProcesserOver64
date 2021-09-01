// ConsoleApplication1.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <thread>
#include <windows.h>
#include <WinBase.h>
#include <vector>
#include <sysinfoapi.h>
using namespace std;

void DistributeThreads(vector<thread> &m_threads);

volatile int g_num = 0;
void Foo()
{
	for (size_t i = 0; i < 10000000; ++i)
	{
		++g_num;
		--g_num;
	}
}
int main()
{
	vector<thread> threads;
	int threadSize = 200;
	threads.resize(threadSize);
	for (int i = 0; i < threadSize; ++i)
	{
		threads[i] = thread(Foo);
	}

	for (int i = 0; i < threadSize; ++i)
	{
		threads[i].join();
	}

	//cout << g_num << endl;
	
	DistributeThreads(threads);

	getchar();
}

void DistributeThreads(vector<thread> &m_threads)
{
	//!!BUG!! need to skip this code for old windows versions
	int nNumGroups = GetActiveProcessorGroupCount();
	cout << nNumGroups << endl;
	if (nNumGroups > 1)
	{
		//Log("System has %d processor groups", nNumGroups);
		for (int i = 0; i < nNumGroups; i++)
		{
			//Log();
			printf("group %d has %d processors \n", i, (int)GetMaximumProcessorCount(i));
		}
		printf("all processors %d \n", (int)GetMaximumProcessorCount(ALL_PROCESSOR_GROUPS));

		cout << thread::hardware_concurrency() << endl;

		int nCurGroup = 0;
		int nNumRemaining = GetMaximumProcessorCount(nCurGroup);
		for (int i = 0; i < m_threads.size(); i++)
		{
			auto hndl = m_threads[i].native_handle();
			GROUP_AFFINITY oldaffinity;
			if (GetThreadGroupAffinity(hndl, &oldaffinity))
			{
				//Log( "thread %d, old msk = %x, old grp = %llx", i, oldaffinity.Mask, oldaffinity.Group );
				GROUP_AFFINITY affinity;
				affinity = oldaffinity;
				if (affinity.Group != nCurGroup)
				{
					affinity.Group = nCurGroup;
					auto bSucc = SetThreadGroupAffinity(hndl, &affinity, nullptr);
					if (!bSucc)
					{
						//Log();
						printf("failed to set gr aff err=%x \n", (int)GetLastError());
					}
					else
					{
						//Log(  );
						printf("Set group for thread %d to %d \n", i, nCurGroup);
					}
					--nNumRemaining;
					if (nNumRemaining == 0)
					{
						nCurGroup = min(nCurGroup + 1, nNumGroups - 1);
						nNumRemaining = GetMaximumProcessorCount(nCurGroup);
					}
				}
			}
		}
	}
}