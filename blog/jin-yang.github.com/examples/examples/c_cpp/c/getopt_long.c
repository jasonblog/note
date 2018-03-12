#include <stdio.h>
/* for getopt_long() */
#include <getopt.h>


int main(int argc, char *argv[]) {
    char ch;                   /* service variables */
    int long_opt_index = 0;
    int longval;
    char *my_argument;
    struct option long_options[] = {        /* long options array. Items are all caSe SensiTivE! */
        { "add", 0, NULL, 'a'   },      /* --add or -a  */
        { "back", 0, NULL, 'b'  },      /* --back or -b */
        { "check", 0, &longval, 'c' },  /* return 'c', or return 0 and set longval to 'c' if "check" is parsed */
        { "extra", 1, &longval, 'x' },
        { 0,    0,    0,    0   }       /* terminating -0 item */
    };


    while ((ch = getopt_long(argc, argv, "abchx:", long_options, &long_opt_index)) != -1) {
       switch (ch) {
           case 'a':   /* long_opt_index does not make sense for these */
               /* 'a' and '--add' are confused (aliased) */
               printf("Option a, or --add.\n");
               break;
           case 'b':
               /* 'b' and '--back' are confused (aliased) */
               printf("Option b, or --back.\n");
               break;
           case 'c':
               /* 'c' and '--check' are distinguished, but handled the same way */
               printf("Option c, not --check.\n");
               break;
           case 'x':
               my_argument = optarg;
               printf("Option x, not --extra. Argument %s.\n", my_argument);
               break;
           case 0:     /* this is returned for long options with option[i].flag set (not NULL). */
                       /* the flag itself will point out the option recognized, and long_opt_index is now relevant */
               switch (longval) {
                   case 'c':
                       /* '--check' is managed here */
                       printf("Option --check, not -c (Array index: %d).\n", long_opt_index);
                       break;
                   case 'x':
                       /* '--extra' is managed here */
                       my_argument = optarg;
                       printf("Option --extra, not -x (Array index: %d). Argument: %s.\n", long_opt_index, my_argument);
                       break;
                   /* there's no default here */
               }
               break;
           case 'h':   /* mind that h is not described in the long option list */
               printf("Usage: getopt-long [-a or --add] [-b or --back] [-c or --check] [-x or --extra]\n");
               break;
           default:
               printf("You, lamah!\n");
       }
    }

    return 0;
}
