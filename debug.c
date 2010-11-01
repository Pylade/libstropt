#include <stdlib.h>
#include <stdio.h>
#include "stropt.h"
#include <unistd.h>
#include <sys/types.h>

void quit_stropt(struct option***);

int main(int argc,char** argv)
{
    struct option** optv;
    struct rtrn* ret1;
    int i=-1;
    /*
    printf("%d\n",(*((struct option**)NULL))->valuec);
    or
    delete_option((struct option**)NULL);
    → SIGSEGV, so to be fixed… improving the destructor.
    */
    srand(getpid());
    optv = new_option_table(3);
    if (optv)
    {
        int fail=0;
        fail |= set_short_options(optv[0],"d","");
        fail |= new_long_option(optv[0],0,"una");
        fail |= set_short_options(optv[1],"fi","r");
        optv[2]->takes_value = 2;
        fail |= set_short_options(optv[2],"i","");
        fail |= new_long_option(optv[2],1,"val");
        if (fail)
            quit_stropt(&optv);

        ret1 = atropt(argc,argv,optv);

        if (!ret1)
            quit_stropt(&optv);

        if (optv[0]->active)
            puts("optv[0] active!");
        if (optv[1]->active)
            puts("optv[1] active!");
        if (optv[2]->active)
            puts("optv[2] active!");

        while ((optv[2]->valuev)[++i]!=NULL)
            printf("value(%d): %s\n",i,optv[2]->valuev[i]);

        delete_option_table(&optv);

        i=-1;
        while((ret1->argsv)[++i]!=NULL)
            puts((ret1->argsv)[i]);

        puts("--- errors ---");
        i=-1;
        while((ret1->errsv)[++i]!=NULL)
            printf("%s: %s\n",argv[(ret1->errsarg)[i]],(ret1->errsv)[i]);

        delete_return(&ret1);
    }
    return 0;
}

void quit_stropt(struct option*** ptr)
{
    delete_option_table(ptr);
    exit(-1);
}

