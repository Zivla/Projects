#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>

char *filename;
int status = 1;

/* Function used to check if current ent is appropriate to enter */
int is_dir_to_recurse(const char *parent, char *name)
{
    struct stat statbuf;

    /* Check to avoid infinite loop */
    if (!strcmp(".", name) || !strcmp("..", name)) {
        return 0;
    }
    char *path = malloc(strlen(name) + strlen(parent) + 2);
    sprintf(path, "%s/%s", parent, name);
    stat(path, &statbuf);
    return S_ISDIR(statbuf.st_mode);
}

static void list(const char *name)
{
    DIR *dp;
    struct dirent *ent;
    dp = opendir(name);
    
    /* Loop through directory */
    while ((ent = readdir(dp)) != NULL) {
        char *entry_name = ent->d_name;
        /* Print if its the wanted file name */
        if (!strcmp(entry_name, filename)) {
            printf("%s/%s\n", name, entry_name);
            status = 0;
        }
        if (is_dir_to_recurse(name, entry_name)) {
            char *next = malloc(strlen(name) + strlen(entry_name) + 2);
            sprintf(next, "%s/%s", name, entry_name);
            list(next);
            free(next);
        }
    }
    closedir(dp);
}

int main(int argc, char **argv)
{
    int i;
    filename = argv[1];
    struct stat statbuf;

    if (argc < 3 ) {
        fprintf(stderr, "usage: %s name dir ...\n", argv[0]);
        exit(2);
    }
    
    /* Loop through for valid directories */
    for (i = 2; i < argc; i++) {
        if(lstat(argv[i], &statbuf)) {
            perror(argv[i]);
            exit(2);
        } else if (S_ISDIR(statbuf.st_mode)) {
            list(argv[i]);
        }
    }   
    return(status);
}
