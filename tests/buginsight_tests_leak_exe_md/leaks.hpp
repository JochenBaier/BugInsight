void* SCOPED_NAME(malloc_1000)=malloc(100);

void* SCOPED_NAME(_aligned_malloc_1000)=_aligned_malloc(1000, 2);
void* SCOPED_NAME(_aligned_offset_malloc_1000)=_aligned_offset_malloc(1001, 4, 3);
void* SCOPED_NAME(_aligned_realloc_a)=_aligned_realloc(0, 12, 4);
void* SCOPED_NAME(_aligned_realloc_b)=_aligned_realloc(SCOPED_NAME(_aligned_realloc_a), 102, 4);
void* SCOPED_NAME(_aligned_recalloc_a)=_aligned_recalloc(0, 0, 2, 2);
void* SCOPED_NAME(_aligned_recalloc_0_10)=_aligned_recalloc(SCOPED_NAME(_aligned_recalloc_a), 10, 2, 2);

void* SCOPED_NAME(calloc_10_20)=calloc(10, 20);
void* SCOPED_NAME(realloc_0_10)=realloc(0, 10);

void* SCOPED_NAME(realloc_a)=realloc(0, 10);
void* SCOPED_NAME(realloc_b)=realloc(SCOPED_NAME(realloc_a), 100);

void* SCOPED_NAME(_expand_a)=malloc(1024);
void* SCOPED_NAME(_expand_b)=_expand(SCOPED_NAME(_expand_a), 1030);

void* SCOPED_NAME(_recalloc_0_10)=_recalloc(0, 10, 20);
void* SCOPED_NAME(_recalloc_a)=_recalloc(0, 10, 4);
void* SCOPED_NAME(_recalloc_b)=_recalloc(SCOPED_NAME(_recalloc_a), 100, 2);

thread_local void* SCOPED_NAME(thread_local_malloc_40)=malloc(40);

void* SCOPED_NAME(heap_alloc_100)=HeapAlloc(GetProcessHeap(), 0, 101);
void* SCOPED_NAME(local_alloc_100)=LocalAlloc(0, 103);
void* SCOPED_NAME(global_alloc_100)=GlobalAlloc(0, 104);

void* SCOPED_NAME(_getcwd)=_getcwd(nullptr, 2040);
void* SCOPED_NAME(_wgetcwd)=_wgetcwd(nullptr, 2041);
void* SCOPED_NAME(_getdcwd)=_getdcwd(3, nullptr, 2042);
void* SCOPED_NAME(_wgetdcwd)=_wgetdcwd(3, nullptr, 2050);
void* SCOPED_NAME(_tempnam)=_tempnam("temp_dir", "file_prefix");
void* SCOPED_NAME(_wtempnam)=_wtempnam(L"temp_dir", L"file_prefix");

char* SCOPED_NAME(newstring)=strdup("test string");
wchar_t* SCOPED_NAME(newstringw)=_wcsdup(L"test string 2");

int* SCOPED_NAME(new_int_scalar)=new int;
int* SCOPED_NAME(new_int_array)=new int[100];
void* SCOPED_NAME(operator_new)=operator new(500);
void* SCOPED_NAME(operator_new_array)=operator new[](20);

//no leaks
std::string SCOPED_NAME(std_string)="text";
std::string SCOPED_NAME(std_string_large)="text too large for small string opti.text too large for small string opti";

std::vector<int> SCOPED_NAME(int_vector) { 1, 2, 3 };
std::vector<std::string> SCOPED_NAME(std_string_vector) { "one", "two" };

std::map<int, int> SCOPED_NAME(int_map) { { 1, 2 }, {3, 4} };

std::shared_ptr<int> SCOPED_NAME(shared_ptr_int)=std::make_shared<int>(42);
std::unique_ptr<int> SCOPED_NAME(unique_ptr_int)=std::make_unique<int>(42);

thread_local std::shared_ptr<int> SCOPED_NAME(shared_ptr_int_thread_local)=std::make_shared<int>(42);
thread_local std::unique_ptr<int> SCOPED_NAME(unique_ptr_int_thread_local)=std::make_unique<int>(42);
