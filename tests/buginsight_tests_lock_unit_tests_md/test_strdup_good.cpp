#define DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
#include "doctest.h"

#include <cstdlib>
#include <vector>


TEST_CASE("test_strdup_exit_0")
{
  SUBCASE("test_api_strdup_basic")
  {
    char buffer[]="This is the buffer text";
    char* newstring=strdup(buffer);

    REQUIRE_EQ(buffer, newstring);
    REQUIRE_EQ(strlen(buffer), strlen(newstring));

    free(newstring);
  }

  SUBCASE("test_api_strdup_empty")
  {
    char buffer[]="";
    char* newstring=strdup(buffer);

    REQUIRE_EQ(buffer, newstring);
    REQUIRE_EQ(strlen(buffer), strlen(newstring));

    free(newstring);
  }

  SUBCASE("test_api_underscore_strdup_basic")
  {
    char buffer[]="This is the buffer text";
    char* newstring=_strdup(buffer);

    REQUIRE_EQ(buffer, newstring);
    REQUIRE_EQ(strlen(buffer), strlen(newstring));

    free(newstring);
  }

  SUBCASE("test_api_underscore_strdup_empty")
  {
    char buffer[]="";
    char* newstring=_strdup(buffer);

    REQUIRE_EQ(buffer, newstring);
    REQUIRE_EQ(strlen(buffer), strlen(newstring));

    free(newstring);
  }

  SUBCASE("test_api_wcsdup_basic")
  {
    wchar_t buffer[]=L"This is the buffer text";
    wchar_t* newstring=wcsdup(buffer);

    REQUIRE_EQ(wcscmp(buffer, newstring), 0);
    REQUIRE_EQ(wcslen(buffer), wcslen(newstring));

    free(newstring);
  }

  SUBCASE("test_api_wcsdup_empty")
  {
    wchar_t buffer[]=L"";
    wchar_t* newstring=wcsdup(buffer);

    REQUIRE_EQ(wcscmp(buffer, newstring), 0);
    REQUIRE_EQ(wcslen(buffer), wcslen(newstring));

    free(newstring);
  }

  SUBCASE("test_api_underscore_wcsdup_basic")
  {
    wchar_t buffer[]=L"This is the buffer text";
    wchar_t* newstring=_wcsdup(buffer);

    REQUIRE_EQ(wcscmp(buffer, newstring), 0);
    REQUIRE_EQ(wcslen(buffer), wcslen(newstring));

    free(newstring);
  }

  SUBCASE("test_api_underscore_wcsdup_empty")
  {
    wchar_t buffer[]=L"";
    wchar_t* newstring=_wcsdup(buffer);

    REQUIRE_EQ(wcscmp(buffer, newstring), 0);
    REQUIRE_EQ(wcslen(buffer), wcslen(newstring));

    free(newstring);
  }

  SUBCASE("test_api_strdup_zero_size")
  {
    char* newstring=strdup(0);
    REQUIRE(newstring==nullptr);
  }

  SUBCASE("test_api_wcsdup_zero_size")
  {
    wchar_t* newstring=wcsdup(0);
    REQUIRE(newstring==nullptr);
  }


}
