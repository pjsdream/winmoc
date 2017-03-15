#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Windows.h>
#include <strsafe.h>
#include <string>


// GetLastWriteTime - Retrieves the last-write time and converts
//                    the time to a string
//
// Return value - TRUE if successful, FALSE otherwise
// hFile      - Valid file handle
// lpszString - Pointer to buffer to receive string

BOOL GetLastWriteTime(HANDLE hFile, LPTSTR lpszString, DWORD dwSize)
{
    FILETIME ftCreate, ftAccess, ftWrite;
    SYSTEMTIME stUTC, stLocal;
    DWORD dwRet;

    // Retrieve the file times for the file.
    if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
        return FALSE;

    // Convert the last-write time to local time.
    FileTimeToSystemTime(&ftWrite, &stUTC);
    SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

    // Build a string showing the date and time.
    dwRet = StringCchPrintf(lpszString, dwSize, 
        TEXT("%04d%02d%02d%02d%02d%02d%03d"),
        stLocal.wYear, stLocal.wMonth, stLocal.wDay,
        stLocal.wHour, stLocal.wMinute, stLocal.wSecond, stLocal.wMilliseconds);

    if( S_OK == dwRet )
        return TRUE;
    else return FALSE;
}


int main(int argc, char** argv)
{
    if (argc != 4 ||
        strcmp(argv[2], "-o") != 0)
    {
        fprintf(stderr, "USAGE: winmoc.exe [input filename] -o [output filename]\n");
        exit(0);
    }

    HANDLE input_file;
    HANDLE output_file;
    TCHAR input_file_date_buffer[MAX_PATH];
    TCHAR output_file_date_buffer[MAX_PATH];
    std::string input_file_date;
    std::string output_file_date;

    std::string input_filename = std::string(argv[1]);
    std::string output_filename = std::string(argv[3]);

    input_file = CreateFile(std::wstring(input_filename.begin(), input_filename.end()).data(), GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, 0, NULL);

    if (input_file == INVALID_HANDLE_VALUE)
    {
        printf("ERROR: CreateFile for '%s' failed with %d\n", input_filename.c_str(), GetLastError());
        exit(0);
    }
    if (GetLastWriteTime(input_file, input_file_date_buffer, MAX_PATH))
    {
        std::wstring wdate(input_file_date_buffer);
        input_file_date = std::string(wdate.begin(), wdate.end());
    }
    else
    {
        printf("ERROR: Could not retrieve last modified date for file '%s' with error %d\n", input_filename.c_str(), GetLastError());
        exit(0);
    }
    CloseHandle(input_file);
    
    output_file = CreateFile(std::wstring(output_filename.begin(), output_filename.end()).data(), GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, 0, NULL);

    bool need_moc = false;

    if (output_file == INVALID_HANDLE_VALUE ||
        !GetLastWriteTime(output_file, output_file_date_buffer, MAX_PATH))
        need_moc = true;
    else
    {
        std::wstring wdate(output_file_date_buffer);
        output_file_date = std::string(wdate.begin(), wdate.end());

        if (input_file_date > output_file_date)
            need_moc = true;
    }

    if (output_file != INVALID_HANDLE_VALUE)
        CloseHandle(output_file);

    if (need_moc)
    {
        char command[1024];
        sprintf_s(command, "C:\\Qt\\5.7\\msvc2015_64\\bin\\moc.exe \"%s\" -o \"%s\"", input_filename.c_str(), output_filename.c_str());
        printf("%s\n", command);
        system(command);
    }
    else
        printf("File is not recently modified\n");

    return 0;
}
