#Licensed to the Apache Software Foundation (ASF) under one
#or more contributor license agreements.  See the NOTICE file
#distributed with this work for additional information
#regarding copyright ownership.  The ASF licenses this file
#to you under the Apache License, Version 2.0 (the
#"License"); you may not use this file except in compliance
#with the License.  You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
#Unless required by applicable law or agreed to in writing,
#software distributed under the License is distributed on an
#"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#KIND, either express or implied.  See the License for the
#specific language governing permissions and limitations
#under the License.  

set (api_tests
test_cs_t1_double_leave_exit_66
test_cs_leave_other_thread_exit_66
test_bad_thread_sanitzer_example_exit_66
test_bad_thread_sanitzer_example_cs_with_spin_count_exit_66
test_cs_t1_a_b_t2_b_a_overlap_exit_66
test_cs_t1_a_b_t2_b_a_no_overlap_exit_66
test_cs_t1_a_b_t2_b_and_more_exit_66
test_bad_cs_100t_10cs_bulk_exit_66
test_cs_t1_a_b_t2_b_a_no_overlap_halt_on_error_false_exit_66
test_cs_t1_a_b_t2_b_a_overlap_recurse_exit_66
test_cs_try_enter_t1_a_b_t2_b_a_overlap_exit_66
test_bad_cs_2t_10cs_bulk_exit_66
test_c_file_thread_sanitzer_example_exit_66
test_deadlock_in_header_exit_66
test_1t_2t2_exit_66
test_cs_too_many_locks_perh_thread_exit_66
test_cs_too_many_locks_perh_thread_1000_exit_66
test_cs_t1_a_b_t2_b_a_large_callstack_exit_66
test_abseil_test_1_exit_66
test_abseil_test_1000_large_exit_66
test_dining_philosophers_exit_66
test_delete_cs_before_leave_exit_66
test_delete_cs_in_other_thread_before_leave_exit_66
test_cs_leave_other_thread_cs_only_leave_disabled_exit_66
test_cs_leave_cs_which_the_thread_does_not_hold_exit_66
test_cs_t1_a_a_exit_0
test_ok_cs_t1_a_b_exit_0
test_cs_t1_a_b_a_b_exit_0
test_cs_t1_a_b_b_a_exit_0
test_ok_cs_t1_a_b_bulk_exit_0
test_ok_cs_t1_a_b_bulk2_exit_0
test_ok_cs_t1_a_b_t2_a_b_no_overlap_exit_0
test_ok_cs_t1_a_b_t2_a_b_may_overlap_exit_0
test_ok_cs_100t_10cs_bulk_exit_0
test_cs_t1_a_b_t2_b_a_no_overlap_cs_ignored_exit_0
test_cs_t1_a_b_t2_b_a_no_overlap_cs_ignored_both_threads_exit_0
test_cs_t1_a_b_t2_b_a_no_overlap_cs_ignored_both_threads_single_exit_0
test_cs_t1_double_leave_cs_disabled_exit_0
test_cs_leave_other_thread_cs_disabled_exit_0
test_t1_aaa_bbb_ccc_recure_exit_0
test_t1_abc_bbb_abc_recure_exit_0
test_ok_cs_t1_a_b_t2_a_b_recurse_exit_0
test_ok_cs_tryenter_critical_section_exit_0
test_cs_try_enter_t1_a_b_t2_b_a_overlap_tryenter_on_second_exit_0
test_cs_delete_without_use_exit_0
test_cs_delete_with_use_exit_0
test_cs_mass_delete_exit_0
test_cs_perf_test_exit_0
test_std_mutex_t1_a_lock_recurse_bad_exit_66
test_std_mutex_t1_a_unlock_double_bad_exit_66
test_std_mutex_unlock_other_thread_exit_66
test_std_mutex_t1_a_b_t2_b_a_overlap_exit_66
test_std_mutex_t1_a_b_t2_b_a_maybe_overlap_exit_66
test_std_mutex_t1_a_b_t2_b_a_no_overlap_exit_66
test_std_mutex_t1_a_b_t2_b_and_more_exit_66
test_std_mutex_100t_10_m_bulk_exit_66
test_lock_guard_std_mutex_t1_a_lock_recurse_bad_exit_66
test_lock_guard_std_mutex_t1_a_b_t2_b_a_maybe_overlap_exit_66
test_std_mutex_try_lock_t1_a_b_t2_b_a_overlap_exit_66
test_mutex_t1_a_b_c_d_e_a_exit_66
test_lock_guard_std_mutex_t1_a_b_t2_b_a_no_overlap_exit_66
test_lock_guard_std_mutex_t1_a_b_t2_b_a_large_callstack_exit_66
test_std_mutex_delete_before_unlock_exit_66
test_std_mutex_t1_a_exit_0
test_std_mutex_t1_a_b_exit_0
test_condition_variable_exit_0
test_lock_guard_std_mutex_t1_a_exit_0
test_std_lock_guard_std_mutex_t1_a_b_exit_0
test_unique_lock_std_mutex_t1_a_exit_0
test_std_unique_std_mutex_t1_a_b_exit_0
test_std_mutex_try_lock_t1_a_b_t2_b_a_overlap_try_on_second_exit_0
test_std_mutex_t1_a_b_t2_b_a_no_overlap_ignored_exit_0
test_lock_guard_std_mutex_t1_a_b_t2_b_a_no_overlap_ignore_exit_0
test_std_mutex_try_lock_second_time_exit_0
test_std_mutex_try_lock_t1_a_b_t2_b_a_no_overlap_exit_0
test_from_abseil_source_test_large_exit_0
test_std_mutex_delete_without_use_exit_0
test_std_mutex_delete_with_use_exit_0
test_std_mutex_mass_delete_exit_0
test_std_recursive_mutex_t1_a_unlock_double_bad_exit_66
test_std_recursive_mutex_unlock_other_thread_exit_66
test_std_recursive_mutex_t1_a_b_t2_b_a_overlap_exit_66
test_std_recursive_mutex_t1_a_b_t2_b_a_maybe_overlap_exit_66
test_std_recursive_mutex_t1_a_b_t2_b_a_no_overlap_exit_66
test_std_recursive_mutex_t1_a_b_t2_b_and_more_exit_66
test_std_recursive_mutex_100t_10_m_bulk_exit_66
test_lock_guard_std_recursive_mutex_t1_a_b_t2_b_a_maybe_overlap_exit_66
test_lock_guard_recursive__t1_a_b_t2_b_a_maybe_overlap_exit_66
test_std_recursive_mutex_delete_before_unlock_exit_66
test_std_recursive_mutex_t1_a_exit_0
test_std_recursive_mutex_t1_a_b_exit_0
test_std_mutex_t1_a_lock_recurse_good_exit_0
test_lock_guard_std_recursive_mutex_t1_a_exit_0
test_std_lock_guard_std_recursive_mutex_t1_a_b_exit_0
test_lock_guard_std_mutex_t1_a_lock_recurse_exit_0
test_std_recursive_mutex_delete_without_use_exit_0
test_std_recursive_mutex_delete_with_use_exit_0
test_std_recursive_mutex_mass_delete_exit_0
test_cs_t1_duration_6_sec_bad_exit_66
test_cs_t1_missing_leave_bad_exit_66
test_cs_2t_misssing_leave_bad_exit_66
test_ok_cs_t1_40_bad_exit_66
test_cs_t1_too_long_duration_exit_66
test_cs_misssing_leave_n_exit_66
test_cs_multi_thread_bad_exit_66
test_mutex_t1_duration_6_sec_bad_exit_66
test_lock_guard_mutex_t1_duration_6_sec_ok_exit_66
test_std_condition_variable_signal_blocked_too_long_exit_66
test_cs_too_many_waiters_exit_66
test_cs_t1_duration_1_sec_ok_exit_0
test_cs_t1_duration_4_sec_ok_exit_0
test_x_times_cs_t1_duration_1_sec_ok_exit_0
test_ok_cs_t1_bulk2_ok_exit_0
test_good_cs_multi_thread_ok_exit_0
test_mutex_t1_duration_1_sec_ok_exit_0
test_mutex_t1_duration_4_sec_ok_exit_0
test_lock_guard_mutex_t1_duration_4_sec_ok_exit_0
test_std_condition_variable_duration_10_sec_good_exit_0
test_std_condition_variable_duration_10_sec_with_signal_exit_0
test_std_condition_variable_signal_short_blocked_exit_0
test_thread_watchdog_many_too_long_durations_but_ignore_errors_exit_0
)

