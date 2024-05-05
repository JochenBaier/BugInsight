// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <array>
#include <cassert>

#include <Windows.h>

#include <utest.h>
#include <sddl.h>
#include <AclAPI.h>
#include <shlwapi.h>

extern size_t buginsight_internal_allocated_bytes(const char* p_function);

struct localalloc_tests
{
};

UTEST_F_SETUP(localalloc_tests)
{
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));
}

UTEST_F_TEARDOWN(localalloc_tests)
{
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("*"));
}

UTEST_F(localalloc_tests, LocalAlloc_0_exit_0)
{
  void* p=LocalAlloc(0, 0);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("LocalAlloc"));
  ASSERT_EQ(0, LocalSize(p));

  LocalFree(p);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("LocalAlloc"));
}

UTEST_F(localalloc_tests, LocalAlloc_100_exit_0)
{
  void* p=LocalAlloc(0, 100);
  ASSERT_EQ(100, buginsight_internal_allocated_bytes("LocalAlloc"));
  ASSERT_EQ(100, LocalSize(p));

  LocalFree(p);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("LocalAlloc"));
}

UTEST_F(localalloc_tests, LocalAlloc_LMEM_ZEROINIT_100_exit_0)
{
  void* p=LocalAlloc(LMEM_ZEROINIT, 100);
  ASSERT_EQ(100, buginsight_internal_allocated_bytes("LocalAlloc"));
  ASSERT_EQ(100, LocalSize(p));

  std::array<uint8_t, 100> zeros{0};

  ASSERT_TRUE(memcmp(zeros.data(), p, 100) == 0);

  LocalFree(p);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("LocalAlloc"));
}

UTEST_F(localalloc_tests, LocalAlloc_10000_exit_0)
{
  void* p=LocalAlloc(0, 10000);
  ASSERT_EQ(10000, buginsight_internal_allocated_bytes("LocalAlloc"));
  ASSERT_EQ(10000, LocalSize(p));

  LocalFree(p);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("LocalAlloc"));
}

UTEST_F(localalloc_tests, LocalReAlloc_1000_1010_exit_0)
{
  void* p=LocalAlloc(LMEM_MOVEABLE, 1000);
  ASSERT_EQ(1000, buginsight_internal_allocated_bytes("LocalAlloc"));
  ASSERT_EQ(1000, LocalSize(p));

  void* p2=LocalReAlloc(p, 1010, 0);
  ASSERT_TRUE(p2);

  ASSERT_EQ(1010, buginsight_internal_allocated_bytes("LocalReAlloc"));
  ASSERT_EQ(1010, LocalSize(p2));
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("LocalAlloc"));

  LocalFree(p2);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("LocalAlloc"));
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("LocalReAlloc"));
}

LPWSTR GetFormattedMessageW(LPWSTR pMessage, ...)
{
  LPWSTR pBuffer=NULL;

  va_list args=NULL;
  va_start(args, pMessage);

  FormatMessageW(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ALLOCATE_BUFFER,
    pMessage,
    0,
    0,
    (LPWSTR)&pBuffer,
    0,
    &args);

  va_end(args);

  return pBuffer;
}

UTEST_F(localalloc_tests, FormatMessageW_exit_0)
{
  LPWSTR pBuffer=NULL;
  LPWSTR pMessage=(LPWSTR)L"%1!*.*s! %3 %4!*s!";

  pBuffer=GetFormattedMessageW(pMessage, 4, 2, L"Bill", L"Bob", 6, L"Bill");
  ASSERT_TRUE(pBuffer);
  ASSERT_TRUE(wcscmp(L"  Bi Bob   Bill", pBuffer) == 0);

  LocalFree(pBuffer);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("FormatMessageW"));
}

LPSTR GetFormattedMessageA(LPSTR pMessage, ...)
{
  LPSTR pBuffer=NULL;

  va_list args=NULL;
  va_start(args, pMessage);

  FormatMessageA(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ALLOCATE_BUFFER,
    pMessage,
    0,
    0,
    (LPSTR)&pBuffer,
    0,
    &args);

  va_end(args);

  return pBuffer;
}


