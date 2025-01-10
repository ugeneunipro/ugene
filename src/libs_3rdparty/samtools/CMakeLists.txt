project(samtools)

set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED Core LinguistTools)

include(${CMAKE_SOURCE_DIR}/cmake/Common.cmake)

include_directories(src)
include_directories(src/samtools_core/htslib)
include_directories(../../include)

add_definitions(-D_FILE_OFFSET_BITS=64)
add_definitions(-DPCRE2_CODE_UNIT_WIDTH=16)
add_definitions(-DHAVE_CONFIG_H)

include(Samtools-common.cmake)

#if (WIN32)
#    add_definitions(-DPTW32_STATIC_LIB)
#endif ()

file(GLOB SRCS
        src/samtools_core/htslib/cram/cram_codecs.c
        src/samtools_core/htslib/cram/cram_decode.c
        src/samtools_core/htslib/cram/cram_encode.c
        src/samtools_core/htslib/cram/cram_external.c
        src/samtools_core/htslib/cram/cram_index.c
        src/samtools_core/htslib/cram/cram_io.c
        src/samtools_core/htslib/cram/cram_stats.c
        src/samtools_core/htslib/cram/mFILE.c
        src/samtools_core/htslib/cram/open_trace_file.c
        src/samtools_core/htslib/cram/pooled_alloc.c
        src/samtools_core/htslib/cram/string_alloc.c
        src/samtools_core/htslib/htscodecs/htscodecs/arith_dynamic.c
        src/samtools_core/htslib/htscodecs/htscodecs/fqzcomp_qual.c
        src/samtools_core/htslib/htscodecs/htscodecs/htscodecs.c
        src/samtools_core/htslib/htscodecs/htscodecs/pack.c
        src/samtools_core/htslib/htscodecs/htscodecs/rANS_static.c
        src/samtools_core/htslib/htscodecs/htscodecs/rANS_static4x16pr.c
        src/samtools_core/htslib/htscodecs/htscodecs/rANS_static32x16pr.c
        src/samtools_core/htslib/htscodecs/htscodecs/rle.c
        src/samtools_core/htslib/htscodecs/htscodecs/tokenise_name3.c
        src/samtools_core/htslib/htscodecs/htscodecs/utils.c
        src/samtools_core/htslib/bgzf.c
        src/samtools_core/htslib/errmod.c
        src/samtools_core/htslib/header.c
        src/samtools_core/htslib/hfile.c
        src/samtools_core/htslib/hts.c
        src/samtools_core/htslib/hts_expr.c
        src/samtools_core/htslib/hts_os.c
        src/samtools_core/htslib/kstring.c
        src/samtools_core/htslib/md5.c
        src/samtools_core/htslib/multipart.c
        src/samtools_core/htslib/probaln.c
        src/samtools_core/htslib/realn.c
        src/samtools_core/htslib/region.c
        src/samtools_core/htslib/sam.c
        src/samtools_core/htslib/sam_mods.c
        src/samtools_core/htslib/tbx.c
        src/samtools_core/htslib/textutils.c
        src/samtools_core/htslib/thread_pool.c
        src/samtools_core/bam.c
        src/samtools_core/bam_aux.c
        src/samtools_core/bam_lpileup.c
        src/samtools_core/bam_plbuf.c
        src/samtools_core/bam_sort.c
        src/samtools_core/bam2bcf.c
        src/samtools_core/bedidx.c
        src/samtools_core/faidx.c
        src/samtools_core/sam_opts.c
        src/samtools_core/sam_utils.c
        src/SamtoolsAdapter.cpp
)

