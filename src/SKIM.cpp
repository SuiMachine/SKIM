// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include <string>
#include <memory>
#include <algorithm>

#include <time.h>

#include "system_tray.h"
#include "injection.h"

#include "Resource.h"

#include "APP_VERSION.H"
#include "ini.h"
#include "branch.h"
#include "network.h"
#include "SKIM.h"

#include <Windows.h>
#include <WindowsX.h>

HWND            hWndMainDlg;
HWND            hWndStatusBar;

HICON           hIconSKIM_LG;
HICON           hIconSKIM_SM;
HINSTANCE       g_hInstance;
HWND            hWndRestart                = nullptr;
bool            child                      = false;
wchar_t         startup_dir [MAX_PATH + 1] = { };

unsigned int
__stdcall
SKIM_FeatureUnsupportedMessage (LPVOID user);


enum {
  SKIM_APPID_TALES_OF_ZESTIRIA      = 351970,
  SKIM_APPID_FINAL_FANTASY_X_X2     = 359870,
  SKIM_APPID_TALES_OF_SYMPHONIA     = 372360,
  SKIM_APPID_DARKSOULS3             = 374320,
  SKIM_APPID_FALLOUT4               = 377160,
  SKIM_APPID_DISGAEA_PC             = 405900,
  SKIM_APPID_TALES_OF_BERSERIA      = 429660,
  SKIM_APPID_NIER_AUTOMATA          = 524220,
  SKIM_APPID_DOTHACK_GU             = 525480,
  SKIM_APPID_TALES_OF_BERSERIA_DEMO = 550350
};

#define SKIM_ProductDecl_32Bit(PrimaryDLL,PlugInDLL,PlugInName,GameName,\
ModName,ConfigTool,Repository,DonateID,AppID,Description)               \
  { (PrimaryDLL), (PlugInDLL), (PlugInName),                            \
    (GameName), (ModName),  (ConfigTool), (Repository),                 \
    (DonateID), (AppID), SK_32_BIT, false, (Description), 0             \
  }

#ifndef _WIN64
#define SKIM_ProductDecl_64Bit(PrimaryDLL,PlugInDLL,PlugInName,GameName,\
ModName,ConfigTool,Repository,DonateID,AppID,Description)               \
  { (PrimaryDLL), (PlugInDLL), (PlugInName),                            \
    L"", L"", L"", L"",                                                 \
    (DonateID), (AppID), SK_64_BIT, false, (Description), 0             \
  }
#else
#define SKIM_ProductDecl_64Bit(PrimaryDLL,PlugInDLL,PlugInName,GameName,\
ModName,ConfigTool,Repository,DonateID,AppID,Description)               \
  { (PrimaryDLL), (PlugInDLL), (PlugInName),                            \
    (GameName), (ModName),  (ConfigTool), (Repository),                 \
    (DonateID), (AppID), SK_64_BIT, false, (Description), 0             \
  }
#endif

#define SKIM_ProductDecl_AllBits(PrimaryDLL,PlugInDLL,PlugInName,GameName,\
ModName,ConfigTool,Repository,DonateID,AppID,Description)                 \
  { (PrimaryDLL), (PlugInDLL), (PlugInName),                              \
    (GameName), (ModName),  (ConfigTool), (Repository),                   \
    (DonateID), (AppID), SK_BOTH_BIT, false, (Description), 0             \
  }

#ifdef _WIN64
#define SKIM_TransientApplicationDecl SKIM_ProductDecl_AllBits
#else
#define SKIM_TransientApplicationDecl SKIM_ProductDecl_32Bit
#endif

sk_product_t products [] =
{
  // Special K (Global Injector)
  //
  {
#ifndef _WIN64
    L"SpecialK32.dll",
#else
    L"SpecialK64.dll",
#endif
    L"SpecialK32.dll", L"Special K", // DLL ProductName
    L"Special K",
    L"Special K (Global Injector)",
    L"",
    L"SpecialK/0.8.x",
    L"8A7FSUFJ6KB2U",
    0,
#ifdef _WIN64
    SK_BOTH_BIT,
#else
    SK_32_BIT,
#endif
    false,
    L"Applies Special K's non-game-specific features to all Steam games "
    L"launched on your system.\r\n\r\n"

    L"Includes Steam achievement unlock sound, "
    L"mouse cursor management and various framerate enhancements "
    L"for DDraw/D3D8/9/11/12/GL games.\r\n\r\n",
    0
  },


  // .hack//G.P.U.
  //
  SKIM_ProductDecl_64Bit ( L"dinput8.dll",

                           L"d3d11.dll",               L"ReShade",
                           L".hack//G.U. Last Recode", L".hack//G.P.U",

                           L"",

                           L"dGPU", L"8A7FSUFJ6KB2U",

                           SKIM_APPID_DOTHACK_GU,

    L"Fixes texture aliasing and streaming performance, adds support for"
    L" texture mods, fixes overexposed bloom / screen flares." ),



  // FAR
  //
  SKIM_ProductDecl_64Bit ( L"dinput8.dll",

                           L"ReShade64.dll",    L"ReShade",
                           L"NieR: Automata™", LR"("FAR" (Fix Automata Res.))",

                           L"",

                           L"FAR/dinput8", L"H6SDVFMHZVUR6",

                           SKIM_APPID_NIER_AUTOMATA,

    L"Fixes NieR: Automata™'s wonky resolution problems, Global Illumination"
    L" performance, post-processing image quality, FMV stuttering, resource"
    L" load hitches, mouse cursor problems, adds > 60 FPS mode and HUD free"
    L" screenshots." ),



  // Tales of Berseria "Fix"
  //
  SKIM_ProductDecl_64Bit ( L"d3d9.dll",

                           L"tbfix.dll",         LR"(Tales of Berseria "Fix")",
                           L"Tales of Berseria", LR"(Tales of Berseria "Fix")",

                           L"",

                           L"TBF", L"ALPEVA3UX74LL",

                           SKIM_APPID_TALES_OF_BERSERIA,

    L"Adds texture mod support, custom button icons, improves"
    L" Namco's framerate limiter, enhances shadow quality,"
    L" adds anti-aliasing and input remapping support." ),

  // Tales of Berseria "Fix"
  //                          ( For Demo Version )
  SKIM_ProductDecl_64Bit ( L"d3d9.dll",

                           L"tbfix.dll",                LR"(Tales of Berseria "Fix")",
                           L"Tales of Berseria (Demo)", LR"(Tales of Berseria "Fix")",

                           L"",

                           L"TBF", L"ALPEVA3UX74LL",

                           SKIM_APPID_TALES_OF_BERSERIA_DEMO,

    L"Adds texture mod support, custom button icons, improves"
    L" Namco's framerate limiter, enhances shadow quality,"
    L" adds anti-aliasing and input remapping support." ),



  // Tales of Zestiria "Fix"
  //
  SKIM_ProductDecl_32Bit ( L"d3d9.dll",

                           L"tzfix.dll",         LR"(Tales of Zestiria "Fix")",
                           L"Tales of Zestiria", LR"(Tales of Zestiria "Fix")",

                           L"tzt.exe",

                           L"TZF", L"X3AAFX8LJWNTU",

                           SKIM_APPID_TALES_OF_ZESTIRIA,

    L"Adds 60 FPS support, enhances shadow quality, fixes texture aliasing,"
    L" adds aspect ratio correction and fixes multi-channel / high"
    L" sample-rate audio stability." ),



  // Tales of Symphonia "Fix"
  //
  {
    L"d3d9.dll",
    L"tsfix.dll", L"Tales of Symphonia \"Fix\"", // DLL ProductName
    L"Tales of Symphonia",
    L"Tales of Symphonia \"Fix\"",
    L"",
    L"TSF",
    L"WNZ6CYRYN3NVJ",
    SKIM_APPID_TALES_OF_SYMPHONIA,
    SK_32_BIT,
    true,
    L"Adds MSAA, fixes Namco's framerate limiter, supports 4K textures, "
    L"fixes input-related problems.",
    0
  },



  // Fallout 4 "Works"
  //
  SKIM_ProductDecl_64Bit ( L"dxgi.dll",

                           L"",           L"",
                           L"Fallout 4", LR"(Fallout 4 "Works")",

                           L"",

                           L"FO4W", L"",

                           SKIM_APPID_FALLOUT4,

    L"Improves framepacing.\r\n\r\n"
    L"  (Use the Global Injector; Plug-In is built-in)" ),



  // Pretty Prinny
  //
  SKIM_ProductDecl_32Bit ( L"OpenGL32.dll",

                           L"PrettyPrinny.dll", L"Pretty Prinny",
                           L"Disgaea PC",       L"Pretty Prinny",

                           L"",

                           L"PrettyPrinny", L"UYL32Y8H4K5H2",

                           SKIM_APPID_DISGAEA_PC,

    L"Improves framepacing, optimizes post-processing, removes the 720p "
    L" resolution lock, adds borderless window and MSAA, supports custom "
    L" button icons." ),



  // Souls "Unsqueezed"
  //
  SKIM_ProductDecl_64Bit ( L"dxgi.dll",

                           L"",               L"Special K",
                           L"Dark Souls III", LR"(Souls "Unsqueezed")",

                           L"ds3t.exe",

                           L"SoulsUnsqueezed", L"L9FJSV8WXMWRU",

                           SKIM_APPID_DARKSOULS3,

    L"Adds support for non-16:9 aspect ratios, texture memory optimizations "
    L" and multi-monitor rendering.\r\n" ),


  // "Untitled" Project X
  //
  SKIM_ProductDecl_32Bit ( L"dxgi.dll",

                           L"UnX.dll",                L"Untitled Project X",
                           L"Final Fantasy X / X-2 "
                           L"HD Remaster",           LR"("Untitled" Project X)",

                           L"UnX_Calibrate.exe",

                           L"UnX", L"6TDLFVRGXLU92",

                           SKIM_APPID_FINAL_FANTASY_X_X2,

    L"Adds dual-audio support, texture modding, cutscene skipping in FFX,"
    L" cursor management, Intel GPU bypass, fullscreen exclusive mode,"
    L" maps all PC-specific extra features to gamepad.\n" )
};

void
SKIM_Exit (void)
{
  SKIM_Tray_RemoveFrom (                          );
  TerminateProcess     (GetCurrentProcess (), 0x00);
  ExitProcess          (                      0x00);
}

const wchar_t*
SKIM_SteamUtil_GetInstallDir (void)
{
         DWORD   len         = MAX_PATH;
  static wchar_t wszSteamPath [MAX_PATH + 2];

  LSTATUS status =
    RegGetValueW ( HKEY_CURRENT_USER,
                     L"SOFTWARE\\Valve\\Steam\\",
                       L"SteamPath",
                         RRF_RT_REG_SZ,
                           nullptr,
                             wszSteamPath,
                               (LPDWORD)&len );

  if (status == ERROR_SUCCESS)
    return wszSteamPath;
  else
    return nullptr;
}

size_t
SKIM_Util_DeleteTemporaryFiles (const wchar_t* wszPath, const wchar_t* wszPattern)
{
  WIN32_FIND_DATA fd;
  HANDLE          hFind  = INVALID_HANDLE_VALUE;
  size_t          files  = 0UL;
  LARGE_INTEGER   liSize = { 0ULL };

  wchar_t wszFindPattern [MAX_PATH * 2] = { };

  lstrcatW (wszFindPattern, wszPath);
  lstrcatW (wszFindPattern, L"\\");
  lstrcatW (wszFindPattern, wszPattern);

  hFind = FindFirstFileW (wszFindPattern, &fd);

  if (hFind != INVALID_HANDLE_VALUE)
  {
    wchar_t wszFullPath [MAX_PATH * 2 + 1] = { };

    do
    {
      if (fd.dwFileAttributes != INVALID_FILE_ATTRIBUTES)
      {
        *wszFullPath = L'\0';

        lstrcatW (wszFullPath, wszPath);
        lstrcatW (wszFullPath, L"\\");
        lstrcatW (wszFullPath, fd.cFileName);

        if (DeleteFileW (wszFullPath))
          ++files;
      }
    } while (FindNextFileW (hFind, &fd) != 0);
  }

  return files;
}
bool
SKIM_Util_CreateDirectories ( const wchar_t* wszPath )
{
  wchar_t* wszSubDir        = _wcsdup (wszPath), *iter;

  wchar_t* wszLastSlash     = wcsrchr (wszSubDir, L'/');
  wchar_t* wszLastBackslash = wcsrchr (wszSubDir, L'\\');

  if (wszLastSlash > wszLastBackslash)
    *wszLastSlash = L'\0';
  else if (wszLastBackslash != nullptr)
    *wszLastBackslash = L'\0';
  else
  {
    free (wszSubDir);
    return false;
  }

  for (iter = wszSubDir; *iter != L'\0'; iter = CharNextW (iter))
  {
    if (*iter == L'\\' || *iter == L'/')
    {
      *iter = L'\0';

      if (GetFileAttributes (wszPath) == INVALID_FILE_ATTRIBUTES)
        CreateDirectoryW (wszSubDir, nullptr);

      *iter = L'\\';
    }
  }

  // The final subdirectory (FULL PATH)
  if (GetFileAttributes (wszPath) == INVALID_FILE_ATTRIBUTES)
    CreateDirectoryW (wszSubDir, nullptr);

  free (wszSubDir);

  return true;
}

void
SKIM_StripTrailingSlashes (wchar_t* wszInOut)
{
  struct test_slashes
  {
    bool operator () (wchar_t a, wchar_t b) const
    {
      auto IsSlash = [](wchar_t a) -> bool {
        return (a == L'\\' || a == L'/');
      };

      return IsSlash (a) && IsSlash (b);
    }
  };
  
  std::wstring wstr (wszInOut);
  
  wstr.erase ( std::unique ( wstr.begin (),
                             wstr.end   (), test_slashes () ),
                 wstr.end () );

  wcscpy (wszInOut, wstr.c_str ());
}