UTEST_F(localalloc_tests, FormatMessageA_exit_0)
{

  LPSTR pBuffer=NULL;
  LPSTR pMessage=(char*)"%1!*.*s! %3 %4!*s!";

  // The variable length arguments correspond directly to the format
  // strings in pMessage.
  pBuffer=GetFormattedMessageA(pMessage, 4, 2, "Bill", "Bob", 6, "Bill");
  ASSERT_TRUE(pBuffer);
  ASSERT_EQ(16, buginsight_internal_allocated_bytes("FormatMessageA"));

  ASSERT_TRUE(strcmp("  Bi Bob   Bill", pBuffer) == 0);

  LocalFree(pBuffer);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("FormatMessageA"));
}

UTEST_F(localalloc_tests, FormatMessageWNoAlloc_exit_0)
{
  std::wstring pMessage=L"%1!*.*s! %4 %5!*s!";
  DWORD_PTR pArgs[]={(DWORD_PTR)4, (DWORD_PTR)2, (DWORD_PTR)L"Bill",
    (DWORD_PTR)L"Bob",
    (DWORD_PTR)6, (DWORD_PTR)L"Bill"};
  const DWORD size=100 + 1;
  WCHAR buffer[size];

  if(!FormatMessageW(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ARGUMENT_ARRAY,
    pMessage.c_str(),
    0,
    0,
    buffer,
    size,
    (va_list*)pArgs))
  {
    ASSERT_TRUE(false);
  }

  ASSERT_TRUE(wcscmp(L"  Bi Bob   Bill", buffer) == 0);
}

UTEST_F(localalloc_tests, FormatMessageANoAlloc_exit_0)
{
  std::string pMessage="%1!*.*s! %4 %5!*s!";
  DWORD_PTR pArgs[]={(DWORD_PTR)4, (DWORD_PTR)2, (DWORD_PTR)"Bill",
    (DWORD_PTR)"Bob",
    (DWORD_PTR)6, (DWORD_PTR)"Bill"};
  const DWORD size=100 + 1;
  CHAR buffer[size];

  if(!FormatMessageA(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ARGUMENT_ARRAY,
    pMessage.c_str(),
    0,
    0,
    buffer,
    size,
    (va_list*)pArgs))
  {
    ASSERT_TRUE(false);
  }

  ASSERT_TRUE(strcmp("  Bi Bob   Bill", buffer) == 0);
}

//FIXME bad test missing...
UTEST_F(localalloc_tests, CommandLineToArgvW_exit_0)
{
  int v=0;
  wchar_t** argvW=CommandLineToArgvW(GetCommandLineW(), &v);

  size_t size_groupNameSide=buginsight_internal_allocated_bytes("CommandLineToArgvW");
  ASSERT_GE(size_groupNameSide, 100);

  LocalFree(argvW);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("CommandLineToArgvW"));
}

UTEST_F(localalloc_tests, ConvertStringSidToSidW_exit_0)
{
  PSID li_sid=nullptr;
  bool ok=ConvertStringSidToSidW(L"S-1-16-4096", &li_sid);
  ASSERT_TRUE(ok);

  size_t size=buginsight_internal_allocated_bytes("ConvertStringSidToSidW");
  ASSERT_EQ(size, 16);

  LPWSTR groupNameSid;
  ok=ConvertSidToStringSidW(li_sid, &groupNameSid);
  ASSERT_TRUE(ok);

  size_t size_groupNameSide=buginsight_internal_allocated_bytes("ConvertSidToStringSidW");
  ASSERT_EQ(size_groupNameSide, 24);

  LocalFree(groupNameSid);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("ConvertSidToStringSidW"));

  LocalFree(li_sid);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("ConvertStringSidToSidW"));
}