if (WIN32)
    file(GLOB WIN_SRCS
            src/samtools_core/htslib/win/pthread/autostatic.c
            src/samtools_core/htslib/win/pthread/cleanup.c
            src/samtools_core/htslib/win/pthread/create.c
            src/samtools_core/htslib/win/pthread/dll.c
            src/samtools_core/htslib/win/pthread/errno.c
            src/samtools_core/htslib/win/pthread/global.c
            src/samtools_core/htslib/win/pthread/pthread.c
            src/samtools_core/htslib/win/pthread/pthread_attr_destroy.c
            src/samtools_core/htslib/win/pthread/pthread_attr_getdetachstate.c
            src/samtools_core/htslib/win/pthread/pthread_attr_getinheritsched.c
            src/samtools_core/htslib/win/pthread/pthread_attr_getschedparam.c
            src/samtools_core/htslib/win/pthread/pthread_attr_getschedpolicy.c
            src/samtools_core/htslib/win/pthread/pthread_attr_getscope.c
            src/samtools_core/htslib/win/pthread/pthread_attr_getstackaddr.c
            src/samtools_core/htslib/win/pthread/pthread_attr_getstacksize.c
            src/samtools_core/htslib/win/pthread/pthread_attr_init.c
            src/samtools_core/htslib/win/pthread/pthread_attr_setdetachstate.c
            src/samtools_core/htslib/win/pthread/pthread_attr_setinheritsched.c
            src/samtools_core/htslib/win/pthread/pthread_attr_setschedparam.c
            src/samtools_core/htslib/win/pthread/pthread_attr_setschedpolicy.c
            src/samtools_core/htslib/win/pthread/pthread_attr_setscope.c
            src/samtools_core/htslib/win/pthread/pthread_attr_setstackaddr.c
            src/samtools_core/htslib/win/pthread/pthread_attr_setstacksize.c
            src/samtools_core/htslib/win/pthread/pthread_barrier_destroy.c
            src/samtools_core/htslib/win/pthread/pthread_barrier_init.c
            src/samtools_core/htslib/win/pthread/pthread_barrier_wait.c
            src/samtools_core/htslib/win/pthread/pthread_barrierattr_destroy.c
            src/samtools_core/htslib/win/pthread/pthread_barrierattr_getpshared.c
            src/samtools_core/htslib/win/pthread/pthread_barrierattr_init.c
            src/samtools_core/htslib/win/pthread/pthread_barrierattr_setpshared.c
            src/samtools_core/htslib/win/pthread/pthread_cancel.c
            src/samtools_core/htslib/win/pthread/pthread_cond_destroy.c
            src/samtools_core/htslib/win/pthread/pthread_cond_init.c
            src/samtools_core/htslib/win/pthread/pthread_cond_signal.c
            src/samtools_core/htslib/win/pthread/pthread_cond_wait.c
            src/samtools_core/htslib/win/pthread/pthread_condattr_destroy.c
            src/samtools_core/htslib/win/pthread/pthread_condattr_getpshared.c
            src/samtools_core/htslib/win/pthread/pthread_condattr_init.c
            src/samtools_core/htslib/win/pthread/pthread_condattr_setpshared.c
            src/samtools_core/htslib/win/pthread/pthread_delay_np.c
            src/samtools_core/htslib/win/pthread/pthread_detach.c
            src/samtools_core/htslib/win/pthread/pthread_equal.c
            src/samtools_core/htslib/win/pthread/pthread_exit.c
            src/samtools_core/htslib/win/pthread/pthread_getconcurrency.c
            src/samtools_core/htslib/win/pthread/pthread_getschedparam.c
            src/samtools_core/htslib/win/pthread/pthread_getspecific.c
            src/samtools_core/htslib/win/pthread/pthread_getunique_np.c
            src/samtools_core/htslib/win/pthread/pthread_getw32threadhandle_np.c
            src/samtools_core/htslib/win/pthread/pthread_join.c
            src/samtools_core/htslib/win/pthread/pthread_key_create.c
            src/samtools_core/htslib/win/pthread/pthread_key_delete.c
            src/samtools_core/htslib/win/pthread/pthread_kill.c
            src/samtools_core/htslib/win/pthread/pthread_mutex_consistent.c
            src/samtools_core/htslib/win/pthread/pthread_mutex_destroy.c
            src/samtools_core/htslib/win/pthread/pthread_mutex_init.c
            src/samtools_core/htslib/win/pthread/pthread_mutex_lock.c
            src/samtools_core/htslib/win/pthread/pthread_mutex_timedlock.c
            src/samtools_core/htslib/win/pthread/pthread_mutex_trylock.c
            src/samtools_core/htslib/win/pthread/pthread_mutex_unlock.c
            src/samtools_core/htslib/win/pthread/pthread_mutexattr_destroy.c
            src/samtools_core/htslib/win/pthread/pthread_mutexattr_getkind_np.c
            src/samtools_core/htslib/win/pthread/pthread_mutexattr_getpshared.c
            src/samtools_core/htslib/win/pthread/pthread_mutexattr_getrobust.c
            src/samtools_core/htslib/win/pthread/pthread_mutexattr_gettype.c
            src/samtools_core/htslib/win/pthread/pthread_mutexattr_init.c
            src/samtools_core/htslib/win/pthread/pthread_mutexattr_setkind_np.c
            src/samtools_core/htslib/win/pthread/pthread_mutexattr_setpshared.c
            src/samtools_core/htslib/win/pthread/pthread_mutexattr_setrobust.c
            src/samtools_core/htslib/win/pthread/pthread_mutexattr_settype.c
            src/samtools_core/htslib/win/pthread/pthread_num_processors_np.c
            src/samtools_core/htslib/win/pthread/pthread_once.c
            src/samtools_core/htslib/win/pthread/pthread_rwlock_destroy.c
            src/samtools_core/htslib/win/pthread/pthread_rwlock_init.c
            src/samtools_core/htslib/win/pthread/pthread_rwlock_rdlock.c
            src/samtools_core/htslib/win/pthread/pthread_rwlock_timedrdlock.c
            src/samtools_core/htslib/win/pthread/pthread_rwlock_timedwrlock.c
            src/samtools_core/htslib/win/pthread/pthread_rwlock_tryrdlock.c
            src/samtools_core/htslib/win/pthread/pthread_rwlock_trywrlock.c
            src/samtools_core/htslib/win/pthread/pthread_rwlock_unlock.c
            src/samtools_core/htslib/win/pthread/pthread_rwlock_wrlock.c
            src/samtools_core/htslib/win/pthread/pthread_rwlockattr_destroy.c
            src/samtools_core/htslib/win/pthread/pthread_rwlockattr_getpshared.c
            src/samtools_core/htslib/win/pthread/pthread_rwlockattr_init.c
            src/samtools_core/htslib/win/pthread/pthread_rwlockattr_setpshared.c
            src/samtools_core/htslib/win/pthread/pthread_self.c
            src/samtools_core/htslib/win/pthread/pthread_setaffinity.c
            src/samtools_core/htslib/win/pthread/pthread_setcancelstate.c
            src/samtools_core/htslib/win/pthread/pthread_setcanceltype.c
            src/samtools_core/htslib/win/pthread/pthread_setconcurrency.c
            src/samtools_core/htslib/win/pthread/pthread_setschedparam.c
            src/samtools_core/htslib/win/pthread/pthread_setspecific.c
            src/samtools_core/htslib/win/pthread/pthread_spin_destroy.c
            src/samtools_core/htslib/win/pthread/pthread_spin_init.c
            src/samtools_core/htslib/win/pthread/pthread_spin_lock.c
            src/samtools_core/htslib/win/pthread/pthread_spin_trylock.c
            src/samtools_core/htslib/win/pthread/pthread_spin_unlock.c
            src/samtools_core/htslib/win/pthread/pthread_testcancel.c
            src/samtools_core/htslib/win/pthread/pthread_timechange_handler_np.c
            src/samtools_core/htslib/win/pthread/pthread_timedjoin_np.c
            src/samtools_core/htslib/win/pthread/pthread_tryjoin_np.c
            src/samtools_core/htslib/win/pthread/pthread_win32_attach_detach_np.c
            src/samtools_core/htslib/win/pthread/ptw32_calloc.c
            src/samtools_core/htslib/win/pthread/ptw32_callUserDestroyRoutines.c
            src/samtools_core/htslib/win/pthread/ptw32_cond_check_need_init.c
            src/samtools_core/htslib/win/pthread/ptw32_getprocessors.c
            src/samtools_core/htslib/win/pthread/ptw32_is_attr.c
            src/samtools_core/htslib/win/pthread/ptw32_MCS_lock.c
            src/samtools_core/htslib/win/pthread/ptw32_mutex_check_need_init.c
            src/samtools_core/htslib/win/pthread/ptw32_new.c
            src/samtools_core/htslib/win/pthread/ptw32_processInitialize.c
            src/samtools_core/htslib/win/pthread/ptw32_processTerminate.c
            src/samtools_core/htslib/win/pthread/ptw32_relmillisecs.c
            src/samtools_core/htslib/win/pthread/ptw32_reuse.c
            src/samtools_core/htslib/win/pthread/ptw32_rwlock_cancelwrwait.c
            src/samtools_core/htslib/win/pthread/ptw32_rwlock_check_need_init.c
            src/samtools_core/htslib/win/pthread/ptw32_semwait.c
            src/samtools_core/htslib/win/pthread/ptw32_spinlock_check_need_init.c
            src/samtools_core/htslib/win/pthread/ptw32_threadDestroy.c
            src/samtools_core/htslib/win/pthread/ptw32_threadStart.c
            src/samtools_core/htslib/win/pthread/ptw32_throw.c
            src/samtools_core/htslib/win/pthread/ptw32_timespec.c
            src/samtools_core/htslib/win/pthread/ptw32_tkAssocCreate.c
            src/samtools_core/htslib/win/pthread/ptw32_tkAssocDestroy.c
            src/samtools_core/htslib/win/pthread/sched_get_priority_max.c
            src/samtools_core/htslib/win/pthread/sched_get_priority_min.c
            src/samtools_core/htslib/win/pthread/sched_getscheduler.c
            src/samtools_core/htslib/win/pthread/sched_setaffinity.c
            src/samtools_core/htslib/win/pthread/sched_setscheduler.c
            src/samtools_core/htslib/win/pthread/sched_yield.c
            src/samtools_core/htslib/win/pthread/sem_close.c
            src/samtools_core/htslib/win/pthread/sem_destroy.c
            src/samtools_core/htslib/win/pthread/sem_getvalue.c
            src/samtools_core/htslib/win/pthread/sem_init.c
            src/samtools_core/htslib/win/pthread/sem_open.c
            src/samtools_core/htslib/win/pthread/sem_post.c
            src/samtools_core/htslib/win/pthread/sem_post_multiple.c
            src/samtools_core/htslib/win/pthread/sem_timedwait.c
            src/samtools_core/htslib/win/pthread/sem_trywait.c
            src/samtools_core/htslib/win/pthread/sem_unlink.c
            src/samtools_core/htslib/win/pthread/sem_wait.c
            src/samtools_core/htslib/win/pthread/signal.c
            src/samtools_core/htslib/win/pthread/w32_CancelableWait.c
            src/samtools_core/htslib/win/sys/time.c
            src/samtools_core/htslib/win/unistd.c
    )

    list(APPEND SRCS ${WIN_SRCS})
endif ()

string(REPLACE "/RTC1" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_VERBOSE_MAKEFILE ON)

# Suppress original samtools warnings
if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
endif ()

add_library(samtools STATIC ${SRCS})

target_link_libraries(samtools
        Qt5::Core
#        U2Core
        zlib)

#target_compile_options(samtools PRIVATE
#        /we4013         # Treat implicit declarations as errors
#)