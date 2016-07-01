#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

static char *progname;
static int lflag = 0, hflag = 0, count = -1;
int status = 1;

int main(int argc, char **argv)
{
    int i, c, mcount, status = 0;
    progname = argv[0];
    char mystr[500];

    FILE *fp;

    /* Get the options and set appropriate values based on 
    which options are selected */
    while ((c = getopt(argc, argv, "lhm:")) != EOF) {
        switch (c) {
        case 'l':
            lflag = 1;
            break;
        case 'h':
            hflag = 1;
            break;
        case 'm':
            count = atoi(optarg);
            break;
        default:
            status = 1;
            break;
        }
    }
    
    /* Check for invalid option arguments or no search string argument */
    if (status || optind == argc) {
        fprintf(stderr, "usage: %s [-lh] [-m count] searchstring [file ...]\n", 
            progname);
        status = 2;
        return(status);   
    }

    char *searchstring = argv[optind];

    /* Check if only a search string is given  */
    if ((argc - optind) == 1) {
        /* Continue to read from stdin and compare */
        while (1) {
            fgets(mystr, 500, stdin);
            if (strstr(mystr, searchstring) != NULL) {
                printf("%s", mystr);
            }
        }
    } else {    /* Case where there are arguments for input files */
        for ( i = 1; i < argc - optind; i++ ) {
            if (strcmp(argv[optind + i], "-") == 0) {
                fp = stdin;
            } else if ((fp = fopen(argv[optind + i], "r")) == NULL) {
                perror(argv[optind + i]);
                exit(2);
            }

            /* Reset max counter */
            mcount = count;

            while (!feof(fp)) { /* Loop until end of file */
                fgets(mystr, 500, fp);  /* Set mystr to the current line*/
                if (strstr(mystr, searchstring) != NULL) {
                    status = 0;
                    if (lflag) {
                        if (mcount != 0) {
                            printf("%s\n", argv[optind + i]);
                        }
                        break;
                    } else if (hflag || (argc == (optind + 2))) {
                        if (mcount < 0) {
                            printf("%s", mystr);
                        } else if (mcount > 0) {
                            printf("%s", mystr);
                            mcount--;
                        } else {
                            break;
                        }
                    } else {
                        if (mcount < 0) {
                            printf("%s:%s", argv[optind + i], mystr);
                        } else if (mcount > 0) {
                            printf("%s:%s", argv[optind + i], mystr);
                            mcount--;
                        } else {
                            break;
                        }
                    }
                }
            }
            fclose(fp);
        }
    }
    return(status);
}
