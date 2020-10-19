#include <stdint.h> /* uint_least32_t */
#include <stddef.h> /* size_t */
#include <stdlib.h> /* EXIT_FAILURE */
#include <stdio.h> /* printf */
#include <unistd.h>  /* close */
#include <fcntl.h> /* open */
#include <sys/mman.h> /* mmap */
#include <cstring> /* strlen */
#include <thread> /* thread */
#include <string> /* string */


uint_least32_t crc32f(const unsigned char *buffer, size_t length);
uint_least32_t _crc32t(const unsigned char *buffer, size_t length);
void uncrc32(const unsigned char *buffer, size_t length, const uint_least32_t hash, const unsigned int leavel, std::string &tile, const unsigned int THRCNT, unsigned int id, int &return_length);



void perr(int status, const char *msg)
{
    printf("\n%s", msg);
    exit(EXIT_FAILURE);
}

void closefd(int fd)
{
    if (close(fd) == -1)
        perr(EXIT_FAILURE, "Failed to close handle!");
}

int main(int argc, char **argv)
{
    unsigned int leavel = 4;

    if (argc < 2)
    {
        perr(EXIT_SUCCESS,
            "\nUsage: UNCRC [<s:unhash>] <s:origin> [<u:leavel>]\n");
    }  
    else if (argc < 3)
    {
        if (access( argv[1], F_OK ) == -1)
            perr(EXIT_FAILURE, "Failed to access <unhash> file!");  
        
        int fd;
        size_t length;
        unsigned char *data;
        
        fd = open(argv[1], O_RDONLY);
        
        if (fd == -1)
            perr(EXIT_FAILURE, "Failed to open <unhash> handle!");
            
        length = lseek(fd, 0, SEEK_END);
        data = (unsigned char*)mmap(0, length, PROT_READ, MAP_PRIVATE, fd, 0); 

        if(data == MAP_FAILED)
        {
            closefd(fd);
            perr(EXIT_FAILURE, "Failed to map <unhash> handle!");
        }

        const uint_least32_t hash = crc32f(data, length);

        if (munmap(data, length) == -1)
        {
            closefd(fd);
            perr(EXIT_FAILURE, "Failed to unmap <unhash> handle!");
        }

        closefd(fd);
        
        printf("\n[crc32           = %x]\n", hash);
        exit(EXIT_SUCCESS);
    }
    else if (argc > 3)
    {
        leavel = atol(argv[3]);
        
        if (leavel == 0)
            perr(EXIT_FAILURE, "\nInvaild leavel!\n");
    } 
    
    if (access( argv[1], F_OK ) == -1)
        perr(EXIT_FAILURE, "Failed to access <unhash> file!");
    
    if (access( argv[1], F_OK ) == -1)
        perr(EXIT_FAILURE, "Failed to access <origin> file!");
    
    printf("\n[unhash filename = \"%s\"]", argv[1]);
    printf("\n[ogigin filename = \"%s\"]", argv[2]);
    printf("\n[input leavel    = %d]", leavel);
    
    int fd;
    size_t length;
    unsigned char *data;
    
    fd = open(argv[2], O_RDONLY);
    
    if (fd == -1)
        perr(EXIT_FAILURE, "Failed to open <unhash> handle!");
        
    length = lseek(fd, 0, SEEK_END);
    data = (unsigned char*)mmap(0, length, PROT_READ, MAP_PRIVATE, fd, 0); 

    if(data == MAP_FAILED)
    {
        closefd(fd);
        perr(EXIT_FAILURE, "Failed to map <unhash> handle!");
    }

    const uint_least32_t hash = _crc32t(data, length);
    
    printf("\n[needed crc32    = %x]", hash ^ 0xFFFFFFFF);

    if (munmap(data, length) == -1)
    {
        closefd(fd);
        perr(EXIT_FAILURE, "Failed to unmap <unhash> handle!");
    }

    closefd(fd);
    
    fd = open(argv[1], O_RDONLY);
    
    if (fd == -1)
        perr(EXIT_FAILURE, "Failed to open <origin> handle!");
    
    length = lseek(fd, 0, SEEK_END);
    data = (unsigned char*)mmap(0, length, PROT_READ, MAP_PRIVATE, fd, 0); 
    
    printf("\n[currnt crc32    = %x]", _crc32t(data, length) ^ 0xFFFFFFFF);
    
    if(data == MAP_FAILED)
    {
        closefd(fd);
        perr(EXIT_FAILURE, "Failed to map <unhash> handle!");
    }
    
    const unsigned int THRCNT = std::thread::hardware_concurrency();
    printf("\n[threads         = %d]", THRCNT);  
    
    std::string tile[THRCNT];
    int append_length[THRCNT] = {0};  
    std::thread threads[THRCNT];
    
    printf("\n");
    
    for (unsigned int i = 0; i < THRCNT; ++i)
    {
        threads[i] = std::thread([&, i](){
            uncrc32(data, length, hash, leavel, std::ref(tile[i]), THRCNT, i, std::ref(append_length[i]));            
        });
    }
    
    for (unsigned int i = 0; i < THRCNT; ++i)
    {
        if (threads[i].joinable()) 
            threads[i].join();
    }

    closefd(fd);
    
    int id = -1;
    
    for (unsigned int i = 0; i < THRCNT; ++i)
    {
        if (append_length[i] > 0)
        {
            printf("\n[found index     = %u]", i);
            printf("\n[append length   = %d]", append_length[i]);
            id = i;
        }
    }
    
    if (append_length[id] <= 0)
    {
        printf("[result          = %s]", 
            append_length[id] == 0 ? "done" : "faill");
        
        if (munmap(data, length) == -1)
            perr(EXIT_FAILURE, "\nFailed to unmap <unhash> handle!");
        
        perr(EXIT_SUCCESS, "\n");
    }

    char *newfile = (char*)malloc(strlen(argv[1]) + 5);
    sprintf(newfile,"%s%s", argv[1], ".crc");
    printf("\n[new file name   = \"%s\"]", newfile);
        
    fd = open(newfile, O_WRONLY | O_TRUNC | O_CREAT, 0644);
    free(newfile);
    
    if (fd == -1)
        perr(EXIT_FAILURE, "Failed to create new file!");
    
    if(write(fd, data, length) != length)
    {
        closefd(fd);
        perr(EXIT_FAILURE, "Failed write to new file!");  
    }
    
    if (munmap(data, length) == -1)
    {
        closefd(fd);
        perr(EXIT_FAILURE, "Failed to unmap <unhash> handle!");
    }
    
    if (write(fd, tile[id].c_str(), tile[id].length()) != append_length[id])
    {
        closefd(fd);
        perr(EXIT_FAILURE, "Failed append to new file!");  
    }
    
    printf("\n[result          = done]\n");
    
    closefd(fd);
    
    return EXIT_SUCCESS;
}