foreach (test ${api_tests})
  if(test MATCHES ".*_exit_66")
      message( TRACE "Test '${test} WILL_FAIL TRUE" )
      add_test (NAME ${test} COMMAND buginsight --on_error=fail --thread_watchdog_max_duration_sec=5 -- $<TARGET_FILE:buginsight_tests_lock_unit_tests_md> --test-case=${test})
      set_property (TEST ${test} PROPERTY WILL_FAIL TRUE)
      set_property (TEST ${test} PROPERTY LABELS md_lock_tests_exit_66)

  elseif(test MATCHES ".*_exit_0")
      message( TRACE "Test '${test} WILL_FAIL FALSE" )
      add_test (NAME ${test} COMMAND buginsight -- $<TARGET_FILE:buginsight_tests_lock_unit_tests_md> --test-case=${test})
      set_property (TEST ${test} PROPERTY LABELS md_lock_tests_exit_0)
  else()
   message( FATAL_ERROR "Test '${test} does not have _exit_0 or _exit_66" )
  endif()

endforeach()


set (test_deadlock_output_to_stdout_passRegex "BugInsight: Potential deadlock at mutex:")
add_test (NAME test_deadlock_output_to_stdout COMMAND buginsight --on_error=fail --thread_watchdog_max_duration_sec=5 -- $<TARGET_FILE:buginsight_tests_lock_unit_tests_md> --test-case=test_bad_thread_sanitzer_example_exit_66)
set_property (TEST test_deadlock_output_to_stdout PROPERTY WILL_FAIL TRUE)
set_property (TEST test_deadlock_output_to_stdout PROPERTY PASS_REGULAR_EXPRESSION "${test_deadlock_output_to_stdout_passRegex}")

