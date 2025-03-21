#include "include/open_document/open_document_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>
#include <Windows.h>
#include <vector>
#include <Lmcons.h>
#include <sys/stat.h>
#include <ShellApi.h>
#include <stdio.h>
#include <stdlib.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <map>
#include <memory>
#include <sstream>
#include <optional>
#include <shlobj.h>

namespace
{

    class OpenDocumentPlugin : public flutter::Plugin
    {
    public:
        static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

        OpenDocumentPlugin();
        virtual ~OpenDocumentPlugin();

    private:
        // Called when a method is called on this plugin's channel from Dart.
        void HandleMethodCall(
            const flutter::MethodCall<flutter::EncodableValue> &method_call,
            std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
    };
    using flutter::EncodableMap;
    using flutter::EncodableValue;
    using namespace std;

    std::string CreateFolderDocuments(std::string& path_s) {
        if (CreateDirectoryA(path_s.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError())
        {
            return path_s;
        }
            return path_s;
    }

    std::string wide_string_to_string(const std::wstring &wide_string)
    {
        if (wide_string.empty())
        {
            return "";
        }

        const auto size_needed = WideCharToMultiByte(CP_UTF8, 0, &wide_string.at(0), (int)wide_string.size(), nullptr, 0, nullptr, nullptr);
        if (size_needed <= 0)
        {
            throw std::runtime_error("WideCharToMultiByte() failed: " + std::to_string(size_needed));
        }

        std::string result(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, &wide_string.at(0), (int)wide_string.size(), &result.at(0), size_needed, nullptr, nullptr);
        return result;
    }

    std::vector<std::string> split(const std::string &s, char delim)
    {
        std::vector<std::string> result;
        std::stringstream ss(s);
        std::string item;
        std::string value;

        while (getline(ss, item, delim))
        {
            result.push_back(item);
            value = item;
        }

        return result;
    }

    std::string GetNameFolder()
    {
        DWORD nSize = 200;
        TCHAR *outStr = new TCHAR[nSize];
        GetModuleFileName(NULL, outStr, nSize);
        std::string value = wide_string_to_string(outStr);
        delete[] outStr;
        std::string delimiter = ".exe";
        std::string path = value.substr(0, value.find(delimiter));
        std::vector<std::string> v = split(path, '\\');
        return v.back();
    }

    void OpenDocument(const char *PATH)
    {
        ShellExecuteA(GetDesktopWindow(), "open", PATH, NULL, NULL, SW_SHOW);
    }

    std::string GetArgument(const flutter::MethodCall<> &method_call)
    {
        const auto *arguments = std::get_if<flutter::EncodableMap>(method_call.arguments());
        auto vName = arguments->find(flutter::EncodableValue("path"));
        std::string path = std::get<std::string>(vName->second);
        std::cout << path;
        return path;
    }

    std::string GetPathDocument()
    {
       std::string nameFolder = GetNameFolder();
       TCHAR* my_documents = new TCHAR[300];
       HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);

       if (result != S_OK)
            std::cout << "Error: " << result << "\n";
       else
            std::cout << "Path: " << my_documents << "\n";

       std::string value = wide_string_to_string(my_documents);
       value += ("\\" + nameFolder);
       try {
          value = CreateFolderDocuments(value);
       }
       catch (...) {
           throw;
       }
       return value;
    }

      void CheckDocument(const std::string& path, unique_ptr<flutter::MethodResult<>>& result) {
       struct stat buffer;
       if(stat (path.c_str(), &buffer) == 0){
       result->Success(flutter::EncodableValue(true));
       }else{
       result->Success(flutter::EncodableValue(false));
       }
      }

    void OpenDocumentPlugin::RegisterWithRegistrar(
        flutter::PluginRegistrarWindows *registrar)
    {
        auto channel =
            std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
                registrar->messenger(), "open_document",
                &flutter::StandardMethodCodec::GetInstance());

        auto plugin = std::make_unique<OpenDocumentPlugin>();

        channel->SetMethodCallHandler(
            [plugin_pointer = plugin.get()](const auto &call, auto result)
            {
                plugin_pointer->HandleMethodCall(call, std::move(result));
            });

        registrar->AddPlugin(std::move(plugin));
    }

    OpenDocumentPlugin::OpenDocumentPlugin() {}

    OpenDocumentPlugin::~OpenDocumentPlugin() {}

    void OpenDocumentPlugin::HandleMethodCall(
        const flutter::MethodCall<flutter::EncodableValue> &method_call,
        std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result)
    {
        std::ostringstream value;

        if (method_call.method_name().compare("getNameFolder") == 0)
        {
            value << GetNameFolder().c_str();
            result->Success(flutter::EncodableValue(value.str()));
        }
        else if (method_call.method_name().compare("openDocument") == 0)
        {
            OpenDocument(GetArgument(method_call).c_str());
            result->Success();
        }
        else if (method_call.method_name().compare("getPathDocument") == 0)
        {
            value << GetPathDocument().c_str();
            result->Success(flutter::EncodableValue(value.str()));
        }
        else if (method_call.method_name().compare("checkDocument") == 0) {
             CheckDocument(GetArgument(method_call).c_str(), result);
        }
        else
        {
            result->NotImplemented();
        }
    }

} // namespace

void OpenDocumentPluginRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar)
{
    OpenDocumentPlugin::RegisterWithRegistrar(
        flutter::PluginRegistrarManager::GetInstance()
            ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
