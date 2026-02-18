#include "headers/fileManager.h"
#include <windows.h>
#include <shobjidl.h>


std::string PWSTR_to_str(PWSTR w)
{
    int size = WideCharToMultiByte(CP_UTF8, 0, w, -1, nullptr, 0, nullptr, nullptr);
    std::string s(size - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, w, -1, s.data(), size, nullptr, nullptr);
    return s;
}



std::string browseFiles(void){
    std::string filePath;
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | 
        COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr))
    {
        IFileOpenDialog *pFileOpen;

        // Create the FileOpenDialog object.
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, 
                IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

        if (SUCCEEDED(hr))
        {
            // Show the Open dialog box.
            hr = pFileOpen->Show(NULL);

            // Get the file name from the dialog box.
            if (SUCCEEDED(hr))
            {
                IShellItem *pItem;
                hr = pFileOpen->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                    filePath = PWSTR_to_str(pszFilePath);
                    
                    pItem->Release();
                }  else { printf("File browser failed.\n"); }
            }  else { printf("File browser failed.\n"); }
            pFileOpen->Release();
        }  else { printf("File browser failed.\n"); }
        CoUninitialize();
    } else { printf("File browser failed.\n"); }

    return filePath;
}