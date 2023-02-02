#include <Windows.h>
#include <fstream>
#include <time.h>
#include <string>

#define ITERATIONS_N 10

using namespace std;

DWORD get_page_size()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	return si.dwPageSize;
}

int main()
{
	const DWORD page_size = get_page_size();
	const DWORD page_number = 0 + 3 + 0 + 5 + 0 + 5;
	const DWORD process_id = GetCurrentProcessId();
	const DWORD start_time = GetTickCount64();

	srand(process_id);

	
	fstream log;
	string log_name = "C:\\Users\\aikak\\Desktop\\Test\\OS_LAB4\\Logs\\writer_log" + to_string(process_id) + ".txt";
	log.open(log_name.c_str(), fstream::app);

	HANDLE mapping_object = OpenFileMapping(
		FILE_MAP_WRITE, 
		FALSE, 
		L"Local\\file"
	);
	void* view_of_file = MapViewOfFile(
		mapping_object,
		FILE_MAP_WRITE,
		0,
		0,
		page_number * page_size
	);
	VirtualLock(view_of_file, page_number * page_size);

	// Открытие семафоров
	HANDLE reader_sem[page_number], writer_sem[page_number];
	for (int i = 0; i < page_number; i++)
	{
		// семафор писатель
		wchar_t writer_name[] = L"writer ";
		writer_name[6] = 'a' + i;
		writer_sem[i] = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, FALSE, writer_name);

		// семафор читатель
		wchar_t reader_name[] = L"reader ";
		reader_name[6] = 'a' + i;
		reader_sem[i] = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, FALSE, reader_name);
	}
	
	// Начало записи
	for (int i = 0; i < ITERATIONS_N; i++)
	{
		int page = WaitForMultipleObjects(page_number, writer_sem, FALSE, INFINITE);
		
		log << process_id << "\t" << page << "\t" << "WRITE_START" << "\t" << GetTickCount64() - start_time << "\t" << GetTickCount64() << "\t" << start_time << "\n";
		char* buf = (char*)((char*)view_of_file + page * page_size);
		buf[0] = 'a' + i;
		buf[1] = '\0';
		Sleep(500 + rand() % 1000);
		log << process_id << "\t" << page << "\t" << "WRITE_END" << "\t" << GetTickCount64() - start_time << "\t" << GetTickCount64() << "\t" << start_time << "\n";
		
		ReleaseSemaphore(reader_sem[page], 1, NULL);
		log << process_id << "\t" << page << "\t" << "RSEM_RELEASED" << "\t" << GetTickCount64() - start_time << "\t" << GetTickCount64() << "\t" << start_time << "\n";
	}

	for (int i = 0; i < page_number; i++)
	{
		CloseHandle(reader_sem[i]);
		CloseHandle(writer_sem[i]);
	}
	VirtualUnlock(view_of_file, page_size);
	UnmapViewOfFile(view_of_file);
	log.close();
	return 0;
}