void
SKIM_FixSlashes (wchar_t* wszInOut)
{ 
  std::wstring wstr (wszInOut);

  for ( auto& it : wstr )
    if (it == L'/')
      it = L'\\';

  wcscpy (wszInOut, wstr.c_str ());
}

std::vector <sk_product_t *>
SKIM_GetInstallableProducts (void)
{
  std::vector <sk_product_t *> prods;

  for (auto& product : products)
  {
    int state =
      SKIM_DetermineInstallState (product);

    if (state != -1)
    {
      std::wstring path (SKIM_FindInstallPath (product.uiSteamAppID));

      if (path.empty ())
      {
        continue;
      }

      else if (! wcscmp (L"<Invalid>", path.c_str ()))
      {
        continue;
      }

      prods.push_back (&product);
    }
  }

  return prods;
}

const wchar_t*
SKIM_FindInstallPath (uint32_t appid)
{
  // DarkSouls 3's install path has additional baggage ;)
  bool ds3 = (appid == SKIM_APPID_DARKSOULS3);

  wchar_t wszAppend [MAX_PATH + 2] = { };

  if (ds3)
    lstrcatW (wszAppend, L"\\Game");


  static wchar_t wszGamePath [MAX_PATH + 2] = { };
                *wszGamePath                = '\0';

  // Special Case: AppID 0 = Special K
  if (appid == 0)
  {
    uint32_t dwLen = MAX_PATH;

    SKIM_Util_GetDocumentsDir (wszGamePath, &dwLen);

    PathAppend (wszGamePath, L"My Mods\\SpecialK\\");

    return wszGamePath;
  }

  using steam_library_t = char* [MAX_PATH + 2];

  static bool scanned_libs = false;

#define MAX_STEAM_LIBRARIES 16
  static int             steam_libs = 0;
  static steam_library_t steam_lib_paths [MAX_STEAM_LIBRARIES] = { };

  static const wchar_t* wszSteamPath;

  if (! scanned_libs)
  {
    wszSteamPath =
      SKIM_SteamUtil_GetInstallDir ();

    if (wszSteamPath != nullptr)
    {
      wchar_t wszLibraryFolders [MAX_PATH * 2 + 1] = { };

      lstrcpyW   (wszLibraryFolders, wszSteamPath);
      PathAppend (wszLibraryFolders, L"steamapps\\libraryfolders.vdf");

      CHandle hLibFolders (
        CreateFileW ( wszLibraryFolders,
                        GENERIC_READ,
                          FILE_SHARE_READ | FILE_SHARE_WRITE,
                            nullptr,
                              OPEN_EXISTING,
                                GetFileAttributesW (wszLibraryFolders),
                                  nullptr ) );

      if (hLibFolders != INVALID_HANDLE_VALUE)
      {
        DWORD  dwSize     = 0,
               dwSizeHigh = 0,
               dwRead     = 0;

        dwSize =
          GetFileSize (hLibFolders, &dwSizeHigh);

        void* data =
          new uint8_t [dwSize + 1] { };

        if (data == nullptr)
        {
          return nullptr;
        }

        std::unique_ptr <uint8_t> _data ((uint8_t *)data);

        dwRead = dwSize;

        if (ReadFile (hLibFolders, data, dwSize, &dwRead, nullptr))
        {
          for (DWORD i = 0; i < dwSize; i++)
          {
            if (((const char *)data) [i] == '"' && i < dwSize - 3)
            {
              if (((const char *)data) [i + 2] == '"')
                i += 2;
              else if (((const char *)data) [i + 3] == '"')
                i += 3;
              else
                continue;

              char* lib_start = nullptr;

              for (DWORD j = i; j < dwSize; j++,i++)
              {
                if (((char *)data) [j] == '"' && lib_start == nullptr && j < dwSize - 1)
                {
                  lib_start = &((char *)data) [j+1];
                }

                else if (((char *)data) [j] == '"')
                {
                  ((char *)data) [j] = '\0';
                  lstrcpyA ((char *)steam_lib_paths [steam_libs++], lib_start);
                  lib_start = nullptr;
                }
              }
            }
          }
        }
      }

      sprintf ((char *)steam_lib_paths [steam_libs++], "%ws", wszSteamPath);
    }

    scanned_libs = true;
  }


  // If we encounter a manifest with invalid values, flag this
  bool invalid = false;


  // Search custom library paths first
  if (steam_libs != 0)
  {
    for (int i = 0; i < steam_libs; i++)
    {
      char szManifest [MAX_PATH * 2] = { };

      sprintf ( szManifest,
                  R"(%s\steamapps\appmanifest_%d.acf)",
                    (char *)steam_lib_paths [i],
                      appid );

      CHandle hManifest (
        CreateFileA ( szManifest,
                      GENERIC_READ,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                          nullptr,
                            OPEN_EXISTING,
                              GetFileAttributesA (szManifest),
                                nullptr ) );

      if (hManifest != INVALID_HANDLE_VALUE)
      {
        DWORD  dwSize     = 0,
               dwSizeHigh = 0,
               dwRead     = 0;

        dwSize =
          GetFileSize (hManifest, &dwSizeHigh);

        auto* szManifestData =
          new char [dwSize + 1] { };

        if (! szManifestData)
          continue;

        std::unique_ptr <char> manifest_data (szManifestData);

        bool bRead =
          ReadFile ( hManifest,
                       szManifestData,
                         dwSize,
                           &dwRead,
                             nullptr );

        if (! (bRead && dwRead))
        {
          continue;
        }

        char* szInstallDir =
          StrStrIA (szManifestData, R"("installdir")");

        char szGamePath [MAX_PATH * 2] = { };

        if (szInstallDir != nullptr)
        {
          // Make sure everything is lowercase
          strncpy (szInstallDir, R"("installdir")", strlen (R"("installdir")"));

          sscanf ( szInstallDir,
                     R"("installdir" "%518[^"]")",
                       szGamePath );
        }

        wsprintf ( wszGamePath,
                     L"%hs\\steamapps\\common\\%hs%s",
                       (char *)steam_lib_paths [i],
                         szGamePath, wszAppend );

        SKIM_StripTrailingSlashes (wszGamePath);
        SKIM_FixSlashes           (wszGamePath);
        PathRemoveBackslashW      (wszGamePath);

        if (SKIM_Util_IsDirectory (wszGamePath))
        {
          return wszGamePath;
        }

        else
          invalid = true;
      }
    }
  }

  if (invalid)
    return L"<Invalid>";

  return L"";
}

bool
__stdcall
SKIM_IsDLLFromProduct (const wchar_t* wszName, const wchar_t* wszProductName)
{
  UINT     cbTranslatedBytes = 0,
           cbProductBytes    = 0;

  uint8_t  cbData      [4096] = { };
  wchar_t  wszPropName [64]   = { };

  wchar_t* wszProduct = nullptr; // Will point somewhere in cbData

  struct LANGANDCODEPAGE {
    WORD wLanguage;
    WORD wCodePage;
  } *lpTranslate = nullptr;

  wchar_t wszFullyQualifiedName [MAX_PATH * 2] = { };

  lstrcatW (wszFullyQualifiedName, wszName);

  if (GetFileAttributes (wszFullyQualifiedName) == INVALID_FILE_ATTRIBUTES)
    return false;

  GetFileVersionInfoExW ( FILE_VER_GET_NEUTRAL | FILE_VER_GET_PREFETCHED,
                            wszFullyQualifiedName,
                              0x00,
                                4096,
                                  cbData );

  if (VerQueryValueW ( cbData,
                         TEXT ("\\VarFileInfo\\Translation"),
                           (LPVOID *)&lpTranslate,
                                     &cbTranslatedBytes ) && cbTranslatedBytes)
  {
    wsprintfW ( wszPropName,
                  L"\\StringFileInfo\\%04x%04x\\ProductName",
                    lpTranslate [0].wLanguage,
                      lpTranslate [0].wCodePage );

    VerQueryValueW ( cbData,
                       wszPropName,
                         (LPVOID *)&wszProduct,
                                   &cbProductBytes );

    return (cbProductBytes && (! wcscmp (wszProduct, wszProductName)));
  }

  return false;
}

bool
SKIM_Util_IsDirectory (const wchar_t* wszPath)
{
  DWORD dwAttrib = 
    GetFileAttributes (wszPath);

  if ( dwAttrib != INVALID_FILE_ATTRIBUTES &&
       dwAttrib  & FILE_ATTRIBUTE_DIRECTORY )
    return true;

  return false;
}

bool
SKIM_Util_IsAdmin (void)
{
  bool   bRet   = false;
  HANDLE hToken = nullptr;

  if (OpenProcessToken (GetCurrentProcess (), TOKEN_QUERY, &hToken)) {
    TOKEN_ELEVATION Elevation;
    DWORD cbSize = sizeof (TOKEN_ELEVATION);

    if (GetTokenInformation (hToken, TokenElevation, &Elevation, sizeof (Elevation), &cbSize)) {
      bRet = Elevation.TokenIsElevated != 0;
    }
  }

  if (hToken)
    CloseHandle (hToken);

  return bRet;
}

bool
SKIM_Util_IsProcessRunning (const wchar_t* wszProcName)
{
  HANDLE         hProcSnap;
  PROCESSENTRY32 pe32;

  hProcSnap =
    CreateToolhelp32Snapshot (TH32CS_SNAPPROCESS, 0);

  if (hProcSnap == INVALID_HANDLE_VALUE)
    return false;

  pe32.dwSize = sizeof PROCESSENTRY32;

  if (! Process32First (hProcSnap, &pe32)) {
    CloseHandle (hProcSnap);
    return false;
  }

  do {
    if (! _wcsicmp (wszProcName, pe32.szExeFile)) {
      CloseHandle (hProcSnap);
      return true;
    }
  } while (Process32Next (hProcSnap, &pe32));

  CloseHandle (hProcSnap);

  return false;
}


bool
SKIM_Util_GetDocumentsDir (wchar_t* buf, uint32_t* pdwLen)
{
  CComHeapPtr <wchar_t> str;
  CHandle               hToken;

  if (! OpenProcessToken (GetCurrentProcess (), TOKEN_READ, &hToken.m_h))
    return false;

  if ( SUCCEEDED (
         SHGetKnownFolderPath (
           FOLDERID_Documents, 0, hToken, &str
         )
       )
     )
  {
    if (buf != nullptr && pdwLen != nullptr && *pdwLen > 0) {
      wcsncpy (buf, str, *pdwLen);
    }

    return true;
  }

  return false;
}



#include <MMSystem.h>

void
SKIM_Util_MoveFileNoFail ( const wchar_t* wszOld, const wchar_t* wszNew )
{
  if (! MoveFileExW ( wszOld,
                        wszNew,
                          MOVEFILE_REPLACE_EXISTING ) )
  {
    wchar_t wszPath [MAX_PATH * 2] = { };
    wcscpy (wszPath, wszNew);

    PathRemoveFileSpec (wszPath);

    wchar_t wszTemp [MAX_PATH * 2] = { };
    GetTempFileNameW (wszPath, L"SKI", timeGetTime (), wszTemp);

    MoveFileExW ( wszNew, wszTemp, MOVEFILE_REPLACE_EXISTING );
    MoveFileExW ( wszOld, wszNew,  MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED);
  }
}






bool
SKIM_Depends_TestKB2533623 (SK_ARCHITECTURE arch)
{
  UNREFERENCED_PARAMETER (arch);

  return GetProcAddress (GetModuleHandle (L"kernel32.dll"), "AddDllDirectory") != nullptr;
}

#include <Msi.h>

bool
SKIM_Depends_TestVisualCRuntime (SK_ARCHITECTURE arch)
{
  if (arch == SK_64_BIT)
  {
    const wchar_t* vcredist2015_upgradecode_x64 =
      L"{36F68A90-239C-34DF-B58C-64B30153CE35}";

    wchar_t        vcredist2015_any_productcode_x64 [39] =
        { L"\0" };

    if ( MsiEnumRelatedProductsW (
           vcredist2015_upgradecode_x64,
             0x00,
               0,
                 vcredist2015_any_productcode_x64 ) == ERROR_SUCCESS ) {
      return lstrlenW (vcredist2015_any_productcode_x64) &&
              MsiQueryProductStateW (vcredist2015_any_productcode_x64) ==
                INSTALLSTATE_DEFAULT;
    }

    return false;
  }

  else if (arch == SK_32_BIT)
  {
    const wchar_t* vcredist2015_upgradecode =
      L"{65E5BD06-6392-3027-8C26-853107D3CF1A}";

    wchar_t vcredist2015_any_productcode [39] =
        { L"\0" };

    if ( MsiEnumRelatedProductsW (
           vcredist2015_upgradecode,
             0x00,
               0,
                 vcredist2015_any_productcode ) == ERROR_SUCCESS ) {
      return lstrlenW (vcredist2015_any_productcode) &&
              MsiQueryProductStateW (vcredist2015_any_productcode) ==
                INSTALLSTATE_DEFAULT;
    }

    return false;
  }

  else {/* if (arch == SK_BOTH_BIT) {*/
    return SKIM_Depends_TestVisualCRuntime (SK_32_BIT) &&
           SKIM_Depends_TestVisualCRuntime (SK_64_BIT);
  }
}

