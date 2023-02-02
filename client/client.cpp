#include <Windows.h>
#include <iostream>


using namespace std;

char buffer[MAX_PATH];

VOID WINAPI after_read(DWORD error, DWORD bytes, LPOVERLAPPED lpov)
{
	buffer[bytes] = '\0';
	if (error)
		std::cout << "Произошла ошибка: " << GetLastError() << "\n";
	else
		cout << "Чтение завершено. Прочитанная строка: " << buffer << "\n";
}

int main()
{
	setlocale(LC_ALL, "ru");
	HANDLE pipe;
	OVERLAPPED overlapped;
	ZeroMemory(&overlapped, sizeof(overlapped));

	pipe = CreateFile(
		L"\\\\.\\pipe\\a",
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,
		NULL
	);//ОТКРЫВАЕМ канал
	bool done = true;
	while (done)
	{
		ReadFileEx(pipe, buffer, MAX_PATH, &overlapped, after_read);
		SleepEx(INFINITE, TRUE);
	}
	CloseHandle(pipe);
	return 0;
}