UTEST_F(localalloc_tests, ConvertStringSidToSidA_exit_0)
{
  PSID li_sid=nullptr;
  bool ok=ConvertStringSidToSid("S-1-16-4096", &li_sid);
  ASSERT_TRUE(ok);

  size_t size=buginsight_internal_allocated_bytes("ConvertStringSidToSidA");
  ASSERT_EQ(size, 16);

  LPSTR groupNameSid;
  ok=ConvertSidToStringSidA(li_sid, &groupNameSid);
  ASSERT_TRUE(ok);

  size_t size_groupNameSide=buginsight_internal_allocated_bytes("ConvertSidToStringSidA");
  ASSERT_EQ(size_groupNameSide, 12);

  LocalFree(groupNameSid);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("ConvertSidToStringSidA"));

  LocalFree(li_sid);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("ConvertStringSidToSidA"));
}

UTEST_F(localalloc_tests, GetNamedSecurityInfoW_exit_0)
{
  PSID pOwner=0;
  PSECURITY_DESCRIPTOR pSD;

  const DWORD res=GetNamedSecurityInfoW(L"C:/Windows", SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION, nullptr, nullptr, nullptr, nullptr, &pSD);
  ASSERT_EQ(res, ERROR_SUCCESS);

  size_t size_groupNameSide=buginsight_internal_allocated_bytes("GetNamedSecurityInfoW");
  ASSERT_EQ(size_groupNameSide, 1024);

  LocalFree(pSD);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("GetNamedSecurityInfoW"));
}

UTEST_F(localalloc_tests, GetNamedSecurityInfoA_exit_0)
{
  PSID pOwner=0;
  PSECURITY_DESCRIPTOR pSD;

  PACL pOldDACL=NULL, pNewDACL=NULL;

  const DWORD res=GetNamedSecurityInfoA("C:/Windows", SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, nullptr, nullptr, &pOldDACL, nullptr, &pSD);
  ASSERT_EQ(res, ERROR_SUCCESS);

  size_t size_groupNameSide=buginsight_internal_allocated_bytes("GetNamedSecurityInfoA");
  ASSERT_EQ(size_groupNameSide, 1024);

  LocalFree((HLOCAL)pNewDACL);

  LocalFree(pSD);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("GetNamedSecurityInfoA"));
}

UTEST_F(localalloc_tests, CryptProtectData_exit_0)
{
  DATA_BLOB DataIn{};
  DATA_BLOB DataOut{};
  BYTE* pbDataInput=(BYTE*)"Hello world of data protection.";
  DWORD cbDataInput=strlen((char*)pbDataInput) + 1;
  DataIn.pbData=pbDataInput;
  DataIn.cbData=cbDataInput;

  bool ok=CryptProtectData(
    &DataIn,
    L"This is the description string.", // A description string. 
    NULL,                               // Optional entropy
    // not used.
    NULL,                               // Reserved.
    NULL,                      // Pass a PromptStruct.
    0,
    &DataOut);

  ASSERT_TRUE(ok);

  size_t size=buginsight_internal_allocated_bytes("CryptProtectData");
  ASSERT_EQ(size, 352);

  LPWSTR pDescrOut=NULL;
  DATA_BLOB DataVerify;

  ok=CryptUnprotectData(
    &DataOut,
    &pDescrOut,
    NULL,                 // Optional entropy
    NULL,                 // Reserved
    NULL,        // Optional PromptStruct
    0,
    &DataVerify);

  ASSERT_TRUE(ok);

  size=buginsight_internal_allocated_bytes("CryptUnprotectData");
  ASSERT_EQ(size, 88 + 64);

  LocalFree(DataOut.pbData);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("CryptProtectData"));

  LocalFree(pDescrOut);
  LocalFree(DataVerify.pbData);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("CryptUnprotectData"));
}