add_test (NAME test_cs_too_many_locks_perh_thread_200 COMMAND buginsight --on_error=fail --thread_watchdog_max_duration_sec=5 -- $<TARGET_FILE:buginsight_tests_lock_unit_tests_md> --test-case=test_cs_too_many_locks_perh_thread_200)
set (test_cs_too_many_locks_perh_thread_200_output "already holds 20 locks. Max number of hold locks per thread reached")
set_property (TEST test_cs_too_many_locks_perh_thread_200 PROPERTY PASS_REGULAR_EXPRESSION "${test_cs_too_many_locks_perh_thread_200_output}")

add_test (NAME test_cs_too_many_locks_perh_thread_200_2 COMMAND buginsight --on_error=fail --thread_watchdog_max_duration_sec=5 -- $<TARGET_FILE:buginsight_tests_lock_unit_tests_md> --test-case=test_cs_too_many_locks_perh_thread_200)
set_property (TEST test_cs_too_many_locks_perh_thread_200_2 PROPERTY WILL_FAIL TRUE)



################## leak tests ####################

set (leak_tests_md
malloc_tests.basic_exit_0
malloc_tests.basic_dbg_exit_0
malloc_tests.size_too_large_exit_0
malloc_tests.calloc_uint64_t_exit_0
malloc_tests.test_api_realloc_resize_10_to_20_exit_0
malloc_tests.test_api_realloc_dbg_resize_10_to_20_exit_0
malloc_tests.test_api_realloc_resize_10_to_5_exit_0
malloc_tests.test_api_realloc_dbg_resize_10_to_5_exit_0
malloc_tests.test_api_realloc_size_zero_exit_0
malloc_tests.test_api_realloc_block_and_size_zero_exit_0
malloc_tests.test_api_realloc_dbg_block_and_size_zero_exit_0
malloc_tests.test_api_recalloc_block_and_size_zero_exit_0
malloc_tests.test_api_recalloc_dbg_block_and_size_zero_exit_0
malloc_tests.test_api_realloc_dbg_size_zero_exit_0
malloc_tests.test_api_calloc_resize_10_to_20_exit_0
malloc_tests.test_api__recalloc_recalloc_10_to_20_exit_0
malloc_tests.test_api__recalloc_recalloc_10_to_5_exit_0
malloc_tests.test_api_recalloc_dbg_resize_10_to_20_exit_0
malloc_tests.test_api_recalloc_dbg_resize_10_to_5_exit_0
malloc_tests._expand_exit_0
malloc_tests.test_api_malloc_free_zero_exit_0
malloc_tests.malloc_GetProcAddress_exit_0
malloc_tests.multithreaded_exit_0
aligned_malloc_tests.basic_exit_0
aligned_malloc_tests.basic_dbg_exit_0
aligned_malloc_tests.size_too_large_exit_0
aligned_malloc_tests.test_api_aligned_recalloc_uint64_t_exit_0
aligned_malloc_tests.test_api_aligned_offset_recalloc_uint64_t_exit_0
aligned_malloc_tests.test_api__aligned_realloc_exit_0
aligned_malloc_tests.test_api__aligned_offset_realloc_resize_10_20_exit_0
aligned_malloc_tests.test_api__aligned_offset_realloc_dbg_resize_10_20_exit_0
aligned_malloc_tests.test_api_realloc_dbg_resize_10_to_20_exit_0
aligned_malloc_tests.test_api_realloc_resize_10_to_5_exit_0
aligned_malloc_tests.test_api_realloc_dbg_resize_10_to_5_exit_0
aligned_malloc_tests.test_api_realloc_size_zero_exit_0
aligned_malloc_tests.test_api_realloc_dbg_size_zero_exit_0
aligned_malloc_tests.test_api_offset_realloc_size_zero_exit_0
aligned_malloc_tests.test_api_offset_realloc_dbg_size_zero_exit_0
aligned_malloc_tests.test_api_recalloc_size_zero_exit_0
aligned_malloc_tests.test_api_recalloc_dgb_size_zero_exit_0
aligned_malloc_tests.test_api_offset_recalloc_size_zero_exit_0
aligned_malloc_tests.test_api_offset_recalloc_dgb_size_zero_exit_0
aligned_malloc_tests.test_api_realloc_block_and_size_zero_exit_0
aligned_malloc_tests.test_api_realloc_dbg_block_and_size_zero_exit_0
aligned_malloc_tests.test_api_recalloc_block_and_size_zero_exit_0
aligned_malloc_tests.test_api_recalloc_dbg_block_and_size_zero_exit_0
aligned_malloc_tests.test_api_realloc_offset_block_and_size_zero_exit_0
aligned_malloc_tests.test_api_realloc_offse_dbg_block_and_size_zero_exit_0
aligned_malloc_tests.test_api_recalloc_offset_block_and_size_zero_exit_0
aligned_malloc_tests.test_api_recalloc_offset_dbg_block_and_size_zero_exit_0
aligned_malloc_tests.test_api__recalloc_recalloc_10_to_20_exit_0
aligned_malloc_tests.test_api_recalloc_dbg_resize_10_to_20_exit_0
aligned_malloc_tests.test_api__recalloc_recalloc_10_to_5_exit_0
aligned_malloc_tests.test_api_recalloc_dbg_resize_10_to_5_exit_0
aligned_malloc_tests.test_api__aligned_free_zero_exit_0
localalloc_tests.LocalAlloc_0_exit_0
localalloc_tests.LocalAlloc_100_exit_0
localalloc_tests.LocalAlloc_LMEM_ZEROINIT_100_exit_0
localalloc_tests.LocalAlloc_10000_exit_0
localalloc_tests.LocalReAlloc_1000_1010_exit_0
localalloc_tests.FormatMessageW_exit_0
localalloc_tests.FormatMessageA_exit_0
localalloc_tests.FormatMessageWNoAlloc_exit_0
localalloc_tests.FormatMessageANoAlloc_exit_0
localalloc_tests.CommandLineToArgvW_exit_0
localalloc_tests.ConvertStringSidToSidW_exit_0
localalloc_tests.ConvertStringSidToSidA_exit_0
localalloc_tests.GetNamedSecurityInfoW_exit_0
localalloc_tests.GetNamedSecurityInfoA_exit_0
localalloc_tests.CryptProtectData_exit_0
localalloc_tests.CryptProtectData_no_Descr_exit_0
localalloc_tests.SetEntriesInAclA_exit_0
localalloc_tests.SetEntriesInAclW_exit_0
localalloc_tests.AllocateAndInitializeSid_exit_0
localalloc_tests.GetSecurityInfo_exit_0
localalloc_tests.test_api_StrDupA_basic_exit_0
localalloc_tests.test_api_StrDupW_basic_exit_0
globalalloc_tests.GlobalAlloc_0_exit_0
globalalloc_tests.GlobalAlloc_100_exit_0
globalalloc_tests.GlobalAlloc_LMEM_ZEROINIT_100_exit_0
globalalloc_tests.GlobalAlloc_10000_exit_0
globalalloc_tests.GlobalReAlloc_1000_1010_exit_0
heapalloc_tests.HeapAlloc_0_exit_0
heapalloc_tests.HeapAlloc_100_exit_0
heapalloc_tests.HeapAlloc_ProcessHeap_exit_0
heapalloc_tests.HeapAlloc_10000_exit_0
heapalloc_tests.HeapAlloc_two_heaps_exit_0
heapalloc_tests.HeapReAlloc_1000_1010_exit_0
heapalloc_tests.HeapAlloc_GetProcAddress_exit_0
strdup_tests.test_api_strdup_basic_exit_0
strdup_tests.test_api_strdup_empty_exit_0
strdup_tests.test_api_underscore_strdup_basic_exit_0
strdup_tests.test_api_underscore_strdup_empty_exit_0
strdup_tests.test_api_wcsdup_basic_exit_0
strdup_tests.test_api_wcsdup_empty_exit_0
strdup_tests.test_api_underscore_wcsdup_basic_exit_0
strdup_tests.test_api_underscore_wcsdup_empty_exit_0
strdup_tests.test_api_strdup_zero_size_exit_0
strdup_tests.test_api_wcsdup_zero_size_exit_0
new_delete_tests.test_new_scalar_exit_0
new_delete_tests.test_new_array_exit_0
new_delete_tests.test_new_struct_exit_0
new_delete_tests.test_new_array_struct_exit_0
new_delete_tests.test_string_array_exit_0
new_delete_tests.test_operator_new_exit_0
new_delete_tests.test_operator_new_array_exit_0
new_delete_tests.test_operator_new_dbg_exit_0
new_delete_tests.test_operator_new_array_dbg_exit_0
new_delete_tests.test_std_vector_int_exit_0
new_delete_tests.test_std_vector_struct_exit_0
new_delete_tests.test_std_vector_leak_exit_0
new_delete_tests.test_non_virtual_dtor_exit_0
new_delete_tests.new_scalar_array_delete_exit_0
winapi_tests.test__getcwd_exit_0
winapi_tests.test__getcwd_dbg_exit_0
winapi_tests.test__wgetcwd_exit_0
winapi_tests.test__wgetcwd_dbg_exit_0
winapi_tests.test__getdcwd_exit_0
winapi_tests.test__getdcwd_dbg_exit_0
winapi_tests.test__wgetdcwd_exit_0
winapi_tests.test__wgetdcwd_dbg_exit_0
winapi_tests.test__tempnam_exit_0
winapi_tests.test__tempnam_dbg_exit_0
winapi_tests.test__wtempnam_exit_0
winapi_tests.test__wtempnam_dbg_exit_0
winapi_tests.test__dupenv_s_exit_0
winapi_tests.test__dupenv_dbg_s_exit_0
winapi_tests.test__wdupenv_s_exit_0
winapi_tests.test__wdupenv_s_dbg_exit_0
winapi_tests.test__fullpath_exit_0
winapi_tests.test__fullpath_dbg_exit_0
winapi_tests.test__fullpath_non_leak_exit_0
winapi_tests.test__wfullpath_exit_0
winapi_tests.test__wfullpath_dbg_exit_0
loadlibrary_tests.LoadLibraryTest_exit_0
loadlibrary_tests.LoadLibrarySameTwice_exit_0
loadlibrary_tests.LoadLibraryMassTest_exit_0
openssl_tests.Basic_exit_0
new_delete_bad_tests.new_array_free_with_free_exit_66
new_delete_bad_tests.new_scalar_double_delete_exit_66
new_delete_bad_tests.new_array_double_delete_exit_66
new_delete_bad_tests.new_array_delete_scalar_exit_66
new_delete_bad_tests.ImallocI1I4I_leak_66
new_delete_bad_tests.ImallocI2I8I_leak_66
new_delete_bad_tests.ImallocI1I16I_leak_66
new_delete_bad_tests.ImallocI1I24I_leak_66
new_delete_bad_tests.ImallocI1I200I_leak_66
malloc_bad_tests.malloc_double_free_exit_66
malloc_bad_tests._malloc_dbg_double_free_exit_66
malloc_bad_tests.realloc_double_free_exit_66
malloc_bad_tests._realloc_dgb_double_free_exit_66
malloc_bad_tests.realloc_with_freed_address_exit_66
malloc_bad_tests._realloc_dgb_with_freed_address_exit_66
malloc_bad_tests._recalloc_double_free_exit_66
malloc_bad_tests._recalloc_dbg_double_free_exit_66
malloc_bad_tests._recalloc_with_freed_address_exit_66
malloc_bad_tests._recalloc_dbg_with_freed_address_exit_66
malloc_bad_tests._expand_with_freed_address_exit_66
malloc_bad_tests._expand_dbg_with_freed_address_exit_66
malloc_bad_tests.ImallocI1I5000I_leak_66
malloc_bad_tests.ImallocI2I10000I_leak_66
malloc_bad_tests.ImallocI1I0I_leak_66
malloc_bad_tests.I_malloc_dbgI1I5000I_leak_66
malloc_bad_tests.I_malloc_dbgI1I0I_leak_66
malloc_bad_tests.IcallocI1I8I_leak_66
malloc_bad_tests.I_calloc_dbgI1I8I_leak_66
malloc_bad_tests.IreallocI1I5000I_leak_66
malloc_bad_tests.I_realloc_dbgI1I5000I_leak_66
malloc_bad_tests.I_recallocI1I20000I_leak_66
malloc_bad_tests.I_recalloc_dbgI1I20000I_leak_66
malloc_bad_tests.I_expandI1I5010I_leak_66
malloc_bad_tests.I_expand_dbgI1I5010I_leak_66
localalloc_bad_tests.LocalAlloc_global_free_exit_66
localalloc_bad_tests.LocalAlloc_double_free_exit_66
localalloc_bad_tests.LocalReAlloc_freed_pointer_exit_66
localalloc_bad_tests.ILocalAllocI1I500I_leak_66
localalloc_bad_tests.ILocalAllocI2I1000I_leak_66
localalloc_bad_tests.ILocalAllocI1I0I_leak_66
localalloc_bad_tests.ILocalReAllocI1I1010I_leak_66
global_alloc_bad_tests.GlobalAlloc_free_local_alloc_exit_66
global_alloc_bad_tests.GlobalAlloc_double_free_exit_66
global_alloc_bad_tests.GlobalReAlloc_freed_pointer_exit_66
global_alloc_bad_tests.IGlobalAllocI1I500I_leak_66
global_alloc_bad_tests.IGlobalAllocI2I1000I_leak_66
global_alloc_bad_tests.IGlobalAllocI1I0I_leak_66
global_alloc_bad_tests.IGlobalReAllocI1I1010I_leak_66
heapalloc_bad_tests.HeapAlloc_double_free_exit_66
heapalloc_bad_tests.HeapReAlloc_free_freed_pointer_exit_66
heapalloc_bad_tests.IHeapAllocI1I1000I_leak_66
heapalloc_bad_tests.IHeapAllocI2I2000I_leak_66
heapalloc_bad_tests.IHeapReAllocI1I1010I_leak_66
aligned_malloc_bad_tests._aligned_malloc_double_free_exit_66
aligned_malloc_bad_tests._aligned_malloc_dbg_double_free_exit_66
aligned_malloc_bad_tests._aligned_offset_malloc_double_free_exit_66
aligned_malloc_bad_tests._aligned_offset_malloc_dbg_double_free_exit_66
aligned_malloc_bad_tests._aligned_recalloc_deleted_pointer_exit_66
aligned_malloc_bad_tests._aligned_recalloc_dbg_deleted_pointer_exit_66
aligned_malloc_bad_tests._aligned_offset_recalloc_deleted_pointer_exit_66
aligned_malloc_bad_tests._aligned_offset_realloc_dbg_deleted_pointer_exit_66
aligned_malloc_bad_tests._aligned_realloc_deleted_pointer_exit_66
aligned_malloc_bad_tests._aligned_realloc_dbg_deleted_pointer_exit_66
aligned_malloc_bad_tests.I_aligned_mallocI1I500I_leak_66
aligned_malloc_bad_tests.I_aligned_mallocI2I1000I_leak_66
aligned_malloc_bad_tests.I_aligned_malloc_dbgI1I500I_leak_66
aligned_malloc_bad_tests.I_aligned_offset_mallocI1I500I_leak_66
aligned_malloc_bad_tests.I_aligned_offset_malloc_dbgI1I500I_leak_66
aligned_malloc_bad_tests.I_aligned_recallocI1I200I_leak_66
aligned_malloc_bad_tests.I_aligned_recalloc_dbgI1I200I_leak_66
aligned_malloc_bad_tests.I_aligned_offset_recallocI1I20000I_leak_66
aligned_malloc_bad_tests.I_aligned_offset_recalloc_dbgI1I20000I_leak_66
winapi_bad_tests.I_getcwdI1I24I_leak_66
winapi_bad_tests.I_getcwd_dbgI1I24I_leak_66
winapi_bad_tests.I_wgetcwdI1I48I_leak_66
winapi_bad_tests.I_wgetcwd_dbgI1I48I_leak_66
winapi_bad_tests.I_getdcwdI1I24I_leak_66
winapi_bad_tests.I_getdcwd_dbgI1I24I_leak_66
winapi_bad_tests.I_wgetdcwdI1I48I_leak_66
winapi_bad_tests.I_wgetdcwd_dbgI1I48I_leak_66
winapi_bad_tests.I_tempnamI1I45I_leak_66
winapi_bad_tests.I_tempnam_dbgI1I45I_leak_66
winapi_bad_tests.I_wtempnamI1I90I_leak_66
winapi_bad_tests.I_wtempnam_dbgI1I90I_leak_66
winapi_bad_tests.I_dupenv_sI1I11I_leak_66
winapi_bad_tests.I_dupenv_s_dbgI1I11I_leak_66
winapi_bad_tests.I_wdupenv_sI1I22I_leak_66
winapi_bad_tests.I_wdupenv_s_dbgI1I22I_leak_66
winapi_bad_tests.I_fullpathI1I29I_leak_66
winapi_bad_tests.I_fullpath_dbgI1I29I_leak_66
winapi_bad_tests.I_wfullpathI1I58I_leak_66
winapi_bad_tests.I_wfullpath_dbgI1I58I_leak_66
strdup_bad_tests.IstrdupI1I24I_leak_66
strdup_bad_tests.IstrdupI2I48I_leak_66
strdup_bad_tests.IstrdupI1I1I_leak_66
strdup_bad_tests.I_strdupI1I24I_leak_66
strdup_bad_tests.I_strdupI1I1I_leak_66
strdup_bad_tests.IwcsdupI1I48I_leak_66
strdup_bad_tests.IwcsdupI1I2I_leak_66
strdup_bad_tests.I_wcsdupI1I48I_leak_66
strdup_bad_tests.I_wcsdupI1I2I_leak_66
)

