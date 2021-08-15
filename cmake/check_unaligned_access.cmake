set (_CHECK_UNALIGNED_PROG "
#include <stdlib.h>
#include <stdint.h>

int main()
{
        char* buf = (char*)malloc(sizeof(uint32_t)*2);
        uint32_t* ptrInt = (uint32_t*)buf;
        buf[0]=0x12;
        buf[1]=0x34;
        buf[2]=0x56;
        buf[3]=0x78;
        if (*ptrInt != 0x12345678 && *ptrInt != 0x78563412)
        {
                return 1;
        }

        buf = buf+1;
        ptrInt = (uint32_t*)buf;
        buf[0]=0x12;
        buf[1]=0x34;
        buf[2]=0x56;
        buf[3]=0x78;
        if (*ptrInt != 0x12345678 && *ptrInt != 0x78563412)
        {
                return 1;
        }

        buf = buf+1;
        ptrInt = (uint32_t*)buf;
        buf[0]=0x12;
        buf[1]=0x34;
        buf[2]=0x56;
        buf[3]=0x78;
        if (*ptrInt != 0x12345678 && *ptrInt != 0x78563412)
        {
                return 1;
        }

        buf = buf+1;
        ptrInt = (uint32_t*)buf;
        buf[0]=0x12;
        buf[1]=0x34;
        buf[2]=0x56;
        buf[3]=0x78;
        if (*ptrInt != 0x12345678 && *ptrInt != 0x78563412)
        {
                return 1;
        }

        return 0;
}")

include(CheckCXXSourceRuns)
CHECK_CXX_SOURCE_RUNS("${_CHECK_UNALIGNED_PROG}" _UNALIGNED_ACCESS_RESULT)
if (_UNALIGNED_ACCESS_RESULT EQUAL 1)
  set(CRASH_ON_UNALIGNED_ACCESS FALSE)
  message("Unaligned memory access works.")
else()
  set(CRASH_ON_UNALIGNED_ACCESS TRUE)
  message("Unaligned memory access is not possible.")
endif()