void
SKIM_DeleteConfigFiles (sk_product_t* product)
{
  if (product->uiSteamAppID == 0)
    return;

  wchar_t wszWrapper [MAX_PATH * 2];
  wchar_t wszPlugIn  [MAX_PATH * 2];

  wchar_t wszInstallPath [MAX_PATH * 2];
  wcsncpy (wszInstallPath,
    SKIM_FindInstallPath (product->uiSteamAppID),
           MAX_PATH
  );
  wcsncat (wszInstallPath, L"\\", MAX_PATH);

  wcsncpy (wszWrapper, product->wszWrapper, MAX_PATH);
  wcsncpy (wszPlugIn,  product->wszPlugIn,  MAX_PATH);

  bool has_plugin = wcsnlen (wszPlugIn, MAX_PATH) > 0;

  *wcsrchr (wszWrapper, L'.') = L'\0';

  wchar_t wszWrapperINI [MAX_PATH * 2] = { };
  wcscat (wszWrapperINI, wszInstallPath);
  wcscat (wszWrapperINI, wszWrapper);
  wcscat (wszWrapperINI, L".ini");

  DeleteFileW (wszWrapperINI);

  wchar_t wszPlugInINI [MAX_PATH * 2] = { };

  if (has_plugin)
  {
    *wcsrchr (wszPlugIn,  L'.') = L'\0';

    wcsncat (wszPlugInINI, wszInstallPath, MAX_PATH);
    wcsncat (wszPlugInINI, wszPlugIn,      MAX_PATH);
    wcsncat (wszPlugInINI, L".ini",        MAX_PATH);

    DeleteFileW (wszPlugInINI);
                *wszPlugInINI  = L'\0';

    wcsncat (wszPlugInINI, wszInstallPath, MAX_PATH);
    wcsncat (wszPlugInINI, L"default_",    MAX_PATH);
    wcsncat (wszPlugInINI, wszPlugIn,      MAX_PATH);
    wcsncat (wszPlugInINI, L".ini",        MAX_PATH);

    DeleteFileW (wszPlugInINI);
                *wszPlugInINI  = L'\0';

    wcsncat (wszPlugInINI, wszInstallPath, MAX_PATH);
    wcsncat (wszPlugInINI, wszPlugIn,      MAX_PATH);
    wcsncat (wszPlugInINI, L".pdb",        MAX_PATH);

    DeleteFileW (wszPlugInINI);
  }

  *wszWrapperINI = L'\0';

  wcsncat (wszWrapperINI, wszInstallPath, MAX_PATH);
  wcsncat (wszWrapperINI, L"default_",    MAX_PATH);
  wcsncat (wszWrapperINI, wszWrapper,     MAX_PATH);
  wcsncat (wszWrapperINI, L".ini",        MAX_PATH);

  DeleteFileW (wszWrapperINI);
              *wszWrapperINI = L'\0';

  wcsncat (wszWrapperINI, wszInstallPath, MAX_PATH);
  wcsncat (wszWrapperINI, wszWrapper,     MAX_PATH);
  wcsncat (wszWrapperINI, L".pdb",        MAX_PATH);

  DeleteFileW (wszWrapperINI);

  *wszWrapperINI = L'\0';
  *wszPlugInINI  = L'\0';

  wcsncat (wszPlugInINI, wszInstallPath,    MAX_PATH);
  wcsncat (wszPlugInINI, L"SpecialK32.pdb", MAX_PATH);

  wcsncat (wszWrapperINI, wszInstallPath,    MAX_PATH);
  wcsncat (wszWrapperINI, L"SpecialK64.pdb", MAX_PATH);

  DeleteFileW (wszWrapperINI);
  DeleteFileW (wszPlugInINI);
}

unsigned int
WINAPI
SKIM_UninstallProduct (LPVOID user)
{
  auto* product = (sk_product_t *)user;

  HWND parent_dlg = hWndMainDlg;

  ShowWindow (parent_dlg, SW_HIDE);

  if (product->uiSteamAppID == 0) {
    SKIM_DisableGlobalInjector ();
  }

  SetCurrentDirectory (SKIM_FindInstallPath (product->uiSteamAppID));

  SKIM_DeleteConfigFiles (product);

  DeleteFileW (L"Version\\installed.ini");
  DeleteFileW (L"Version\\repository.ini");

  DeleteFileW (product->wszWrapper);
  DeleteFileW (product->wszPlugIn);

  wchar_t wszUninstall [256];
  wsprintf ( wszUninstall, L"%s has been successfully uninstalled.",
               product->wszProjectName );


  MessageBox ( nullptr,
                 wszUninstall,
                   L"Uninstall Success",
                     MB_OK | MB_ICONINFORMATION | MB_SETFOREGROUND );

  ShowWindow          (parent_dlg, SW_SHOW);
  SetForegroundWindow (parent_dlg);
  SendMessage         (parent_dlg, WM_INITDIALOG, 0x00, 0x00);

  if (child)
  {
    SKIM_Exit ();
  }

  SKIM_BranchManager::singleton ()->setProduct ((uint32_t)-1);
  SKIM_OnProductSelect          ();
  SKIM_BranchManager::singleton ()->setProduct ((uint32_t)-1);
  SKIM_OnBranchSelect           ();

  CloseHandle (GetCurrentThread ());

  return 0;
}

#include <time.h>

DWORD
SKIM_CountProductBranches (sk_product_t *pProduct)
{
  SKIM_BranchManager::singleton ()->setProduct (pProduct->uiSteamAppID);

  return SKIM_BranchManager::singleton ()->getNumberOfBranches ();
}

SKIM_BranchManager::Branch*
SKIM_GetProductBranchByIdx (sk_product_t *pProduct, int idx)
{
  SKIM_BranchManager::singleton ()->setProduct (pProduct->uiSteamAppID);

  return SKIM_BranchManager::singleton ()->getBranchByIndex (idx);
}



unsigned int
__stdcall
SKIM_MigrateProduct (LPVOID user)//sk_product_t* pProduct)
{
  auto* product = (sk_product_t *)user;

  // Disable installation of FO4W
  if ( product->uiSteamAppID == SKIM_APPID_FALLOUT4 )
  {
    CloseHandle (GetCurrentThread ());
    return 0;
  }

  if (! SKIM_Depends_TestKB2533623 (SK_BOTH_BIT) )
  {
    int               nButtonPressed =   0;
    TASKDIALOGCONFIG  config         = { };

    config.cbSize             = sizeof (config);
    config.hInstance          = g_hInstance;
    config.hwndParent         = GetActiveWindow ();
    config.pszWindowTitle     = L"Special K Install Manager (Fatal Error)";
    config.dwCommonButtons    = 0;
    config.pszMainInstruction = L"Missing KB2533623 Update for Windows 7";
    config.pButtons           = nullptr;
    config.cButtons           = 0;

    config.pfCallback         = 
      []( _In_ HWND     hWnd,
          _In_ UINT     uNotification,
          _In_ WPARAM   wParam,
          _In_ LPARAM   lParam,
          _In_ LONG_PTR dwRefData ) ->
      HRESULT
      {
        UNREFERENCED_PARAMETER (dwRefData);
        UNREFERENCED_PARAMETER (wParam);

        if (uNotification == TDN_HYPERLINK_CLICKED)
        {
          ShellExecuteW ( hWnd,
                            L"OPEN",
                              (wchar_t *)lParam,
                                nullptr, nullptr,
                                  SW_SHOWMAXIMIZED );

          SKIM_Exit ();
        }

        return S_FALSE;
      };

    config.dwFlags           |= TDF_ENABLE_HYPERLINKS;
    config.pszMainIcon        = TD_ERROR_ICON;

    config.pszContent       =
      SKIM_RunLHIfBitness ( 32, L"Please grab the x86"
                                L" version from <a href=\""
                                L"https://www.microsoft.com/en-us/"
                                L"download/details.aspx?id=26767\">here</a>"
                                L" to continue.",

                                // 64-Bit
                                L"Please grab the x64"
                                L" version from <a href=\""
                                L"https://www.microsoft.com/en-us/"
                                L"download/details.aspx?id=267647\">here</a>"
                                L" to continue." );

    TaskDialogIndirect (&config, &nButtonPressed, nullptr, nullptr);
    return 0;
  }

  if (! SKIM_Depends_TestVisualCRuntime (SK_BOTH_BIT) )
  {
    int               nButtonPressed = 0;
    TASKDIALOGCONFIG  config         = {0};

    config.cbSize             = sizeof (config);
    config.hInstance          = g_hInstance;
    config.hwndParent         = GetActiveWindow ();
    config.pszWindowTitle     = L"Special K Install Manager (Fatal Error)";
    config.dwCommonButtons    = 0;
    config.pszMainInstruction = L"Missing Visual C++ 2015 Runtime";
    config.pButtons           = nullptr;
    config.cButtons           = 0;

    config.pfCallback         = 
      []( _In_ HWND     hWnd,
          _In_ UINT     uNotification,
          _In_ WPARAM   wParam,
          _In_ LPARAM   lParam,
          _In_ LONG_PTR dwRefData ) ->
      HRESULT
      {
        UNREFERENCED_PARAMETER (dwRefData);
        UNREFERENCED_PARAMETER (wParam);

        if (uNotification == TDN_HYPERLINK_CLICKED)
        {
          ShellExecuteW ( hWnd,
                            L"OPEN",
                              (wchar_t *)lParam,
                                nullptr, nullptr,
                                  SW_SHOWMAXIMIZED );

          SKIM_Exit ();
        }

        return S_FALSE;
      };

    config.dwFlags           |= TDF_ENABLE_HYPERLINKS;

    config.pszMainIcon        = TD_ERROR_ICON;

    config.pszContent         = L"Please grab _BOTH_, the x86 and x64 "
                                L"versions from <a href=\""
                                L"https://go.microsoft.com/fwlink/?LinkId=746571"
                                L"\">here (x86)</a> and <a href=\""
                                L"https://go.microsoft.com/fwlink/?LinkId=746572"
                                L"\">here (x64)</a> to continue.";

    TaskDialogIndirect (&config, &nButtonPressed, nullptr, nullptr);

    return 0;
  }

  wchar_t wszInstallPath [MAX_PATH] = { };

  wcscpy ( wszInstallPath,
            SKIM_FindInstallPath (product->uiSteamAppID) );

  wchar_t wszAppID      [MAX_PATH * 2] = { };
  wchar_t wszExecutable [MAX_PATH    ] = { };

  GetModuleFileName (GetModuleHandle (nullptr), wszExecutable, MAX_PATH);

  wsprintf (wszAppID, L"\"%ws\" %lu", wszExecutable, product->uiSteamAppID);

  if ( product->uiSteamAppID != 0 )
  {
    if ( GetFileAttributes (L"steam_api.dll")   == INVALID_FILE_ATTRIBUTES &&
         GetFileAttributes (L"steam_api64.dll") == INVALID_FILE_ATTRIBUTES )
    {
      wchar_t wszDeployedSteamAPI   [MAX_PATH + 2] = { };
      wchar_t wszDeployedSteamAPI64 [MAX_PATH + 2] = { };

      lstrcatW   (wszDeployedSteamAPI, wszInstallPath);
      PathAppend (wszDeployedSteamAPI, L"steam_api.dll");

      lstrcatW   (wszDeployedSteamAPI64, wszInstallPath);
      PathAppend (wszDeployedSteamAPI64, L"steam_api64.dll");

      if ( GetFileAttributes (wszDeployedSteamAPI)   != INVALID_FILE_ATTRIBUTES ||
           GetFileAttributes (wszDeployedSteamAPI64) != INVALID_FILE_ATTRIBUTES )
      {
        STARTUPINFO         sinfo = { };
        PROCESS_INFORMATION pinfo = { };

        sinfo.cb          = sizeof STARTUPINFO;
        sinfo.dwFlags     = STARTF_USESHOWWINDOW;
        sinfo.wShowWindow = SW_HIDE;

        wchar_t wszRepoINI    [MAX_PATH + 2] = { };

        _snwprintf ( wszRepoINI,
                       MAX_PATH,
                         L"%s\\Version\\repository.ini",
                           wszInstallPath );

        SKIM_DeleteConfigFiles (product);

        DeleteFileW (wszRepoINI);

        CreateProcess ( wszExecutable,
                          wszAppID,
                            nullptr, nullptr,
                              FALSE, DETACHED_PROCESS,
                                nullptr, wszInstallPath,
                                  &sinfo, &pinfo );

        if (hWndMainDlg != nullptr)
        {
          ShowWindow (hWndMainDlg, SW_HIDE);
        }

        if (WaitForSingleObject ( pinfo.hProcess, INFINITE ) == WAIT_OBJECT_0)
        {
          CloseHandle (pinfo.hThread);
          CloseHandle (pinfo.hProcess);
        }

        if (! child)
        {
          ShowWindow          (hWndMainDlg, SW_SHOW);
          SetForegroundWindow (hWndMainDlg);
          SendMessage         (hWndMainDlg, WM_INITDIALOG, 0x00, 0x00);
        }

        else
        {
          SendMessage         (hWndMainDlg, WM_CLOSE, 0x00, 0x00);
          SendMessage         (hWndMainDlg, WM_QUIT,  0x00, 0x00);
        }

        return 1;
      }

      else
      {
        wchar_t wszErrorMsg [512] = { };

        wsprintf ( wszErrorMsg,
                      L"Unable to locate a valid install path (steam_api{64}.dll is missing)!\r\n\r\n"
                      L"Game is supposed to be installed to '%s', but is not.",
                        SKIM_FindInstallPath (product->uiSteamAppID) );

        MessageBox ( nullptr,
                      wszErrorMsg,
                        L"Special K Installer Fatal Error",
                          MB_ICONHAND | MB_OK | MB_APPLMODAL | MB_SETFOREGROUND );
      }
    }
  }

  SetCurrentDirectory (
    SKIM_FindInstallPath (product->uiSteamAppID)
  );

  ShowWindow (hWndMainDlg, SW_HIDE);

  wchar_t wszInstallerDLL [MAX_PATH + 2] = { };
  _snwprintf ( wszInstallerDLL,
                 MAX_PATH,
                   L"%s\\%s",
                     wszInstallPath, product->wszWrapper );

  HMODULE hModInstaller =
    LoadLibrary (wszInstallerDLL);

  if (hModInstaller != nullptr)
  {
    wchar_t wszRepoINI      [MAX_PATH + 2] = { };

    _snwprintf ( wszRepoINI,
                   MAX_PATH,
                     L"%s\\Version\\repository.ini",
                       wszInstallPath );

    DeleteFileW (wszRepoINI);

    using SK_UpdateSoftware_pfn = HRESULT (__stdcall *)(const wchar_t* wszProduct);
    using SK_FetchVersionInfo_pfn = bool    (__stdcall *)(const wchar_t* wszProduct);

    auto SK_UpdateSoftware =
      (SK_UpdateSoftware_pfn)
        GetProcAddress ( hModInstaller,
                          "SK_UpdateSoftware" );

    auto SK_FetchVersionInfo =
      (SK_FetchVersionInfo_pfn)
        GetProcAddress ( hModInstaller,
                        "SK_FetchVersionInfo" );

    if ( SK_FetchVersionInfo != nullptr &&
         SK_UpdateSoftware   != nullptr )
    {
      if (SK_FetchVersionInfo (product->wszRepoName))
          SK_UpdateSoftware   (product->wszRepoName);
      else
      {
        MessageBoxW (nullptr, L"Failed to fetch version info from GitHub", L"Download Failure", MB_ICONERROR);
      }
    }
  }

  //FreeLibrary (hModInstaller);

  if (! child)
  {
    ShowWindow          (hWndMainDlg, SW_SHOW);
    SetForegroundWindow (hWndMainDlg);
    SendMessage         (hWndMainDlg, WM_INITDIALOG, 0x00, 0x00);
  }

  else
  {
    SendMessage         (hWndMainDlg, WM_CLOSE, 0x00, 0x00);
    SendMessage         (hWndMainDlg, WM_QUIT,  0x00, 0x00);
  }

  SKIM_BranchManager::singleton ()->setProduct ((uint32_t)-1);
  SKIM_OnProductSelect          ();
  SKIM_BranchManager::singleton ()->setProduct ((uint32_t)-1);
  SKIM_OnBranchSelect           ();


  return 0;
}