foreach (test ${leak_tests_md})
  if(test MATCHES ".*_exit_66")
      add_test (NAME ${test} COMMAND buginsight --on_error=fail -- $<TARGET_FILE:buginsight_tests_leak_unit_tests_md> --filter=${test})
      set_property (TEST ${test} PROPERTY WILL_FAIL TRUE)
      set_property (TEST ${test} PROPERTY LABELS md_leak_tests md_leak_tests_exit_66)
  elseif(test MATCHES ".*_leak_66")
      add_test (NAME ${test} COMMAND buginsight -- $<TARGET_FILE:buginsight_tests_leak_unit_tests_md> --filter=${test})
      set_property (TEST ${test} PROPERTY LABELS md_leak_tests md_leak_tests_leak_66)

      string(REPLACE "I" ";" VERSION_LIST ${test})
      list(GET VERSION_LIST 0 START)
      list(GET VERSION_LIST 1 FUNCTION_NAME)
      list(GET VERSION_LIST 2 CALL_COUNT)
      list(GET VERSION_LIST 3 EXPECTED_LEAKS_IN_BYTES)

      string(REPLACE "_dbg" "(_dbg)?" FUNCTION_NAME ${FUNCTION_NAME})
      string(REGEX REPLACE "^_" "_\?" FUNCTION_NAME ${FUNCTION_NAME})
      #message( "FUNCTION_NAME  '${FUNCTION_NAME}'" )

      set(regex_string "${FUNCTION_NAME} *\\| *${CALL_COUNT}\\| *${EXPECTED_LEAKS_IN_BYTES}")

      set_property (TEST ${test} PROPERTY PASS_REGULAR_EXPRESSION "${regex_string}")
  elseif(test MATCHES ".*_exit_0")

    add_test (NAME ${test} COMMAND buginsight -- $<TARGET_FILE:buginsight_tests_leak_unit_tests_md> --filter=${test})
      set_property (TEST ${test} PROPERTY LABELS md_leak_tests md_leak_tests_exit_0)
      set(regex_string "Memory leaks found:         No")
      set_property (TEST ${test} PROPERTY PASS_REGULAR_EXPRESSION "${regex_string}")

  else()
   message( FATAL_ERROR "Test '${test} does not have valid ending" )
  endif()