UTEST_F(localalloc_tests, CryptProtectData_no_Descr_exit_0)
{
  DATA_BLOB DataIn;
  DATA_BLOB DataOut;
  BYTE* pbDataInput=(BYTE*)"Hello world of data protection.";
  DWORD cbDataInput=strlen((char*)pbDataInput) + 1;
  DataIn.pbData=pbDataInput;
  DataIn.cbData=cbDataInput;

  bool ok=CryptProtectData(
    &DataIn,
    L"This is the description string.", // A description string. 
    NULL,                               // Optional entropy
    // not used.
    NULL,                               // Reserved.
    NULL,                      // Pass a PromptStruct.
    0,
    &DataOut);

  ASSERT_TRUE(ok);

  size_t size=buginsight_internal_allocated_bytes("CryptProtectData");
  ASSERT_EQ(size, 352);

  DATA_BLOB DataVerify;

  ok=CryptUnprotectData(
    &DataOut,
    nullptr,
    NULL,                 // Optional entropy
    NULL,                 // Reserved
    NULL,        // Optional PromptStruct
    0,
    &DataVerify);

  ASSERT_TRUE(ok);

  size=buginsight_internal_allocated_bytes("CryptUnprotectData");
  ASSERT_EQ(size, 64);

  LocalFree(DataOut.pbData);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("CryptProtectData"));

  LocalFree(DataVerify.pbData);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("CryptUnprotectData"));
}

UTEST_F(localalloc_tests, SetEntriesInAclA_exit_0)
{
  PSID pEveryoneSID=NULL, pAdminSID=NULL;
  PACL pACL=NULL;
  PSECURITY_DESCRIPTOR pSD=NULL;
  EXPLICIT_ACCESS ea[2];
  SID_IDENTIFIER_AUTHORITY SIDAuthWorld=
    SECURITY_WORLD_SID_AUTHORITY;

  // Create a well-known SID for the Everyone group.
  if(!AllocateAndInitializeSid(&SIDAuthWorld, 1,
    SECURITY_WORLD_RID,
    0, 0, 0, 0, 0, 0, 0,
    &pEveryoneSID))
  {
    ASSERT_TRUE(false);
  }

  // Initialize an EXPLICIT_ACCESS structure for an ACE.
  // The ACE will allow Everyone read access to the key.
  ZeroMemory(&ea, 2 * sizeof(EXPLICIT_ACCESS));
  ea[0].grfAccessPermissions=KEY_READ;
  ea[0].grfAccessMode=SET_ACCESS;
  ea[0].grfInheritance=NO_INHERITANCE;
  ea[0].Trustee.TrusteeForm=TRUSTEE_IS_SID;
  ea[0].Trustee.TrusteeType=TRUSTEE_IS_WELL_KNOWN_GROUP;
  ea[0].Trustee.ptstrName=(LPTSTR)pEveryoneSID;

  SID_IDENTIFIER_AUTHORITY SIDAuthNT=SECURITY_NT_AUTHORITY;

  // Create a SID for the BUILTIN\Administrators group.
  if(!AllocateAndInitializeSid(&SIDAuthNT, 2,
    SECURITY_BUILTIN_DOMAIN_RID,
    DOMAIN_ALIAS_RID_ADMINS,
    0, 0, 0, 0, 0, 0,
    &pAdminSID))
  {
    ASSERT_TRUE(false);
  }

  // Initialize an EXPLICIT_ACCESS structure for an ACE.
  // The ACE will allow the Administrators group full access to
  // the key.
  ea[1].grfAccessPermissions=KEY_ALL_ACCESS;
  ea[1].grfAccessMode=SET_ACCESS;
  ea[1].grfInheritance=NO_INHERITANCE;
  ea[1].Trustee.TrusteeForm=TRUSTEE_IS_SID;
  ea[1].Trustee.TrusteeType=TRUSTEE_IS_GROUP;
  ea[1].Trustee.ptstrName=(LPTSTR)pAdminSID;

  // Create a new ACL that contains the new ACEs.
  DWORD dwRes=SetEntriesInAclA(2, ea, NULL, &pACL);
  if(ERROR_SUCCESS != dwRes)
  {
    ASSERT_TRUE(false);
  }

  FreeSid(pEveryoneSID);
  FreeSid(pAdminSID);

  size_t size=buginsight_internal_allocated_bytes("SetEntriesInAclA");
  ASSERT_EQ(size, 52);

  LocalFree(pACL);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("SetEntriesInAclA"));

}