unsigned int
__stdcall
SKIM_InstallProduct (LPVOID user)//sk_product_t* pProduct)
{
  static int tries = 0;

  auto* product = (sk_product_t *)user;

  // Disable installation of FO4W
  if ( product->uiSteamAppID == SKIM_APPID_FALLOUT4 ) {
    CloseHandle (GetCurrentThread ());
    return 0;
  }

  if (! SKIM_Depends_TestKB2533623 (SK_BOTH_BIT) )
  {
    int               nButtonPressed = 0;
    TASKDIALOGCONFIG  config         = {0};

    config.cbSize             = sizeof (config);
    config.hInstance          = g_hInstance;
    config.hwndParent         = GetActiveWindow ();
    config.pszWindowTitle     = L"Special K Install Manager (Fatal Error)";
    config.dwCommonButtons    = 0;
    config.pszMainInstruction = L"Missing KB2533623 Update for Windows 7";
    config.pButtons           = nullptr;
    config.cButtons           = 0;

    config.pfCallback         = 
      []( _In_ HWND     hWnd,
          _In_ UINT     uNotification,
          _In_ WPARAM   wParam,
          _In_ LPARAM   lParam,
          _In_ LONG_PTR dwRefData ) ->
      HRESULT
      {
        UNREFERENCED_PARAMETER (dwRefData);
        UNREFERENCED_PARAMETER (wParam);

        if (uNotification == TDN_HYPERLINK_CLICKED) {
          ShellExecuteW ( hWnd,
                            L"OPEN",
                              (wchar_t *)lParam,
                                nullptr, nullptr,
                                  SW_SHOWMAXIMIZED );

          SKIM_Exit ();
        }

        return S_FALSE;
      };

    config.dwFlags           |= TDF_ENABLE_HYPERLINKS;

    config.pszMainIcon        = TD_ERROR_ICON;

    config.pszContent       =
      SKIM_RunLHIfBitness ( 32, L"Please grab the x86"
                                L" version from <a href=\""
                                L"https://www.microsoft.com/en-us/"
                                L"download/details.aspx?id=26767\">here</a>"
                                L" to continue.",

                                // 64-Bit
                                L"Please grab the x64"
                                L" version from <a href=\""
                                L"https://www.microsoft.com/en-us/"
                                L"download/details.aspx?id=267647\">here</a>"
                                L" to continue." );

    TaskDialogIndirect (&config, &nButtonPressed, nullptr, nullptr);
    return 0;
  }

  SK_ARCHITECTURE arch = SK_64_BIT;

  if (product->architecture == SK_32_BIT)
    arch = SK_BOTH_BIT;

  if (! SKIM_Depends_TestVisualCRuntime (arch) )
  {
    int               nButtonPressed = 0;
    TASKDIALOGCONFIG  config         = {0};

    config.cbSize             = sizeof (config);
    config.hInstance          = g_hInstance;
    config.hwndParent         = GetActiveWindow ();
    config.pszWindowTitle     = L"Special K Install Manager (Fatal Error)";
    config.dwCommonButtons    = 0;
    config.pszMainInstruction = L"Missing Visual C++ 2015 Runtime";
    config.pButtons           = nullptr;
    config.cButtons           = 0;

    config.pfCallback         = 
      []( _In_ HWND     hWnd,
          _In_ UINT     uNotification,
          _In_ WPARAM   wParam,
          _In_ LPARAM   lParam,
          _In_ LONG_PTR dwRefData ) ->
      HRESULT
      {
        UNREFERENCED_PARAMETER (dwRefData);
        UNREFERENCED_PARAMETER (wParam);

        if (uNotification == TDN_HYPERLINK_CLICKED)
        {
          ShellExecuteW ( hWnd,
                            L"OPEN",
                              (wchar_t *)lParam,
                                nullptr, nullptr,
                                  SW_SHOWMAXIMIZED );

          SKIM_Exit ();
        }

        return S_FALSE;
      };

    config.dwFlags           |= TDF_ENABLE_HYPERLINKS;

    config.pszMainIcon        = TD_ERROR_ICON;

    if (arch == SK_BOTH_BIT)
    {
      config.pszContent       = L"Please grab _BOTH_, the x86 and x64 "
                                L"versions from <a href=\""
                                L"https://go.microsoft.com/fwlink/?LinkId=746571"
                                L"\">here (x86)</a> and <a href=\""
                                L"https://go.microsoft.com/fwlink/?LinkId=746572"
                                L"\">here (x64)</a> to continue.";
    }

    else if (arch == SK_32_BIT)
    {
      config.pszContent       = L"Please grab the x86"
                                L" version from <a href=\""
                                L"https://go.microsoft.com/fwlink/?LinkId=746571"
                                L"\">here</a>"
                                L" to continue.";
    }

    else if (arch == SK_64_BIT)
    {
      config.pszContent       = L"Please grab the x64"
                                L" version from <a href=\""
                                L"https://go.microsoft.com/fwlink/?LinkId=746572"
                                L"\">here</a>"
                                L" to continue.";
    }

    TaskDialogIndirect (&config, &nButtonPressed, nullptr, nullptr);
    return 0;
  }

  wchar_t wszInstallPath [MAX_PATH + 2] = { };

  wcsncpy ( wszInstallPath,
              SKIM_FindInstallPath (product->uiSteamAppID),
                MAX_PATH );

  wchar_t   wszVersionPath [MAX_PATH + 2] = { };
  wcsncpy  (wszVersionPath, wszInstallPath, MAX_PATH);
  lstrcatW (wszVersionPath, L"\\Version\\");

  SKIM_Util_CreateDirectories (wszVersionPath);

  wchar_t wszAppID      [MAX_PATH * 2] = { };
  wchar_t wszExecutable [MAX_PATH + 2] = { };

  GetModuleFileName (GetModuleHandle (nullptr), wszExecutable, MAX_PATH );

  wsprintf (wszAppID, L"\"%ws\" %lu", wszExecutable, product->uiSteamAppID);

  if ( product->uiSteamAppID != 0 )
  {
    if ( GetFileAttributes (L"steam_api.dll")   == INVALID_FILE_ATTRIBUTES &&
         GetFileAttributes (L"steam_api64.dll") == INVALID_FILE_ATTRIBUTES )
    {
      wchar_t wszDeployedSteamAPI   [MAX_PATH + 2] = { };
      wchar_t wszDeployedSteamAPI64 [MAX_PATH + 2] = { };

      lstrcatW   (wszDeployedSteamAPI,   wszInstallPath);
      PathAppend (wszDeployedSteamAPI, L"steam_api.dll");

      lstrcatW   (wszDeployedSteamAPI64,     wszInstallPath);
      PathAppend (wszDeployedSteamAPI64, L"steam_api64.dll");

      if ( GetFileAttributes (wszDeployedSteamAPI)   != INVALID_FILE_ATTRIBUTES ||
           GetFileAttributes (wszDeployedSteamAPI64) != INVALID_FILE_ATTRIBUTES )
      {
        STARTUPINFO         sinfo = { };
        PROCESS_INFORMATION pinfo = { };

        sinfo.cb          = sizeof STARTUPINFO;
        sinfo.dwFlags     = STARTF_USESHOWWINDOW;
        sinfo.wShowWindow = SW_HIDE;

        wchar_t wszInstallerINI [MAX_PATH + 2] = { };
        wchar_t wszRepoINI      [MAX_PATH + 2] = { };

        _snwprintf ( wszInstallerINI,
                       MAX_PATH,
                         L"%s\\Version\\installed.ini",
                           wszInstallPath );

        DeleteFileW (wszInstallerINI);
 
        _snwprintf ( wszRepoINI,
                       MAX_PATH,
                         L"%s\\Version\\repository.ini",
                           wszInstallPath );

        DeleteFileW (wszRepoINI);

        SKIM_DeleteConfigFiles (product);

        CreateProcess ( wszExecutable,
                          wszAppID,
                            nullptr, nullptr,
                              FALSE, DETACHED_PROCESS,
                                nullptr, wszInstallPath,
                                  &sinfo, &pinfo );

        if (hWndMainDlg != nullptr) {
          ShowWindow (hWndMainDlg, SW_HIDE);
        }

        if (WaitForSingleObject ( pinfo.hProcess, INFINITE ) == WAIT_OBJECT_0) {
          CloseHandle (pinfo.hThread);
          CloseHandle (pinfo.hProcess);
        }

        if (! child) {
          ShowWindow          (hWndMainDlg, SW_SHOW);
          SetForegroundWindow (hWndMainDlg);
          SendMessage         (hWndMainDlg, WM_INITDIALOG, 0x00, 0x00);
        } else {
          SendMessage         (hWndMainDlg, WM_CLOSE, 0x00, 0x00);
          SendMessage         (hWndMainDlg, WM_QUIT,  0x00, 0x00);
        }

        SKIM_BranchManager::singleton ()->setProduct ((uint32_t)-1);
        SKIM_OnProductSelect          ();
        SKIM_BranchManager::singleton ()->setProduct ((uint32_t)-1);
        SKIM_OnBranchSelect           ();

        return 1;
      }

      else
      {
        wchar_t wszErrorMsg [512] = { };

        wsprintf ( wszErrorMsg,
                      L"Unable to locate a valid install path (steam_api{64}.dll is missing)!\r\n\r\n"
                      L"Game is supposed to be installed to '%s', but is not.",
                        SKIM_FindInstallPath (product->uiSteamAppID) );

        MessageBox ( nullptr,
                      wszErrorMsg,
                        L"Special K Installer Fatal Error",
                          MB_ICONHAND | MB_OK | MB_APPLMODAL | MB_SETFOREGROUND );
      }
    }
  }

  SetCurrentDirectory (
    SKIM_FindInstallPath (product->uiSteamAppID)
  );

  ShowWindow (hWndMainDlg, SW_HIDE);

  bool bValidInstaller = 
    SKIM_FetchInstallerDLL (*product);

  wchar_t wszInstallerDLL [MAX_PATH + 2] = { };
  wchar_t wszRepoINI      [MAX_PATH + 2] = { };

  _snwprintf ( wszInstallerDLL,
                 MAX_PATH,
                   L"%s\\%s",
                     wszInstallPath, product->wszWrapper );

  _snwprintf ( wszRepoINI,
                 MAX_PATH,
                   L"%s\\Version\\repository.ini",
                     wszInstallPath );

  HMODULE hModInstaller =
    bValidInstaller ? LoadLibrary (wszInstallerDLL) : nullptr;

  DeleteFileW (wszRepoINI);

  SKIM_DeleteConfigFiles (product);


  if (hModInstaller != nullptr)
  {
    using SK_UpdateSoftware_pfn   = HRESULT (__stdcall *)(const wchar_t* wszProduct);
    using SK_FetchVersionInfo_pfn = bool    (__stdcall *)(const wchar_t* wszProduct);

    auto SK_UpdateSoftware =
      (SK_UpdateSoftware_pfn)
        GetProcAddress ( hModInstaller,
                          "SK_UpdateSoftware" );

    auto SK_FetchVersionInfo =
      (SK_FetchVersionInfo_pfn)
        GetProcAddress ( hModInstaller,
                        "SK_FetchVersionInfo" );

    if ( SK_FetchVersionInfo != nullptr &&
         SK_UpdateSoftware   != nullptr )
    {
      if (SK_FetchVersionInfo (product->wszRepoName))
      {
        if (FAILED (SK_UpdateSoftware (product->wszRepoName)))
        {
        //FreeLibrary (hModInstaller);
        //             hModInstaller = nullptr;
          DeleteFileW (wszInstallerDLL);
        }
      }

      else
      {
        MessageBoxW (nullptr, L"Failed to fetch version info from GitHub", L"Download Failure", MB_ICONERROR);
      }
    }
  }

  // Give a second try, in case Internet cache went kaput
  else if (tries == 0)
  {
    ++tries;
    DeleteFileW                (wszInstallerDLL);
    return SKIM_InstallProduct (user);
  }
  

  // Too many tries, give up.
  else
  {
    tries = 0;
    DeleteFileW (wszInstallerDLL);
    MessageBoxW (nullptr, L"Something went wrong attempting to download Installer DLL.", L"Install Failed", MB_OK | MB_ICONERROR);
  }

//if (hModInstaller != nullptr) FreeLibrary (hModInstaller);


  if (! child)
  {
    SKIM_BranchManager::singleton ()->setProduct ((uint32_t)-1);
    SKIM_OnProductSelect          ();
    SKIM_BranchManager::singleton ()->setProduct ((uint32_t)-1);
    SKIM_OnBranchSelect           ();

    ShowWindow          (hWndMainDlg, SW_SHOW);
    SetForegroundWindow (hWndMainDlg);
    SendMessage         (hWndMainDlg, WM_INITDIALOG, 0x00, 0x00);
  }

  else
  {
    SendMessage         (hWndMainDlg, WM_CLOSE, 0x00, 0x00);
    SendMessage         (hWndMainDlg, WM_QUIT,  0x00, 0x00);
  }

  SKIM_BranchManager::singleton ()->setProduct ((uint32_t)-1);
  SKIM_OnProductSelect          ();
  SKIM_BranchManager::singleton ()->setProduct ((uint32_t)-1);
  SKIM_OnBranchSelect           ();

  return 0;
}