endforeach()


set (regex_exe_leaks "\
malloc                        \\|        46\\|                7659[\r\n\t ]*\
_expand                       \\|         7\\|                7210[\r\n\t ]*\
_aligned_offset_malloc        \\|         7\\|                7007[\r\n\t ]*\
_aligned_malloc               \\|         7\\|                7000[\r\n\t ]*\
_recalloc                     \\|        14\\|                2800[\r\n\t ]*\
calloc                        \\|         7\\|                1400[\r\n\t ]*\
realloc                       \\|        14\\|                 770[\r\n\t ]*\
GlobalAlloc                   \\|         7\\|                 728[\r\n\t ]*\
LocalAlloc                    \\|         7\\|                 721[\r\n\t ]*\
_aligned_realloc              \\|         7\\|                 714[\r\n\t ]*\
HeapAlloc                     \\|         7\\|                 707[\r\n\t ]*\
_wtempnam                     \\|         7\\|                 636[\r\n\t ]*\
_wgetdcwd                     \\|         7\\|                 336[\r\n\t ]*\
_wgetcwd                      \\|         7\\|                 336[\r\n\t ]*\
_tempnam                      \\|         7\\|                 318[\r\n\t ]*\
_wcsdup                       \\|         7\\|                 196[\r\n\t ]*\
_getdcwd                      \\|         7\\|                 168[\r\n\t ]*\
_getcwd                       \\|         7\\|                 168[\r\n\t ]*\
_aligned_recalloc             \\|         7\\|                 140[\r\n\t ]*\
_strdup                       \\|         7\\|                  84")

