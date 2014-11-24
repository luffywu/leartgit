/*
 * =====================================================================================
 *
 *       Filename:  remove_sht.c
 *
 *    Description:  remove section header table
 *
 *        Version:  1.0
 *        Created:  2014/11/24 15时19分42秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  luffy
 *   Organization:  
 *
 * =====================================================================================
 */
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <elf.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
typedef unsigned char byte;

int RemoveSHT(const char *path) {
    int fd = 0;
    struct stat st;
    void *pMap = NULL;
    byte *pbyMap = NULL;
    Elf32_Ehdr *pEhdr = NULL;
    int newSize = 0;


    fd = open(path, O_RDWR);
    if(fd <= 0) {
        fprintf(stderr, "open error\n");
        goto err1;
    }

    if(fstat(fd, &st) == -1) {
        fprintf(stderr, "fstat error\n");
        goto err2;
    }

    pMap = mmap(NULL, st.st_size,
            PROT_READ|PROT_WRITE, MAP_SHARED,
            fd, 0);
    if(pMap == NULL) {
        fprintf(stderr, "mmap error\n");
        goto err2;
    }
    pbyMap = (byte *)pMap;

    pEhdr = (Elf32_Ehdr *)pMap;
    newSize = pEhdr->e_shoff;
    pEhdr->e_shnum = 0;
    pEhdr->e_shstrndx = 0;
    pEhdr->e_shoff = 0;
    munmap(pMap, st.st_size);
    ftruncate(fd, newSize);
    close(fd);
    return 0;
err2:
    close(fd);
err1:
    return -1;
}

int main(int argc, char **argv) {
    if(argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
    }
    RemoveSHT(argv[1]);
    return 0;
}