unsigned int
__stdcall
SKIM_UpdateProduct (LPVOID user)
{
  auto* product = (sk_product_t *)user;

  static int tries = 0;

  // Disable installation of FO4W
  if ( product->uiSteamAppID == SKIM_APPID_FALLOUT4 ) {
    CloseHandle (GetCurrentThread ());
    return 0;
  }

  wchar_t wszInstallPath     [MAX_PATH + 2] = { };
  wchar_t wszVersionPath     [MAX_PATH + 2] = { };

  wchar_t wszInstallerDLL    [MAX_PATH + 2] = { };
  wchar_t wszInstallerBackup [MAX_PATH + 2] = { };

  wchar_t wszRepoINI         [MAX_PATH + 2] = { };
  wchar_t wszRepoBackup      [MAX_PATH + 2] = { };

  wcsncpy ( wszInstallPath,
              SKIM_FindInstallPath (product->uiSteamAppID),
                MAX_PATH );

  wcsncpy  (wszVersionPath, wszInstallPath, MAX_PATH);
  lstrcatW (wszVersionPath, L"\\Version\\");

  SKIM_Util_CreateDirectories (wszVersionPath);

  SetCurrentDirectory (
    SKIM_FindInstallPath (product->uiSteamAppID)
  );

  ShowWindow (hWndMainDlg, SW_HIDE);

  _snwprintf ( wszInstallerDLL,
                 MAX_PATH,
                   L"%s\\%s",
                     wszInstallPath, product->wszWrapper );

  _snwprintf ( wszInstallerBackup,
                 MAX_PATH,
                   L"%s\\Version\\%s.old",
                     wszInstallPath, product->wszWrapper );

  if (GetFileAttributes (wszInstallPath) != INVALID_FILE_ATTRIBUTES)
  {
    MoveFileW (wszInstallerDLL, wszInstallerBackup);
  }

  bool bValidInstaller = 
    LoadLibrary (wszInstallerBackup);

  if (! bValidInstaller)
    SKIM_FetchInstallerDLL (*product);

  HMODULE hModInstaller =
    bValidInstaller ? GetModuleHandle (wszInstallerBackup) :
                      LoadLibrary     (wszInstallerDLL);

  _snwprintf ( wszRepoINI,
                 MAX_PATH,
                   L"%s\\Version\\repository.ini",
                     wszInstallPath );

  _snwprintf ( wszRepoBackup,
                 MAX_PATH,
                   L"%s\\Version\\repository.ini.old",
                     wszInstallPath );

  SKIM_Util_MoveFileNoFail (wszRepoINI, wszRepoBackup);


  if (hModInstaller != nullptr)
  {
    using SK_UpdateSoftware_pfn   = HRESULT (__stdcall *)(const wchar_t* wszProduct);
    using SK_FetchVersionInfo_pfn = bool    (__stdcall *)(const wchar_t* wszProduct);

    auto SK_UpdateSoftware =
      (SK_UpdateSoftware_pfn)
        GetProcAddress ( hModInstaller,
                          "SK_UpdateSoftware" );

    auto SK_FetchVersionInfo =
      (SK_FetchVersionInfo_pfn)
        GetProcAddress ( hModInstaller,
                        "SK_FetchVersionInfo" );

    if ( SK_FetchVersionInfo != nullptr &&
         SK_UpdateSoftware   != nullptr )
    {
      bool update_available = false;

      if (SK_FetchVersionInfo (product->wszRepoName))
      {
        if (S_OK == SK_UpdateSoftware (product->wszRepoName))
        {
          update_available = true;
        }
      }

      else
      {
        MessageBoxW (nullptr, L"Failed to fetch version info from GitHub", L"Download Failure", MB_ICONERROR);
      }

      if (! update_available)
      {
        SKIM_Util_MoveFileNoFail (wszInstallerBackup, wszInstallerDLL);
        SKIM_Util_MoveFileNoFail (wszRepoBackup,      wszRepoINI);
      }
    }
  }

  //if (hModInstaller != GetModuleHandle (wszInstallerBackup))
  //  FreeLibrary (GetModuleHandle (wszInstallerBackup));
  //
  //FreeLibrary (hModInstaller);

  SKIM_BranchManager::singleton ()->setProduct ((uint32_t)-1);
  SKIM_OnProductSelect          ();
  SKIM_BranchManager::singleton ()->setProduct ((uint32_t)-1);
  SKIM_OnBranchSelect           ();

  ShowWindow          (hWndMainDlg, SW_SHOW);
  SetForegroundWindow (hWndMainDlg);
  SendMessage         (hWndMainDlg, WM_INITDIALOG, 0x00, 0x00);

  SKIM_BranchManager::singleton ()->setProduct ((uint32_t)-1);
  SKIM_OnProductSelect          ();
  SKIM_BranchManager::singleton ()->setProduct ((uint32_t)-1);
  SKIM_OnBranchSelect           ();

  CloseHandle (GetCurrentThread ());

  return 0;
}


bool
SKIM_DetermineInstallState (sk_product_t& product)
{
  // Special Case: SpecialK
  if (! _wcsnicmp (product.wszWrapper, SKIM_RunLHIfBitness ( 32, L"SpecialK32.dll",
                                                                 L"SpecialK64.dll" ), MAX_PATH) )
  {
             uint32_t dwLen = MAX_PATH * 2 - 1;
    wchar_t wszSpecialK_ROOT [MAX_PATH * 2] = { };

    SKIM_Util_GetDocumentsDir (wszSpecialK_ROOT, &dwLen);

    PathAppend (wszSpecialK_ROOT, L"My Mods\\SpecialK");
    PathAppend (wszSpecialK_ROOT, product.wszWrapper);

    if (GetFileAttributes (wszSpecialK_ROOT) != INVALID_FILE_ATTRIBUTES)
    {
      product.install_state =
        SKIM_IsDLLFromProduct (product.wszWrapper, product.wszDLLProductName) ?
          1 : 0;
      return true;
    }

    else {
      product.install_state = 0;
      return false;
    }
  }


  wchar_t wszFileToTest [MAX_PATH + 2] = { };

  if (lstrlenW (product.wszPlugIn))
  {
    _snwprintf ( wszFileToTest,
                   MAX_PATH,
                     L"%s\\%s",
                       SKIM_FindInstallPath (product.uiSteamAppID),
                         product.wszPlugIn );
  }

  else
  {
    _snwprintf ( wszFileToTest,
                   MAX_PATH,
                     L"%s\\%s",
                       SKIM_FindInstallPath (product.uiSteamAppID),
                         product.wszWrapper );
  }


  const wchar_t* wszInstallPath =
    SKIM_FindInstallPath (product.uiSteamAppID);

  if (wszInstallPath == nullptr || (! wcslen (wszInstallPath)))
  {
    product.install_state = -1;
    return false;
  }

  if (! SKIM_Util_IsDirectory (SKIM_FindInstallPath (product.uiSteamAppID)))
  {
    product.install_state = -1;
    return false;
  }

  else
  {
    if (GetFileAttributes (wszFileToTest) != INVALID_FILE_ATTRIBUTES)
    {
      product.install_state =
        SKIM_IsDLLFromProduct (wszFileToTest, product.wszDLLProductName) ?
          1 : 0;

      return true;
    }

    product.install_state = 0;
    return false;
  }
}

std::wstring
SKIM_SummarizeRenderAPI (sk_product_t& product)
{
  std::wstring ret = L"";

  if (! _wcsicmp (product.wszWrapper, L"d3d9.dll"))
    ret += L"Direct3D 9";

  else if (! _wcsicmp (product.wszWrapper, L"dxgi.dll"))
  {
     if (! _wcsicmp (product.wszRepoName, L"UnX"))
      ret += L"Direct3D 11";
    else if ( (! _wcsicmp (product.wszRepoName, L"FO4W")) ||
              (! _wcsicmp (product.wszRepoName, L"SoulsUnsqueezed")) ||
              (! _wcsicmp (product.wszRepoName, L"FAR")) )
      ret += L"Direct3D 11";
    else
      ret += L"Direct3D 10/11/12?";
  }
  else if (! _wcsicmp (product.wszWrapper, L"OpenGL32.dll"))
    ret += L"OpenGL";
#ifndef _WIN64
  else if (! _wcsicmp (product.wszWrapper, L"SpecialK32.dll"))
#else
  else if (! _wcsicmp (product.wszWrapper, L"SpecialK64.dll"))
#endif
    ret += L"GL/D3D9/11/12/Vulkan";
  else if ( (! _wcsicmp (product.wszRepoName, L"FAR/dinput8")) ||
            (! _wcsicmp (product.wszRepoName, L"dGPU")) )
      ret += L"Direct3D 11";

  if (product.architecture == SK_32_BIT)
    ret += L" (32-bit)";
  else if (product.architecture == SK_64_BIT)
    ret += L" (64-bit)";
  else if (product.architecture == SK_BOTH_BIT)
    ret += L" (32-/64-bit)";

  return ret;
}

int last_sel = 0;

RECT
SKIM_GetHWNDRect (HWND hWnd)
{
  RECT   out;

  GetWindowRect (hWnd, &out);

  return out;
}

RECT
SKIM_GetClientRect (HWND hWnd)
{
  RECT   out;

  GetClientRect (hWnd, &out);

  return out;
}

RECT
SKIM_GetDlgItemRect (HWND hDlg, UINT nIDDlgItem)
{
  return
    SKIM_GetHWNDRect (
      GetDlgItem ( hDlg, nIDDlgItem )
    );
}

void
SKIM_OnBranchSelect (void)
{
  HWND hWndProducts =
    GetDlgItem (hWndMainDlg, IDC_PRODUCT_SELECT);

  int sel = std::max (0, ComboBox_GetCurSel (hWndProducts));

  static HWND
       hWndBranchSelect =
            GetDlgItem (hWndMainDlg, IDC_BRANCH_SELECT),
        hWndBranchFrame =
            GetDlgItem (hWndMainDlg, IDC_BRANCH_FRAME),
         hWndBranchDesc =
            GetDlgItem (hWndMainDlg, IDC_BRANCH_DESC),
       hWndBrachMigrate =
            GetDlgItem (hWndMainDlg, IDC_MIGRATE_CMD);

  int  count         = SKIM_CountProductBranches (&products [sel]);

  int  user_idx      = ComboBox_GetCurSel (hWndBranchSelect);
  int  installed_idx = user_idx;
  bool empty         = ComboBox_GetCount  (hWndBranchSelect) == 0;

  if (empty)
  {
    user_idx      = 0;
    installed_idx = 0;
  }

  if (count > 1)
  {
    SKIM_BranchManager::Branch* pBranch = nullptr;

    for (int i = 0; i < count; i++)
    {
      pBranch = SKIM_GetProductBranchByIdx ( &products [sel], i );

      if (! pBranch)
        continue;

      if (pBranch->name == SKIM_BranchManager::singleton ()->getCurrentBranch ()->name)
        installed_idx = i;

      if (empty)
      {
        ComboBox_InsertString ( hWndBranchSelect,
                                  i,
                                    pBranch->name.c_str ()
        );
      }
    }

    if (empty)
      user_idx = installed_idx;

    ComboBox_SetCurSel (hWndBranchSelect, user_idx);

    EnableWindow (hWndBranchSelect, TRUE);
    EnableWindow ( hWndBranchFrame, TRUE);
    EnableWindow (  hWndBranchDesc, TRUE);

    EnableWindow (hWndBrachMigrate, installed_idx != user_idx);

    std::wstring branch_desc =
      SKIM_GetProductBranchByIdx ( &products [sel], user_idx )->description;

    SetWindowText (hWndBranchDesc, branch_desc.c_str () );
  }

  else if (count <= 1) {
    ComboBox_ResetContent (hWndBranchSelect);
    ComboBox_InsertString (hWndBranchSelect, 0, L"Main");
    ComboBox_SetCurSel    (hWndBranchSelect, 0);

    SetWindowText (hWndBranchDesc, L"Default branch; fewer updates but stable.");

    EnableWindow (hWndBranchSelect, FALSE);
    EnableWindow ( hWndBranchFrame, FALSE);
    EnableWindow (  hWndBranchDesc, FALSE);
    EnableWindow (hWndBrachMigrate, FALSE);
  }

  if ((! SKIM_DetermineInstallState (products [sel])) || count < 1)
  {
    SendMessage  ( hWndStatusBar,
                    SB_SETTEXT,
                      LOBYTE (0) | HIBYTE (0),
                        (! SKIM_DetermineInstallState (products [sel])) ?
                          (LPARAM)L"\tMod Not Installed" :
                          (LPARAM)L"\tMod Not Installed  -  (Run game once)" );

    EnableWindow (hWndBrachMigrate, FALSE);

    std::wstring path (SKIM_FindInstallPath (products [sel].uiSteamAppID));

    if (path.empty ())
    {
      SendMessage  ( hWndStatusBar,
                      SB_SETTEXT,
                        LOBYTE (0) | HIBYTE (0),
                          (LPARAM)L"\tGame Not Installed" );
    }

    else if (! wcscmp (L"<Invalid>", path.c_str ()))
    {
      SendMessage  ( hWndStatusBar,
                      SB_SETTEXT,
                        LOBYTE (0) | HIBYTE (0),
                          (LPARAM)L"\tInvalid Steam Manifest" );
    }
  }

  else
  {
    wchar_t wszStatus [256];

    wsprintf ( wszStatus,
                 L"\t%s        (%s)",
                  SKIM_BranchManager::singleton ()->getInstallPackage ().c_str (),
                    SKIM_GetProductBranchByIdx (
                      &products [sel],
                        installed_idx
                    )->name.c_str ()
             );

    SendMessage ( hWndStatusBar,
                    SB_SETTEXT,
                      LOBYTE (0) | HIBYTE (0),
                        (LPARAM)wszStatus );
  }
}

