include (breakpad.pri)

HEADERS += src/google_breakpad/common/breakpad_types.h \
           src/google_breakpad/common/minidump_cpu_amd64.h \
           src/google_breakpad/common/minidump_cpu_arm64.h \
           src/google_breakpad/common/minidump_cpu_arm.h \
           src/google_breakpad/common/minidump_cpu_mips.h \
           src/google_breakpad/common/minidump_cpu_ppc64.h \
           src/google_breakpad/common/minidump_cpu_ppc.h \
           src/google_breakpad/common/minidump_cpu_sparc.h \
           src/google_breakpad/common/minidump_cpu_x86.h \
           src/google_breakpad/common/minidump_exception_linux.h \
           src/google_breakpad/common/minidump_exception_mac.h \
           src/google_breakpad/common/minidump_exception_ps3.h \
           src/google_breakpad/common/minidump_exception_solaris.h \
           src/google_breakpad/common/minidump_exception_win32.h \
           src/google_breakpad/common/minidump_format.h \
           src/google_breakpad/common/minidump_size.h

win32 {
HEADERS += src/client/windows/common/ipc_protocol.h \
           src/client/windows/crash_generation/crash_generation_client.h \
           src/client/windows/handler/exception_handler.h \
           src/common/windows/guid_string.h \
           src/common/windows/string_utils-inl.h

SOURCES += src/client/windows/crash_generation/crash_generation_client.cc \
           src/client/windows/handler/exception_handler.cc \
           src/common/windows/guid_string.cc
} 

unix {
HEADERS += src/client/minidump_file_writer.h \
           src/client/minidump_file_writer-inl.h \
           src/common/linux/linux_libc_support.h \
           src/common/convert_UTF.h \
           src/common/macros.h \
           src/common/memory.h \
           src/common/memory_allocator.h \
           src/common/scoped_ptr.h \
           src/common/string_conversion.h \
           src/common/using_std_string.h

SOURCES += src/client/minidump_file_writer.cc \
           src/common/linux/linux_libc_support.cc \
           src/common/convert_UTF.cc \
           src/common/string_conversion.cc
}

unix_not_mac() {
HEADERS += src/client/linux/crash_generation/crash_generation_client.h \
           src/client/linux/dump_writer_common/mapping_info.h \
           src/client/linux/dump_writer_common/raw_context_cpu.h \
           src/client/linux/dump_writer_common/seccomp_unwinder.h \
           src/client/linux/dump_writer_common/thread_info.h \
           src/client/linux/dump_writer_common/ucontext_reader.h \
           src/client/linux/handler/exception_handler.h \
           src/client/linux/handler/microdump_extra_info.h \
           src/client/linux/handler/minidump_descriptor.h \
           src/client/linux/log/log.h \
           src/client/linux/microdump_writer/microdump_writer.h \
           src/client/linux/minidump_writer/cpu_set.h \
           src/client/linux/minidump_writer/directory_reader.h \
           src/client/linux/minidump_writer/line_reader.h \
           src/client/linux/minidump_writer/linux_dumper.h \
           src/client/linux/minidump_writer/linux_ptrace_dumper.h \
           src/client/linux/minidump_writer/minidump_writer.h \
           src/client/linux/minidump_writer/proc_cpuinfo_reader.h \
           src/common/basictypes.h \
           src/common/linux/eintr_wrapper.h \
           src/common/linux/elf_gnu_compat.h \
           src/common/linux/elfutils-inl.h \
           src/common/linux/elfutils.h \
           src/common/linux/file_id.h \
           src/common/linux/guid_creator.h \
           src/common/linux/ignore_ret.h \
           src/common/linux/memory_mapped_file.h \
           src/common/linux/safe_readlink.h \
           src/common/memory_range.h \
           src/common/minidump_type_helper.h \
           src/third_party/lss/linux_syscall_support.h

SOURCES += src/client/linux/crash_generation/crash_generation_client.cc \
           src/client/linux/dump_writer_common/seccomp_unwinder.cc \
           src/client/linux/dump_writer_common/thread_info.cc \
           src/client/linux/dump_writer_common/ucontext_reader.cc \
           src/client/linux/handler/exception_handler.cc \
           src/client/linux/handler/minidump_descriptor.cc \
           src/client/linux/log/log.cc \
           src/client/linux/microdump_writer/microdump_writer.cc \
           src/client/linux/minidump_writer/linux_dumper.cc \
           src/client/linux/minidump_writer/linux_ptrace_dumper.cc \
           src/client/linux/minidump_writer/minidump_writer.cc \
           src/common/linux/elfutils.cc \
           src/common/linux/file_id.cc \
           src/common/linux/guid_creator.cc \
           src/common/linux/memory_mapped_file.cc \
           src/common/linux/safe_readlink.cc
}

macx {
HEADERS += src/client/mac/crash_generation/client_info.h \
           src/client/mac/crash_generation/crash_generation_client.h \
           src/client/mac/crash_generation/crash_generation_server.h \
           src/client/mac/handler/breakpad_nlist_64.h \
           src/client/mac/handler/dynamic_images.h \
           src/client/mac/handler/exception_handler.h \
           src/client/mac/handler/mach_vm_compat.h \
           src/client/mac/handler/minidump_generator.h \
           src/client/mac/handler/protected_memory_allocator.h \
           src/client/mac/handler/ucontext_compat.h \
           src/common/mac/bootstrap_compat.h \
           src/common/mac/byteswap.h \
           src/common/mac/file_id.h \
           src/common/mac/macho_id.h \
           src/common/mac/macho_utilities.h \
           src/common/mac/macho_walker.h \
           src/common/mac/scoped_task_suspend-inl.h \
           src/common/mac/string_utilities.h \
           src/common/md5.h

SOURCES += src/client/mac/crash_generation/crash_generation_client.cc \
           src/client/mac/crash_generation/crash_generation_server.cc \
           src/client/mac/handler/breakpad_nlist_64.cc \
           src/client/mac/handler/dynamic_images.cc \
           src/client/mac/handler/exception_handler.cc \
           src/client/mac/handler/protected_memory_allocator.cc \
           src/common/mac/bootstrap_compat.cc \
           src/common/mac/file_id.cc \
           src/common/mac/macho_id.cc \
           src/common/mac/macho_utilities.cc \
           src/common/mac/macho_walker.cc \
           src/common/mac/string_utilities.cc \
           src/common/md5.cc

OBJECTIVE_HEADERS += src/common/mac/MachIPC.h

OBJECTIVE_SOURCES += src/client/mac/handler/minidump_generator.cc \
                     src/common/mac/MachIPC.mm
}