UTEST_F(localalloc_tests, SetEntriesInAclW_exit_0)
{
  PSID pEveryoneSID=NULL, pAdminSID=NULL;
  PACL pACL=NULL;
  PSECURITY_DESCRIPTOR pSD=NULL;
  EXPLICIT_ACCESS_W ea[2];
  SID_IDENTIFIER_AUTHORITY SIDAuthWorld=
    SECURITY_WORLD_SID_AUTHORITY;

  // Create a well-known SID for the Everyone group.
  if(!AllocateAndInitializeSid(&SIDAuthWorld, 1,
    SECURITY_WORLD_RID,
    0, 0, 0, 0, 0, 0, 0,
    &pEveryoneSID))
  {
    ASSERT_TRUE(false);
  }

  // Initialize an EXPLICIT_ACCESS structure for an ACE.
  // The ACE will allow Everyone read access to the key.
  ZeroMemory(&ea, 2 * sizeof(EXPLICIT_ACCESS));
  ea[0].grfAccessPermissions=KEY_READ;
  ea[0].grfAccessMode=SET_ACCESS;
  ea[0].grfInheritance=NO_INHERITANCE;
  ea[0].Trustee.TrusteeForm=TRUSTEE_IS_SID;
  ea[0].Trustee.TrusteeType=TRUSTEE_IS_WELL_KNOWN_GROUP;
  ea[0].Trustee.ptstrName=(LPWCH)pEveryoneSID;

  SID_IDENTIFIER_AUTHORITY SIDAuthNT=SECURITY_NT_AUTHORITY;

  // Create a SID for the BUILTIN\Administrators group.
  if(!AllocateAndInitializeSid(&SIDAuthNT, 2,
    SECURITY_BUILTIN_DOMAIN_RID,
    DOMAIN_ALIAS_RID_ADMINS,
    0, 0, 0, 0, 0, 0,
    &pAdminSID))
  {
    ASSERT_TRUE(false);
  }

  // Initialize an EXPLICIT_ACCESS structure for an ACE.
  // The ACE will allow the Administrators group full access to
  // the key.
  ea[1].grfAccessPermissions=KEY_ALL_ACCESS;
  ea[1].grfAccessMode=SET_ACCESS;
  ea[1].grfInheritance=NO_INHERITANCE;
  ea[1].Trustee.TrusteeForm=TRUSTEE_IS_SID;
  ea[1].Trustee.TrusteeType=TRUSTEE_IS_GROUP;
  ea[1].Trustee.ptstrName=(LPWCH)pAdminSID;

  // Create a new ACL that contains the new ACEs.
  DWORD dwRes=SetEntriesInAclW(2, ea, NULL, &pACL);
  if(ERROR_SUCCESS != dwRes)
  {
    ASSERT_TRUE(false);
  }

  FreeSid(pEveryoneSID);
  FreeSid(pAdminSID);

  size_t size=buginsight_internal_allocated_bytes("SetEntriesInAclW");
  ASSERT_EQ(size, 52);

  LocalFree(pACL);
  ASSERT_EQ(0, buginsight_internal_allocated_bytes("SetEntriesInAclW"));

}