HWND hWndRichProductDescrip;

void
SKIM_OnProductSelect (void)
{
  HWND hWndProducts =
    GetDlgItem (hWndMainDlg, IDC_PRODUCT_SELECT);

  int sel = std::max (0, ComboBox_GetCurSel (hWndProducts));

  last_sel = sel;

  Static_SetText (
    GetDlgItem (hWndMainDlg, IDC_NAME_OF_GAME),
      products [sel].wszGameName
  );

  Static_SetText (
    GetDlgItem (hWndMainDlg, IDC_API_SUMMARY),
      SKIM_SummarizeRenderAPI (products [sel]).c_str ()
  );

  CHARFORMAT2W cf2             = {                 };
               cf2.cbSize      = sizeof CHARFORMAT2W;
               cf2.crTextColor = RGB (0, 0, 0);
               cf2.dwMask      = CFM_COLOR;

  SendMessage (hWndRichProductDescrip, EM_SETSEL,        0,             -1);
  SendMessage (hWndRichProductDescrip, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf2);
  SendMessage (hWndRichProductDescrip, EM_REPLACESEL,    FALSE,         (LPARAM)products [sel].wszDescription);

  //HICON btn_icon = 0;

  HWND hWndManage =
    GetDlgItem (hWndMainDlg, IDC_MANAGE_CMD);

  // Easy way to detect Special K
  if (products [sel].uiSteamAppID == 0)
  {
    SetWindowTextA ( hWndManage, SKIM_GetInjectorState () ? "Stop Injecting" : "Start Injecting" );
  }

  else
    SetWindowTextA ( hWndManage, "Manage/Repair" );


  HWND hWndInstall =
    GetDlgItem (hWndMainDlg, IDC_INSTALL_CMD);

  HWND hWndUninstall =
    GetDlgItem (hWndMainDlg, IDC_UNINSTALL_CMD);


  if (products [sel].install_state < 0)
  {
    Button_Enable (hWndInstall,   0);
    Button_Enable (hWndManage,    0);
    Button_Enable (hWndUninstall, 0);
  }


  else if (products [sel].install_state == 0)
  {
    Button_Enable (hWndInstall,   1);
    Button_Enable (hWndManage,    0);
    Button_Enable (hWndUninstall, 0);
  }


  else if (products [sel].install_state == 1)
  {
    Button_Enable (hWndInstall,   0);
    Button_Enable (hWndUninstall, 1);

    // Tales of Symphonia (has DLC)
    if (products [sel].uiSteamAppID == SKIM_APPID_TALES_OF_SYMPHONIA)
      Button_Enable (hWndManage, 1);
    else if (products [sel].uiSteamAppID == SKIM_APPID_DARKSOULS3)
      Button_Enable (hWndManage, 1);
    else if (products [sel].uiSteamAppID == SKIM_APPID_TALES_OF_ZESTIRIA)
      Button_Enable (hWndManage, 1);
    else
      Button_Enable (hWndManage, 0);
  }

  if (products [sel].uiSteamAppID == 0 && products [sel].install_state == 1)
  {
    Button_Enable (hWndManage, 1);
  }

  static HWND
       hWndBranchSelect =
            GetDlgItem (hWndMainDlg, IDC_BRANCH_SELECT);

  ComboBox_ResetContent (hWndBranchSelect);
  SKIM_OnBranchSelect ();

  void
  SKIM_Tray_ResetMenu (void);
  SKIM_Tray_ResetMenu ();

  SKIM_Tray_Init          (hWndMainDlg);
  SKIM_Tray_UpdateProduct (&products [sel]);
}

bool
SKIM_ConfirmClose (void)
{
  if (SKIM_GetInjectorState ())
  {
    int               nButtonPressed =  0;
    TASKDIALOGCONFIG  config         = { };
  
    config.cbSize             = sizeof (config);
    config.hInstance          = g_hInstance;
    config.hwndParent         = GetActiveWindow ();
    config.pszWindowTitle     = L"Special K Install Manager";
    config.dwCommonButtons    = TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON;
    config.pszMainInstruction = L"Closing SKIM Will Stop Injection";
    config.pButtons           = nullptr;
    config.cButtons           = 0;
    config.nDefaultButton     = IDCANCEL;
  
    config.dwFlags            = /*TDF_SIZE_TO_CONTENT*/0x00;
    config.pszMainIcon        = TD_WARNING_ICON;
  
    config.pszContent         = L"Any software using Special K will continue, "
                                L"however, injection into new applications will cease.";

    config.pszFooter          = L"Minimizing SKIM will remove it from the task bar.";
    //config.pszFooterIcon      = L"SKIM64.exe:1";
  
    TaskDialogIndirect (&config, &nButtonPressed, nullptr, nullptr);
  
    if ( nButtonPressed == IDCANCEL )
    {
      return false;
    }
  }

  return true;
}

INT_PTR
CALLBACK
Main_DlgProc (
  _In_ HWND   hWndDlg,
  _In_ UINT   uMsg,
  _In_ WPARAM wParam,
  _In_ LPARAM lParam )
{
  UNREFERENCED_PARAMETER (lParam);

  HWND hWndProducts =
    GetDlgItem (hWndDlg, IDC_PRODUCT_SELECT);

  sk_product_t* product =
    &products [ComboBox_GetCurSel (hWndProducts)];

  switch (uMsg)
  {
    case WM_INITDIALOG:
    {
      SendMessage (hWndDlg, WM_SETICON, ICON_BIG,   (LPARAM)hIconSKIM_LG);
      SendMessage (hWndDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIconSKIM_SM);

      static bool init = false;

      if (! init) {
        INITCOMMONCONTROLSEX icex        = {                         };
                             icex.dwSize = sizeof INITCOMMONCONTROLSEX;
                             icex.dwICC  = ICC_STANDARD_CLASSES |
                                           ICC_BAR_CLASSES;

        InitCommonControlsEx (&icex);

        SKIM_Tray_Init (hWndDlg);

        LoadLibrary (L"msftedit.dll");

        SKIM_Tray_UpdateStartup ();

        init = true;
      }

      hWndRichProductDescrip =
        CreateWindowEx ( 0, MSFTEDIT_CLASS, TEXT ("RichTextHere"),
                           WS_VISIBLE | WS_CHILD | ES_READONLY | ES_MULTILINE|
                           ES_LEFT    | WS_DISABLED,
                           210,85,230,140,
                           hWndDlg, nullptr, g_hInstance, nullptr );

      hWndStatusBar =
        CreateWindowEx ( 0, STATUSCLASSNAME,
                           nullptr,
                             WS_CHILD | WS_VISIBLE |
                             SBARS_TOOLTIPS,
                                0, 0, 0, 0,
                                  hWndDlg,
                                    (HMENU)IDC_STATUS,
                                        g_hInstance, nullptr);

      SetCurrentDirectory (startup_dir);

      for (auto& _product : products)
      {
        SKIM_DetermineInstallState (_product);
      }

      hWndMainDlg = hWndDlg;

      ComboBox_SetCurSel    (hWndProducts, last_sel);
      ComboBox_ResetContent (hWndProducts);

      for (int i = 0; i < sizeof (products) / sizeof (sk_product_t); i++)
      {
        ComboBox_InsertString (hWndProducts, i, products [i].wszProjectName);
      }

      ComboBox_SetCurSel   (hWndProducts, last_sel);
      SKIM_OnProductSelect ();

      return TRUE;
    }

    case WM_COMMAND:
    {
      //switch (HIWORD (wParam))
      //{
      //  case 0:
      //    SKIM_Tray_ProcessCommand (hWndDlg, lParam, wParam);
      //    break;
      //  default:
      //    break;
      //};

      switch (LOWORD (wParam))
      {
        case IDC_PRODUCT_SELECT:
        {
          switch (HIWORD (wParam))
          {
            case CBN_SELCHANGE:
            {
              SKIM_OnProductSelect ();
            } break;
          }
        } break;

        case IDC_BRANCH_SELECT:
        {
          switch (HIWORD (wParam))
          {
            case CBN_SELCHANGE:
            {
              SKIM_OnBranchSelect ();
            } break;
          }
        } break;

        case IDC_DONATE:
        {
          int sel = std::max (0, ComboBox_GetCurSel (hWndProducts));

          wchar_t wszURL [1024];
          wsprintf ( wszURL,
                       L"https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=%s",
                         products [sel].wszDonateID );

          ShellExecuteW (NULL, L"OPEN", wszURL, nullptr, nullptr, SW_SHOWMAXIMIZED);
        } break;

        case IDC_MIGRATE_CMD:
        {
          static HWND
            hWndBranchSelect =
              GetDlgItem (hWndMainDlg, IDC_BRANCH_SELECT);

          wchar_t wszSel [256] = { };

          ComboBox_GetText (hWndBranchSelect, wszSel, 255);

          SKIM_BranchManager::singleton ()->migrateToBranch (wszSel);
          SKIM_BranchManager::singleton ()->setProduct      ((uint32_t)-1);

          if (product->uiSteamAppID != 0)
          {
            _beginthreadex (
              nullptr,
                0,
                 SKIM_MigrateProduct,
                   product,
                     0x00,
                       nullptr );
          }

          else if (product->uiSteamAppID == 0)
          {
            _beginthreadex (
              nullptr,
                0,
                  SKIM_MigrateGlobalInjector,
                    (LPVOID)hWndDlg,
                      0x00,
                        nullptr );
          }
        } break;

        case IDC_INSTALL_CMD:
        {
          if (product->uiSteamAppID != 0)
          {
            _beginthreadex (
              nullptr,
                0,
                 SKIM_InstallProduct,
                   product,
                     0x00,
                       nullptr );
          }

          else if (product->uiSteamAppID == 0)
          {
            _beginthreadex (
              nullptr,
                0,
                  SKIM_InstallGlobalInjector,
                    (LPVOID)hWndDlg,
                      0x00,
                        nullptr );
          }
        } break;


        case IDC_MANAGE_CMD:
        {
          // Tales of Symphonia  (has DLC)
          //
          if (product->uiSteamAppID == 372360)
          {
            extern unsigned int
            __stdcall
            DLCDlg_Thread (LPVOID user);

            SetCurrentDirectory (
              SKIM_FindInstallPath (product->uiSteamAppID)
            );

            ShowWindow (hWndMainDlg, SW_HIDE);

            SKIM_FetchDLCManagerDLL (*product);

            _beginthreadex ( nullptr, 0,
                               DLCDlg_Thread, (LPVOID)hWndMainDlg,
                                 0x00, nullptr );
          }


          // Tales of Zestiria  (has external config tool)
          //
          else if (product->uiSteamAppID == 351970)
          {
            STARTUPINFO         sinfo = { };
            PROCESS_INFORMATION pinfo = { };

            sinfo.cb          = sizeof STARTUPINFO;
            sinfo.dwFlags     = STARTF_USESHOWWINDOW;
            sinfo.wShowWindow = SW_SHOWNORMAL;

            SetCurrentDirectory (
              SKIM_FindInstallPath (product->uiSteamAppID)
            );

            CreateProcess ( L"tzt.exe",
                              nullptr,
                                nullptr, nullptr,
                                  FALSE, DETACHED_PROCESS,
                                    nullptr, SKIM_FindInstallPath (product->uiSteamAppID),
                                      &sinfo, &pinfo );

            if (hWndMainDlg != 0)
            {
              ShowWindow (hWndMainDlg, SW_HIDE);
            }

            if (WaitForSingleObject ( pinfo.hProcess, INFINITE ) == WAIT_OBJECT_0)
            {
              CloseHandle (pinfo.hThread);
              CloseHandle (pinfo.hProcess);
            }

            ShowWindow          (hWndMainDlg, SW_SHOW);
            SetForegroundWindow (hWndMainDlg);
            SendMessage         (hWndMainDlg, WM_INITDIALOG, 0x00, 0x00);
          }


          // Dark Souls III  (has external config tool)
          //
          else if (product->uiSteamAppID == 374320)
          {
            STARTUPINFO         sinfo = { };
            PROCESS_INFORMATION pinfo = { };

            sinfo.cb          = sizeof STARTUPINFO;
            sinfo.dwFlags     = STARTF_USESHOWWINDOW;
            sinfo.wShowWindow = SW_SHOWNORMAL;

            SetCurrentDirectory (
              SKIM_FindInstallPath (product->uiSteamAppID)
            );

            CreateProcess ( L"ds3t.exe",
                              nullptr,
                                nullptr, nullptr,
                                  FALSE, DETACHED_PROCESS,
                                    nullptr, SKIM_FindInstallPath (product->uiSteamAppID),
                                      &sinfo, &pinfo );

            if (hWndMainDlg != 0)
            {
              ShowWindow (hWndMainDlg, SW_HIDE);
            }

            if (WaitForSingleObject ( pinfo.hProcess, INFINITE ) == WAIT_OBJECT_0)
            {
              CloseHandle (pinfo.hThread);
              CloseHandle (pinfo.hProcess);
            }

            ShowWindow          (hWndMainDlg, SW_SHOW);
            SetForegroundWindow (hWndMainDlg);
            SendMessage         (hWndMainDlg, WM_INITDIALOG, 0x00, 0x00);
          }


          // Global Injector  (SKIM can start/stop injection)
          //
          else if (product->uiSteamAppID == 0)
          {
            SKIM_GlobalInject_StartStop (hWndDlg);
          }
        }  break;


        case IDC_UNINSTALL_CMD:
        {
          _beginthreadex (
            nullptr,
              0,
                SKIM_UninstallProduct, product, 0x00, nullptr);
        } break;
      }
      return (INT_PTR)true;
    }

    case WM_QUIT:
    case WM_CLOSE:
    case WM_DESTROY:
    {
      if (! hWndRestart)
      {
        if (SKIM_ConfirmClose ())
        {
          if (SKIM_GlobalInject_Stop ())
          {
            SKIM_StopInjectingAndExit (hWndDlg);
          }
        }
      }

      return 1;
    } break;

    case SKIM_STOP_INJECTION:
    {
      SKIM_GlobalInject_Stop (hWndDlg, false);

      return 0;
    }

    case SKIM_STOP_INJECTION_AND_EXIT:
    {
      SKIM_StopInjectingAndExit (hWndDlg, false);

      //return 0;
    }

    case SKIM_START_INJECTION:
    {
      SKIM_GlobalInject_Start (hWndDlg);

      return 0;
    }

    case SKIM_RESTART_INJECTION:
    {
      SKIM_GlobalInject_Stop (hWndDlg, false);
      hWndRestart           = hWndDlg;
      hWndMainDlg           = nullptr;
      return 0;
    }

    case (WM_USER | 0x0420):
    {
      SKIM_Tray_RefreshMenu (hWndDlg);

      switch (LOWORD (lParam))
      {
        case NIN_BALLOONSHOW:
          break;
        case NIN_POPUPOPEN:
        {
        } break;

        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
          SKIM_Tray_RestoreFrom (hWndDlg);
          break;

        case WM_CONTEXTMENU:
        {
          SKIM_Tray_HandleContextMenu (hWndDlg);
        }
        break;
      }

      return (INT_PTR)false;
    } break;

    case WM_MENUCOMMAND:
    {
      SKIM_Tray_ProcessCommand (hWndDlg, lParam, wParam);

      return (INT_PTR)false;
    } break;

    case WM_SIZE:
    {
      if      (wParam == SIZE_MINIMIZED)
      {
        SKIM_Tray_SendTo (hWndDlg);
      }

      else if (wParam == SIZE_RESTORED)
      {
        SKIM_Tray_RestoreFrom (hWndDlg);
      }

      return (INT_PTR)false;
    } break;

    case WM_PAINT:
    case WM_CREATE:
      return (INT_PTR)false;
  }

  return (INT_PTR)false;
}

