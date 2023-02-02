#include <Windows.h>
#include <iostream>


using namespace std;
int main()
{
	setlocale(LC_ALL, "ru");
	HANDLE pipe = NULL;
	OVERLAPPED overlapped;
	wstring pipe_name;
	pipe = CreateNamedPipe(
		L"\\\\.\\pipe\\a",
		PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,
		PIPE_TYPE_MESSAGE,
		2,
		MAX_PATH,
		MAX_PATH,
		0,
		NULL
	);
	ZeroMemory(&overlapped, sizeof(overlapped));
	overlapped.hEvent = CreateEvent(NULL, TRUE, NULL, NULL);
	ConnectNamedPipe(pipe, &overlapped);

	while (true)
	{
		string s;
		cin >> s;
		DWORD bytes = 0;
		WriteFile(pipe, s.c_str(), s.length(), &bytes, &overlapped);
		WaitForSingleObject(overlapped.hEvent, INFINITE);
		if (s != "END")
			cout << "Операция ввода-вывода завершена.\n";
		else
			break;
	}
	DisconnectNamedPipe(pipe);
	CloseHandle(pipe);
	CloseHandle(overlapped.hEvent);
	return 0;
}