#message ("regex_exe_leaks: ${regex_exe_leaks}")

add_test (NAME buginsight_tests_leak_exe_md_test COMMAND buginsight --on_error=fail_on_exit --thread_watchdog_max_duration_sec=5 -- $<TARGET_FILE:buginsight_tests_leak_exe_md>)
set_property (TEST buginsight_tests_leak_exe_md_test PROPERTY PASS_REGULAR_EXPRESSION "${regex_exe_leaks}")


set (regex_exe_leaks_with_dll "\
malloc                        \\|        41\\|                6490[\r\n\t ]*\
_expand                       \\|         6\\|                6180[\r\n\t ]*\
_aligned_offset_malloc        \\|         6\\|                6006[\r\n\t ]*\
_aligned_malloc               \\|         6\\|                6000[\r\n\t ]*\
_recalloc                     \\|        12\\|                2400[\r\n\t ]*\
calloc                        \\|         6\\|                1200[\r\n\t ]*\
realloc                       \\|        12\\|                 660[\r\n\t ]*\
GlobalAlloc                   \\|         6\\|                 624[\r\n\t ]*\
LocalAlloc                    \\|         6\\|                 618[\r\n\t ]*\
_aligned_realloc              \\|         6\\|                 612[\r\n\t ]*\
HeapAlloc                     \\|         6\\|                 606[\r\n\t ]*\
_wtempnam                     \\|         6\\|                 544[\r\n\t ]*\
_wgetdcwd                     \\|         6\\|                 288[\r\n\t ]*\
_wgetcwd                      \\|         6\\|                 288[\r\n\t ]*\
_tempnam                      \\|         6\\|                 272[\r\n\t ]*\
_wcsdup                       \\|         6\\|                 168[\r\n\t ]*\
_getdcwd                      \\|         6\\|                 144[\r\n\t ]*\
_getcwd                       \\|         6\\|                 144[\r\n\t ]*\
_aligned_recalloc             \\|         6\\|                 120[\r\n\t ]*\
_strdup                       \\|         6\\|                  72")


add_test (NAME buginsight_tests_leak_exe_load_dll COMMAND buginsight --on_error=fail_on_exit --thread_watchdog_max_duration_sec=5 -- $<TARGET_FILE:buginsight_tests_leak_exe_load_dll>)
set_property (TEST buginsight_tests_leak_exe_load_dll PROPERTY PASS_REGULAR_EXPRESSION "${regex_exe_leaks_with_dll}")