int
SKIM_GetProductIdx (sk_product_t* prod)
{
  for (int i = 0; i < sizeof (products) / sizeof (sk_product_t); i++)
  {
    if (products [i].uiSteamAppID == prod->uiSteamAppID)
      return i;
  }

  return -1;
}

sk_product_t*
SKIM_GetProductByIdx (int idx)
{
  if ((unsigned int)idx < sizeof (products) / sizeof (sk_product_t))
    return &products [idx];

  return nullptr;
}

sk_product_t*
SKIM_FindProductByAppID (uint32_t appid)
{
  for (auto& product : products)
  {
    if (product.uiSteamAppID == appid)
      return &product;
  }

  return nullptr;
}

void
SKIM_DisableGlobalInjector ()
{
  SKIM_GlobalInject_Stop (false);
  SKIM_GlobalInject_Free ();

  // Wait for uninjection to finish (the stupid way)
  int iters = 0;
  while (GetFileAttributes (L"SpecialK32.pid") != INVALID_FILE_ATTRIBUTES && iters < 4) {
    SleepEx (150UL, TRUE); ++iters;
  }

  __time32_t _time;
  _time32  (&_time);

  wchar_t wszTemp      [MAX_PATH + 2] = { };
  wchar_t wsz32BitDLL  [MAX_PATH + 2] = { };
  wchar_t wsz64BitDLL  [MAX_PATH + 2] = { };
  wchar_t wszSKIM      [MAX_PATH + 2] = { };
  wchar_t wszRepo      [MAX_PATH + 2] = { };
  wchar_t wszInstalled [MAX_PATH + 2] = { };

  wcsncpy    (wszSKIM,     SKIM_FindInstallPath (0), MAX_PATH);
  PathAppend (wszSKIM,     L"SKIM64.exe");

  wcsncpy    (wsz32BitDLL, SKIM_FindInstallPath (0), MAX_PATH);
  PathAppend (wsz32BitDLL, L"SpecialK32.dll");

  wcsncpy    (wsz64BitDLL, SKIM_FindInstallPath (0), MAX_PATH);
  PathAppend (wsz64BitDLL, L"SpecialK64.dll");

  GetTempFileNameW (SKIM_FindInstallPath (0), L"SKI", _time, wszTemp);
  MoveFileW        (wsz32BitDLL, wszTemp);
  DeleteFileW      (wszTemp);

  *wszTemp = L'\0';

  GetTempFileNameW (SKIM_FindInstallPath (0), L"SKI", _time + 1, wszTemp);
  MoveFileW        (wsz64BitDLL, wszTemp);
  DeleteFileW      (wszTemp);

  wcsncpy (wszRepo,      SKIM_FindInstallPath (0), MAX_PATH);
  wcsncpy (wszInstalled, SKIM_FindInstallPath (0), MAX_PATH);

  PathAppend (wszInstalled, L"Version\\installed.ini");
  PathAppend (wszRepo,      L"Version\\repository.ini");

  DeleteFileW (wszInstalled);
  DeleteFileW (wszRepo);

  SKIM_SetStartMenuLink (FALSE, wszSKIM);
}


//
// XXX: This needs re-writing for 0.8.x
//
unsigned int
__stdcall
SKIM_MigrateGlobalInjector (LPVOID user)
{
  HWND hWndParent = (HWND)user;

  SKIM_GlobalInject_Stop (false);
  SKIM_GlobalInject_Free ();

  wchar_t wszDestDLL32 [MAX_PATH + 2] = { };
  wchar_t wszDestDLL64 [MAX_PATH + 2] = { };

  uint32_t dwStrLen = MAX_PATH;
  SKIM_Util_GetDocumentsDir (wszDestDLL32, &dwStrLen);

  dwStrLen = MAX_PATH;
  SKIM_Util_GetDocumentsDir (wszDestDLL64, &dwStrLen);

  PathAppend (wszDestDLL32, L"My Mods\\SpecialK\\");
  PathAppend (wszDestDLL64, L"My Mods\\SpecialK\\");

  // Create the destination directory
  SKIM_Util_CreateDirectories (wszDestDLL32);

  SetCurrentDirectoryW (wszDestDLL32);

  GetShortPathNameW (wszDestDLL32, wszDestDLL32, MAX_PATH);
  GetShortPathNameW (wszDestDLL64, wszDestDLL64, MAX_PATH);

  if (StrStrIW (wszDestDLL32, L" ")) {
    MessageBox ( hWndParent,
                   L"You may have 8.3 Filenames Disabled; they are necessary to "
                   L"make this software work!\n\n"
                   L"Delete the directory \"Documents\\My Mods\" after you fix "
                   L"Windows and run the software again.",
                     L"MS-DOS 8.3 Filename Not Possible",
                       MB_ICONERROR | MB_OK );
  }

  else {
    PathAppend (wszDestDLL32, L"SpecialK32.dll");
    PathAppend (wszDestDLL64, L"SpecialK64.dll");

    sk_product_t sk32 =
    {
      L"SpecialK32.dll",
      L"",
      L"Special K",
      L"Special K",
      L"Special K (Global Injector)",
      L"",
      L"SpecialK",
      L"8A7FSUFJ6KB2U",
      0,
      SK_BOTH_BIT,
      false,
      nullptr,
      0
    };

    sk_product_t sk64 =
    {
      L"SpecialK64.dll",
      L"",
      L"Special K",
      L"Special K",
      L"Special K (Global Injector)",
      L"",
      L"SpecialK",
      L"8A7FSUFJ6KB2U",
      0,
      SK_BOTH_BIT,
      false,
      nullptr,
      0
    };

    wcsncpy ( sk32.wszWrapper, 
                wszDestDLL32,
                  MAX_PATH );

    wcsncpy ( sk64.wszWrapper, 
                wszDestDLL64,
                  MAX_PATH );

                     SKIM_FetchInjector32 (sk32);
    SKIM_RunIf64Bit (SKIM_FetchInjector64 (sk64));


    int               nButtonPressed = 0;
    TASKDIALOGCONFIG  config         = {0};

    config.cbSize             = sizeof (config);
    config.hInstance          = g_hInstance;
    config.hwndParent         = hWndParent;
    config.pszWindowTitle     = L"Special K Install Manager";
    config.dwCommonButtons    = 0;//TDCBF_OK_BUTTON;
    config.pszMainInstruction = L"Installation of Global Injector Pending";
    config.pButtons           = nullptr;
    config.cButtons           = 0;
    config.pszContent         = L"Global Injector Downloaded but not Activated\r\n"
                                L"For compatibility options, PRESS AND HOLD "
                                L"Ctrl + Shift before launching a game.";
    config.pszMainIcon        = TD_INFORMATION_ICON;

    TaskDialogIndirect (&config, &nButtonPressed, nullptr, nullptr);
  }


  SKIM_GlobalInject_Start ();

  ShowWindow  ((HWND)hWndParent, SW_SHOW);
  SendMessage ((HWND)hWndParent, WM_INITDIALOG, 0x00, 0x00);


  SKIM_BranchManager::singleton ()->setProduct ((uint32_t)-1);
  SKIM_OnProductSelect          ();
  SKIM_BranchManager::singleton ()->setProduct ((uint32_t)-1);
  SKIM_OnBranchSelect           ();

  return 0;
}

