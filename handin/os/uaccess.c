#include "vm.h"
#include "riscv.h"
#include "memlayout.h"
#include "string.h"

static int access_ok(struct mm* mm, uint64 addr, uint64 len) {
	if (!IS_USER_VA(addr) || addr + len < addr || !IS_USER_VA(addr + len - 1))
		return 0;
	return 1;
} 

static void begin_user_access() {
	w_sstatus(r_sstatus() | SSTATUS_SUM);
}

static void end_user_access() {
	w_sstatus(r_sstatus() & ~SSTATUS_SUM);
}

int copy_to_user(struct mm* mm, uint64 __user dstva, char *src, uint64 len) {
    if(access_ok(mm, dstva, len)) {
        begin_user_access();
        memmove((void *)dstva, src, len);
        end_user_access();
        return 0;
    }else{
        return -EINVAL;
    }
}

int copy_from_user(struct mm* mm, char *dst, uint64 __user srcva, uint64 len){
    if(access_ok(mm, srcva, len)) {
        begin_user_access();
        memmove(dst, (void *)srcva, len);
        end_user_access();
        return 0;
    }else{
        return -EINVAL;
    }
}

// Copy a null-terminated string from user to kernel.
// Copy bytes to dst from virtual address srcva in a given page table,
// until a '\0', or max.
// Return 0 on success, -1 on error.
int copystr_from_user(struct mm *mm, char *dst, uint64 __user srcva, uint64 max) {
    uint64 n, va0, pa0, kva0;
    int got_null = 0, len = 0;

    while (got_null == 0 && max > 0) {
        va0 = PGROUNDDOWN(srcva);
        pa0 = walkaddr(mm, va0);
        if (pa0 == 0)
            return -EINVAL;
        n = PGSIZE - (srcva - va0);
        if (n > max)
            n = max;

        kva0    = PA_TO_KVA(pa0);
        char *p = (char *)(kva0 + (srcva - va0));
        while (n > 0) {
            if (*p == '\0') {
                *dst     = '\0';
                got_null = 1;
                break;
            } else {
                *dst = *p;
            }
            --n;
            --max;
            p++;
            dst++;
            len++;
        }

        srcva = va0 + PGSIZE;
    }
    if (got_null) {
        return 0;
    } else {
        return -1;
    }
}
