#include <linux/compiler.h>
#include <linux/errno.h>
#include <linux/export.h>
#include <linux/fault-inject-usercopy.h>
#include <linux/kasan-checks.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/syscalls.h>
#include <linux/thread_info.h>
#include <linux/uaccess.h>

#define MAX_LENGTH 4096

/* Looks like there is no strcpy_from_user function in kernel... so i made it!
 */
long strcpy_from_user(char *dst, const char __user *src) {
  unsigned long max_addr, src_addr;
  long res = 0;

  might_fault();
  if (should_fail_usercopy())
    return -EFAULT;
  max_addr = user_addr_max();
  src_addr = (unsigned long)untagged_addr(src);
  if (likely(src_addr < max_addr)) {
    unsigned long max = max_addr - src_addr;
    if (user_read_access_begin(src, max)) {
      while (max) {
        char c;

        unsafe_get_user(c, src + res, efault);

        if (!c)
          return res;
        dst[res] = c;
        res++;
        max--;
      }
      user_read_access_end();
      return res;
    }
  }
efault:
  return -EFAULT;
}

SYSCALL_DEFINE0(current_addr) { return (long)current; }

SYSCALL_DEFINE1(add_note, const char __user *, str) {
  return strcpy_from_user(current->note, str);
}

SYSCALL_DEFINE0(delete_note) {
  memset(current->note, 0, sizeof(current->note));
  return 0;
}

SYSCALL_DEFINE1(show_note, const char __user *, str) {
  return copy_to_user((void *)str, current->note, strlen(current->note));
}

SYSCALL_DEFINE1(copy_note, const char __user *, str) {
  return copy_from_user(current->note, (void *)str, sizeof(current->note));
}