UTEST_F(localalloc_tests, AllocateAndInitializeSid_exit_0)
{
  SID_IDENTIFIER_AUTHORITY SIDAuthNT=SECURITY_NT_AUTHORITY;

  PSID pAdminSID=NULL;
  if(!AllocateAndInitializeSid(&SIDAuthNT, 2,
    SECURITY_BUILTIN_DOMAIN_RID,
    DOMAIN_ALIAS_RID_ADMINS,
    0, 0, 0, 0, 0, 0,
    &pAdminSID))
  {
    ASSERT_TRUE(false);
  }

  size_t size=buginsight_internal_allocated_bytes("AllocateAndInitializeSid");
  ASSERT_EQ(size, 16);

  PSID pAdminSID2=NULL;
  if(!AllocateAndInitializeSid(&SIDAuthNT, 2,
    SECURITY_BUILTIN_DOMAIN_RID,
    DOMAIN_ALIAS_RID_ADMINS,
    0, 0, 0, 0, 0, 0,
    &pAdminSID2))
  {
    ASSERT_TRUE(false);
  }

  size=buginsight_internal_allocated_bytes("AllocateAndInitializeSid");
  ASSERT_EQ(size, 32);

  FreeSid(pAdminSID);

  size=buginsight_internal_allocated_bytes("AllocateAndInitializeSid");
  ASSERT_EQ(size, 16);

  FreeSid(pAdminSID2);

  size=buginsight_internal_allocated_bytes("AllocateAndInitializeSid");
  ASSERT_EQ(size, 0);
}

UTEST_F(localalloc_tests, GetSecurityInfo_exit_0)
{
  DWORD dwRtnCode=0;
  PSID pSidOwner=NULL;
  HANDLE hFile;
  PSECURITY_DESCRIPTOR pSD=NULL;

  hFile=CreateFile(
    TEXT("C:/Windows/notepad.exe"),
    GENERIC_READ,
    FILE_SHARE_READ,
    NULL,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    NULL);

  if(hFile == INVALID_HANDLE_VALUE) {
    ASSERT_TRUE(false);
  }

  dwRtnCode=GetSecurityInfo(
    hFile,
    SE_FILE_OBJECT,
    OWNER_SECURITY_INFORMATION,
    &pSidOwner,
    NULL,
    NULL,
    NULL,
    &pSD);

  if(dwRtnCode != ERROR_SUCCESS) {
    ASSERT_TRUE(false);
  }

  size_t size=buginsight_internal_allocated_bytes("GetSecurityInfo");
  ASSERT_EQ(size, 1024);

  auto res=LocalFree(pSD);

  size=buginsight_internal_allocated_bytes("GetSecurityInfo");
  ASSERT_EQ(size, 0);

  dwRtnCode=GetSecurityInfo(
    hFile,
    SE_FILE_OBJECT,
    0,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL);

  if(dwRtnCode != ERROR_SUCCESS) {
    ASSERT_TRUE(false);
  }

  size=buginsight_internal_allocated_bytes("GetSecurityInfo");
  ASSERT_EQ(size, 0);
}

UTEST_F(localalloc_tests, test_api_StrDupA_basic_exit_0)
{
  char buffer[]="This is the buffer text";
  char* newstring=StrDupA(buffer);
  ASSERT_EQ(strlen(buffer) + 1, buginsight_internal_allocated_bytes("StrDupA"));


  ASSERT_STREQ(buffer, newstring);
  ASSERT_EQ(strlen(buffer), strlen(newstring));

  size_t size=buginsight_internal_allocated_bytes("StrDupA");
  ASSERT_EQ(size, 24);

  LocalFree(newstring);

  size=buginsight_internal_allocated_bytes("StrDupA");
  ASSERT_EQ(size, 0);
}

UTEST_F(localalloc_tests, test_api_StrDupW_basic_exit_0)
{
  wchar_t buffer[]=L"This is the buffer text";
  wchar_t* newstring=StrDupW(buffer);
  //ASSERT_EQ(wcslen(buffer) + 2, buginsight_internal_allocated_bytes("StrDupW"));

  size_t size=buginsight_internal_allocated_bytes("StrDupW");
  ASSERT_EQ(size, 48);

  LocalFree(newstring);

  size=buginsight_internal_allocated_bytes("StrDupW");
  ASSERT_EQ(size, 0);
}