unsigned int
__stdcall
SKIM_InstallGlobalInjector (LPVOID user)
{
  auto hWndParent = (HWND)user;

  SKIM_GlobalInject_Stop (false);

  wchar_t wszDestDLL32     [MAX_PATH + 2] = { };
  wchar_t wszDestDLL64     [MAX_PATH + 2] = { };

  wchar_t wszDestVersion   [MAX_PATH + 2] = { };
  wchar_t wszDestInstaller [MAX_PATH + 2] = { };
  wchar_t wszTempInstaller [MAX_PATH + 2] = { };

  wchar_t wszExec          [MAX_PATH + 2] = { };

  uint32_t dwStrLen = MAX_PATH;
  SKIM_Util_GetDocumentsDir (wszDestDLL32, &dwStrLen);

  lstrcatW (wszDestDLL32, L"\\My Mods\\SpecialK");

  wcsncpy  (wszDestInstaller, wszDestDLL32, MAX_PATH);
  wcsncpy  (wszTempInstaller, wszDestDLL32, MAX_PATH);
  wcsncpy  (wszDestVersion,   wszDestDLL32, MAX_PATH);
  lstrcatW (wszDestVersion, L"\\Version\\");

  PathAppendW (wszDestInstaller,
    SKIM_RunLHIfBitness ( 32, L"SKIM.exe", L"SKIM64.exe" ) );
  PathAppendW (wszTempInstaller,
    SKIM_RunLHIfBitness ( 32, L"SKIM.old", L"SKIM64.old" ) );

  // Create the destination directory
  SKIM_Util_CreateDirectories (wszDestInstaller);
  SKIM_Util_CreateDirectories (wszDestVersion);

  GetModuleFileName (GetModuleHandle (nullptr), wszExec, MAX_PATH);

  if (_wcsnicmp (startup_dir, wszDestDLL32, MAX_PATH))
  {
    SKIM_Util_MoveFileNoFail (wszExec, wszDestInstaller);

    STARTUPINFO         sinfo = { };
    PROCESS_INFORMATION pinfo = { };

    sinfo.cb          = sizeof STARTUPINFO;
    sinfo.dwFlags     = STARTF_USESHOWWINDOW;
    sinfo.wShowWindow = SW_NORMAL;

    CreateProcess ( wszDestInstaller,
                      L"\"SKIM64.exe\" 0",
                        nullptr, nullptr,
                          FALSE, 0x00,
                            nullptr, wszDestDLL32,
                              &sinfo, &pinfo );

    CloseHandle (pinfo.hThread);
    CloseHandle (pinfo.hProcess);

    SKIM_Exit ();
  }

  DeleteFileW (L"Version\\installed.ini");

  //
  //

#if 0
  if (StrStrIW (wszDestDLL32, L" ")) {
    MessageBox ( hWndParent,
                   L"You may have 8.3 Filenames Disabled; they are necessary to "
                   L"make this software work!\n\n"
                   L"Delete the directory \"Documents\\My Mods\" after you fix "
                   L"Windows and run the software again.",
                     L"MS-DOS 8.3 Filename Not Possible",
                       MB_ICONERROR | MB_OK );
  }

  else 
#endif
  {
#ifdef _WIN64
    wcsncpy  (wszDestDLL64, wszDestDLL32, MAX_PATH);
    lstrcatW (wszDestDLL64, L"\\SpecialK64.dll");

    static sk_product_t sk64 =
    {
      L"SpecialK64.dll",
      L"",
      L"Special K",
      L"Special K",
      L"Special K (Global Injector)",
      L"",
      L"SpecialK/0.8.x",
      L"8A7FSUFJ6KB2U",
      0,
      SK_BOTH_BIT,
      false,
      nullptr,
      0
    };

    wcsncpy ( sk64.wszWrapper, 
                wszDestDLL64,
                  MAX_PATH );

    SKIM_FetchInjector64  (sk64);

    HMODULE hModInstaller =
      LoadLibrary (wszDestDLL64);
#else
    lstrcatW (wszDestDLL32, L"\\SpecialK32.dll");

    static sk_product_t sk32 =
    {
      L"SpecialK32.dll",
      L"",
      L"Special K",
      L"Special K",
      L"Special K (Global Injector)",
      L"",
      L"SpecialK/0.8.x",
      L"8A7FSUFJ6KB2U",
      0,
      SK_BOTH_BIT,
      nullptr,
      nullptr
    };

    wcsncpy ( sk32.wszWrapper, 
                wszDestDLL32,
                  MAX_PATH );

    SKIM_FetchInjector32  (sk32);

    HMODULE hModInstaller =
      LoadLibrary (wszDestDLL32);
#endif

    if (hModInstaller != nullptr)
    {
      using SK_UpdateSoftware1_pfn   = HRESULT (__stdcall *)(const wchar_t* wszProduct, bool force);
      using SK_FetchVersionInfo1_pfn = bool    (__stdcall *)(const wchar_t* wszProduct, bool force);

      auto SK_UpdateSoftware1 =
        (SK_UpdateSoftware1_pfn)
          GetProcAddress ( hModInstaller,
                            "SK_UpdateSoftware1" );

      auto SK_FetchVersionInfo1 =
        (SK_FetchVersionInfo1_pfn)
          GetProcAddress ( hModInstaller,
                          "SK_FetchVersionInfo1" );

      if ( SK_FetchVersionInfo1 != nullptr &&
           SK_UpdateSoftware1   != nullptr )
      {
        ShowWindow  ((HWND)hWndParent, SW_HIDE);

        if (! SK_FetchVersionInfo1 (L"SpecialK", true))
          MessageBoxW (nullptr, L"Failed to fetch version info from GitHub", L"Download Failure", MB_ICONERROR);

        if (SUCCEEDED (SK_UpdateSoftware1 (L"SpecialK", true)))
        {
          SKIM_SetStartupInjection (TRUE, wszDestInstaller);
          SKIM_SetStartMenuLink    (TRUE, wszDestInstaller);

          int               nButtonPressed =  0;
          TASKDIALOGCONFIG  config         = { };
          
          config.cbSize              = sizeof (config);
          config.hInstance           = g_hInstance;
          config.hwndParent          = hWndParent;
          config.pszWindowTitle      = L"Special K Install Manager";
          config.dwCommonButtons     = 0;//TDCBF_OK_BUTTON;
          config.pszMainInstruction  = L"Setup of Global Injector Successful";
          config.pButtons            = nullptr;
          config.cButtons            = 0;

          wchar_t wszWorkingDir [MAX_PATH] = { };
          wchar_t wszContent    [  4096  ] = { };

          GetCurrentDirectoryW (MAX_PATH - 1, wszWorkingDir);

          swprintf ( wszContent, L"This program (SKIM64.exe) is now located in:\r\n"
                                 L"\r\n     "
                                 L"<a href=\"%ws\">Documents\\My Mods\\SpecialK\\"
                                 L"</a>\r\n\r\n"
                                 L"This program will now minimize itself to the system tray.",
                                   wszWorkingDir );

          config.pszContent          = wszContent;
          config.pszFooterIcon       = TD_SHIELD_ICON;
          config.pszFooter           = L"Compatibility Menu:   HOLD "
                                       L"Ctrl + Shift at game startup.";
          config.pszMainIcon         = TD_INFORMATION_ICON;
          config.pszVerificationText = L"Start With Windows";
          config.dwFlags             = TDF_VERIFICATION_FLAG_CHECKED |
                                       TDF_ENABLE_HYPERLINKS;

          config.pfCallback          = 
            []( _In_ HWND     hWnd,
                _In_ UINT     uNotification,
                _In_ WPARAM   wParam,
                _In_ LPARAM   lParam,
                _In_ LONG_PTR dwRefData ) ->
            HRESULT
            {
              UNREFERENCED_PARAMETER (dwRefData);
              UNREFERENCED_PARAMETER (wParam);

              if (uNotification == TDN_HYPERLINK_CLICKED)
              {
                ShellExecuteW ( hWnd,
                                  L"OPEN",
                                    (wchar_t *)lParam,
                                      nullptr, nullptr,
                                        SW_SHOWNORMAL );

                SKIM_Tray_RemoveFrom (                          );
                SendMessage          (hWndMainDlg, SKIM_RESTART_INJECTION, 0, 0);
              }

              return S_OK;
            };
          
          BOOL bStartWithWindows = TRUE;
          
          TaskDialogIndirect (&config, &nButtonPressed, nullptr, &bStartWithWindows);

          SKIM_SetStartupInjection (bStartWithWindows,       wszDestInstaller);
          SKIM_Tray_RemoveFrom     (                                         );
          SendMessage              (hWndMainDlg, SKIM_RESTART_INJECTION, 0, 0);
        }

        else
        {
        //FreeLibrary (hModInstaller);
          DeleteFileW (wszDestDLL64);
        }

        ShowWindow  ((HWND)hWndParent, SW_SHOW);
        SendMessage ((HWND)hWndParent, WM_INITDIALOG, 0x00, 0x00);
      }
    }
  }

  SKIM_OnProductSelect          ();
  SKIM_BranchManager::singleton ()->setProduct ((uint32_t)-1);
  SKIM_OnProductSelect          ();

  return 0;
}

#include <memory>

int
WINAPI
wWinMain ( _In_     HINSTANCE hInstance,
           _In_opt_ HINSTANCE hPrevInstance,
           _In_     LPWSTR    lpCmdLine,
           _In_     int       nCmdShow )
{
  CoInitializeEx (nullptr, COINIT_MULTITHREADED);

  g_hInstance    =                   hInstance;
  hIconSKIM_LG   = (HICON)LoadImage (hInstance, MAKEINTRESOURCE (IDI_ICON1), IMAGE_ICON, 128, 128, 0x00);
  hIconSKIM_SM   =        LoadIcon  (hInstance, MAKEINTRESOURCE (IDI_ICON1));

  for (auto& product : products)
  {
    SKIM_DetermineInstallState (product);
  }

  GetCurrentDirectoryW  (MAX_PATH, startup_dir);
  wchar_t wszVersionDir [MAX_PATH + 2] = { };

  lstrcatW   (wszVersionDir, startup_dir);
  PathAppend (wszVersionDir, L"Version");

  SKIM_Util_DeleteTemporaryFiles (startup_dir,   L"*.tmp");
  SKIM_Util_DeleteTemporaryFiles (wszVersionDir, L"*.old");

  wchar_t* wszArgs =
    _wcsdup (PathGetArgsW (GetCommandLineW ()));

                                 WNDCLASS wc = { };
  GetClassInfo  (g_hInstance, L"#32770", &wc);
                                          wc.lpszClassName = L"SKIM_Frontend";
  RegisterClass (                        &wc);

  HWND hWndExisting =
    FindWindow (L"SKIM_Frontend", nullptr);



  bool injector_action = false;
  bool __SKIM_Inject   = false,
       __SKIM_Uninject = false;

  if (wcslen (wszArgs))
  {
         if (StrStrIW (wszArgs, L"+Inject")) __SKIM_Inject   = true;
    else if (StrStrIW (wszArgs, L"-Inject")) __SKIM_Uninject = true;

    if (__SKIM_Inject || __SKIM_Uninject)
      injector_action = true;
  }

  // If we're not starting / stopping injection, then reuse an existing instance.
  else if ((! wcslen (wszArgs)) && IsWindow (hWndExisting))
  {
    SetForegroundWindow (hWndExisting);
    BringWindowToTop    (hWndExisting);
    SetActiveWindow     (hWndExisting);

    SendMessage (hWndExisting, WM_SIZE, SIZE_RESTORED, 0);

    if (wszArgs != nullptr)
      free (wszArgs);

    return 0;
  }


  if (injector_action)
  {
    if (__SKIM_Uninject)
    {
      // Exit the existing instance
      if (IsWindow (hWndExisting))
        SendMessage (hWndExisting, SKIM_STOP_INJECTION_AND_EXIT, 0, 0);

      SKIM_Exit ();
    }

    if (__SKIM_Inject)
    {
      __SKIM_Inject = false;

      if ((! IsWindow (hWndExisting)) && SKIM_GlobalInject_Start ())
      {
        hWndMainDlg =
          CreateDialog ( hInstance,
                           MAKEINTRESOURCE (IDD_FRONTEND),
                             0,
                               Main_DlgProc );

        // Minimize to system tray immediately
        SendMessage (hWndMainDlg, WM_SIZE, SIZE_MINIMIZED, 0);
      }

      else
      {
        if (IsWindow (hWndExisting))
          SendMessage (hWndExisting, SKIM_START_INJECTION, 0, 0);

        SKIM_Exit ();
      }
    }
  }

  else
  {
    hWndMainDlg =
      CreateDialog ( hInstance,
                       MAKEINTRESOURCE (IDD_FRONTEND),
                         0,
                           Main_DlgProc );

    // Show normally (or whatever nCmdShow tells us to do)
    ShowWindow (hWndMainDlg, SW_NORMAL);
    ShowWindow (hWndMainDlg, nCmdShow);
  }


  wchar_t    wszTitle [128] = { };
  lstrcatW ( wszTitle,
                L"Special K Install Manager - (v " );
  lstrcatW ( wszTitle,
                SKIM_VERSION_STR_W );
  lstrcatW ( wszTitle,
                L")" );

  SetWindowText (hWndMainDlg, wszTitle);


  // We can install PlugIns by passing their AppID through the cmdline
  if ((! injector_action) && wcslen (wszArgs))
  {
    int32_t appid = _wtoi (wszArgs);

    if (appid > 0)
    {
      sk_product_t* prod =
        SKIM_FindProductByAppID (appid);

      if (prod != nullptr)
      {
        child = true;

        // Set the correct selection after re-launching
        uintptr_t idx = ( (uintptr_t)prod - (uintptr_t)products ) /
                         sizeof sk_product_t;

        HWND hWndProductCtrl =
          GetDlgItem (hWndMainDlg, IDC_PRODUCT_SELECT);

        ComboBox_SetCurSel   (hWndProductCtrl, idx);
        SKIM_OnProductSelect ();

        SetCurrentDirectory (SKIM_FindInstallPath (appid));
        _beginthreadex ( nullptr, 0,
                           SKIM_InstallProduct, prod,
                             0x00, nullptr );
      }
    }

    else
    {
      bool zero = StrStrIW (wszArgs, L"0")  != nullptr ? wcslen (StrStrIW (wszArgs, L"0"))  == 1 : false;
      bool neg1 = StrStrIW (wszArgs, L"-1") != nullptr ? wcslen (StrStrIW (wszArgs, L"-1")) == 2 : false;

      // Install Global Injector
      if (zero && appid == 0)
      {
        _beginthreadex (
          nullptr,
            0,
              SKIM_InstallGlobalInjector,
                (LPVOID)hWndMainDlg,
                  0x00,
                    nullptr );
      }

      // Uninstall Global Injector
      else if (neg1 && appid == -1)
      {
        _beginthreadex (
          nullptr,
            0,
              SKIM_UninstallProduct, SKIM_FindProductByAppID (0), 0x00, nullptr);
      }
    }
  }


  MSG  msg;
  BOOL bRet;

  while (hWndMainDlg != nullptr && (bRet = GetMessage (&msg, nullptr, 0, 0)) != 0)
  {
    if (bRet == -1)
      break;

    if (hWndMainDlg != nullptr && hWndRestart == nullptr)
    {
      TranslateMessage (&msg);
      DispatchMessage  (&msg);
    }

    else
      break;
  }

  if (hWndRestart)
  {
    SKIM_Tray_RemoveFrom (           );
    DestroyWindow        (hWndRestart);

    STARTUPINFO         sinfo = { };
    PROCESS_INFORMATION pinfo = { };

    sinfo.cb          = sizeof STARTUPINFO;
    sinfo.dwFlags     = STARTF_USESHOWWINDOW;
    sinfo.wShowWindow = SW_NORMAL;

    CreateProcess ( L"SKIM64.exe",
                      L"\"SKIM64.exe\" +Inject",
                        nullptr, nullptr,
                          FALSE, 0x00,
                            nullptr, startup_dir,
                              &sinfo, &pinfo );

    CloseHandle (pinfo.hThread);
    CloseHandle (pinfo.hProcess);
  }

  free (wszArgs);

  // Prevent DLL shutdown, we didn't load the DLLs for their regular intended purpose
  SKIM_Exit ();

UNREFERENCED_PARAMETER (nCmdShow);
UNREFERENCED_PARAMETER (lpCmdLine);
UNREFERENCED_PARAMETER (hPrevInstance);

#ifdef _DEBUG
  return 0;
#endif
}
