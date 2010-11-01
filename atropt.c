/*
 *  Libstropt: an easy to use library about command-line options parsing
 *  Copyright (C) 2010 Paul Bazin
 *
 *  This file is part of Libstrotp.
 *
 *  This libray is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 *  @file
 *  Internal functions.
 *  The functions defined in this file, except atropt, are not part of
 *  the API. Your program should not (and therefore can not) call them.
 *  They are documented there only for hacking purposes. All those
 *  functions are used by atropt to perform the %option parsing.
 */

#include "stropt.h"
#include "atropt.h"

/* Begin debug functions */

void* smalloc(size_t);
void* srealloc(void*, size_t);

void* smalloc(size_t size)
{
    void* ret;
    if(rand()%999999)
        ret=malloc(size);
    else
        ret=NULL;
    return ret;
}

void* srealloc(void* ptr, size_t size)
{
    void* ret;
    if(rand()%999999)
        ret=realloc(ptr,size);
    else
        ret=NULL;
    return ret;
}

/* End debug functions */

static int str2cnt(const char* s1, const char* s2)
{
    int i=0;
    while (s1[i]==s2[i] && s1[i]!='\0')
        i++;
    if (s1[i] != s2[i])
        i=0;
    return i;
}

static int deleq(char* str)
{
    int i=-1;
    while (str[++i] != '\0')
        if (str[i] == '=')
        {
            str[i]='\0';
            return i;
        }
    return -1;
}

static struct rtrn* new_return(void)
{
    struct rtrn* ret = smalloc(sizeof *ret);
    if (ret)
    {
        ret->argsc=0;
        ret->errsc=0;
        ret->argsv = smalloc(sizeof *ret->argsv);
        ret->errsv = smalloc(sizeof *ret->errsv);
        ret->errsarg = smalloc(sizeof *ret->errsarg);
        if (ret->argsv && ret->errsv && ret->errsarg)
        {
            ret->argsv[0]=NULL;
            ret->errsv[0]=NULL;
            ret->errsarg[0]=0;
        }
        else
        {
            free(ret->argsv);
            free(ret->errsv);
            free(ret->errsarg);
            free(ret);
            ret=NULL;
        }
    }
    return ret;
}

static int new_return_arg(struct rtrn** ret, char* arg)
{
    int r=-1;
    int i=0;
    char** tmp;
    while ((*ret)->argsv[i++]);
    tmp = srealloc((*ret)->argsv, (sizeof *(*ret)->argsv)*(i+1));
    if (tmp)
    {
        r=0;
        (*ret)->argsv=tmp;
        (*ret)->argsv[i-1]=arg;
        (*ret)->argsv[i]=NULL;
        (*ret)->argsc=i;
    }
    return r;
}

static int new_return_error(struct rtrn** ret, const char* err, int arg)
{
    int r=-2;
    int i=0;
    const char** tmpv;
    int* tmparg;
    while ((*ret)->errsv[i++]);
    (*ret)->errsc=i;
    tmpv = srealloc((*ret)->errsv, (sizeof *(*ret)->errsv)*(i+1));
    if (tmpv)
    {
        r++;
        (*ret)->errsv=tmpv;
        (*ret)->errsv[i-1]=err;
        (*ret)->errsv[i]=NULL;
    }
    tmparg = srealloc((*ret)->errsarg, (sizeof *(*ret)->errsarg)*i);
    if (tmparg)
    {
        r++;
        (*ret)->errsarg=tmparg;
        (*ret)->errsarg[i-1]=arg;
    }
    return r;
}

static int give_value(const char* val, struct option* opt)
{
    int r=0;
    unsigned short i=0;
    if (opt->takes_value == 1)
    {
        free(opt->value);
        opt->value = smalloc((sizeof *opt->value) * (strlen(val)+1));
        if (opt->value)
            strcpy(opt->value, val);
        else
            r=-1;
    }
    else
    {
        char** tmp;
        while (opt->valuev[i++]);
        tmp = srealloc(opt->valuev, (sizeof *opt->valuev)*(i+1));
        if (tmp)
        {
            opt->valuev=tmp;
            (opt->valuev)[i-1] = smalloc((sizeof *(opt->valuev)[i-1]) * (strlen(val)+1));
            if ((opt->valuev)[i-1])
                strcpy((opt->valuev)[i-1], val);
            else
                r=-1;
            (opt->valuev)[i]=NULL;
            opt->valuec=i;
        }
        else
            r=-1;
    }
    return r;
}

static int short_activate(const char* next, struct option* opt)
{
    int r=0;
    opt->active=1;
    if (next)
    {
        if (opt->takes_value)
        {
            if (next[0] != '-' && next[0])
                r=1;
            else if (!next[1])
                r=1;
            if (r)
                if (give_value(next, opt))
                    r=-1;
        }
    }
    else
        r=1;
    return r;
}

static int long_activate(const char* arg, int eq, struct option* opt)
{
    int r=0;
    opt->active=1;
    if (eq)
    {
        if (opt->takes_value)
        {
            r = give_value(arg+eq, opt);
        }
    }
    return r;
}

static int atrshortopt(char last, const char** argv, int argn, char charopt, struct option** optv, struct rtrn** ret)
{
    int r=1;
    char yet=0;
    unsigned short optn;
    for (optn=0;optv[optn];optn++)
    {
        short shn;
        for (shn=0;optv[optn]->short_act[shn];shn++)
            if (optv[optn]->short_act[shn] == charopt)
            {
                const char* next;
                int status;
                yet=1;
                if (last)
                    next = (const char*) argv[argn+1];
                else
                    next = NULL;
                status = short_activate(next, optv[optn]);
                if (!status)
                    r=0;
                else if (status == -1)
                    r=-1;
                break;
            }
        if (r == -1)
            break;
        for (shn=0;optv[optn]->short_unact[shn];shn++)
            if (optv[optn]->short_unact[shn] == charopt)
            {
                yet=1;
                optv[optn]->active=0;
                break;
            }
    }
    if (!yet)
        if (new_return_error(ret, "no option matched", argn))
            r=-1;
    return r;
}

static int atrlongopt(char** argv, int argn, struct option** optv, struct rtrn** ret)
{
    int r=0;
    unsigned short optn;
    char ok=1;
    char yet=0;
    int eq=deleq(argv[argn]+2);
    if (!eq)
    {
        r=1;
        if (new_return_error(ret, "illegal '='", argn))
            r=-1;
    }
    else
    {
        for (optn=0;optv[optn]&&ok;optn++)
        {
            unsigned short lgn=0;
            while (optv[optn]->long_act[lgn] && ok)
                if (str2cnt(optv[optn]->long_act[lgn++], (const char*) argv[argn]+2))
                {
                    yet=1;
                    if (long_activate((const char*) argv[argn]+2, eq+1, optv[optn]))
                        ok=0;
                }
            lgn=0;
            while (optv[optn]->long_unact[lgn] && ok)
                if (str2cnt(optv[optn]->long_unact[lgn++], (const char*) argv[argn]+2))
                {
                    yet=1;
                    optv[optn]->active=0;
                }
        }
    }
    if (!ok)
        r=-1;
    else
        if (!yet)
            if (new_return_error(ret, "no option matched", argn))
                r=-1;
    if (eq != -1)
        (argv[argn]+2)[eq]='=';
    return r;
}

struct rtrn* atropt(int argc, char** argv, struct option** optv)
{
    char ok=1;
    char skip=0;
    unsigned short argn;
    struct rtrn* ret=new_return();
    if (ret)
    {
        for (argn=1;argn<argc&&ok;argn++)
        {
            if (argv[argn][0]=='-' && argv[argn][1] && !skip)
            {
                if (argv[argn][1] != '-')
                {
                    char jump=0;
                    unsigned short s_flag=0;
                    while (argv[argn][++s_flag] != '\0')
                    {
                        /* Test statements */
                        char last=0;
                        int status;
                        if (!argv[argn][s_flag+1])
                            last=1;
                        status = atrshortopt(last, (const char**) argv, argn, argv[argn][s_flag], optv, &ret);
                        if (status != -1)
                        {
                            if (!status)
                                jump=1;
                        }
                        else
                            ok=0;
                        /* End test statements */

                        /* short optn;
                        for (optn=0;optv[optn]!= NULL;optn++)
                        {
                            short shn;
                            for (shn=0;optv[optn]->short_unact[shn]!='\0';shn++)
                                if (optv[optn]->short_unact[shn] == argv[argn][s_flag])
                                    optv[optn]->active=0;
                            for (shn=0;optv[optn]->short_act[shn]!='\0';shn++)
                                if (optv[optn]->short_act[shn] == argv[argn][s_flag])
                                {
                                    optv[optn]->active=1;
                                    if (optv[optn]->takes_value == 1)
                                        next=optv[optn];
                                }
                        } */
                    }
                    if (jump)
                        argn++;
                }
                else
                {
                    if (argv[argn][2] == '\0')
                        skip=1;
                    else
                    {
                        if (atrlongopt(argv, argn, optv, &ret) == -1)
                            ok=0;
                    }
                }
            }
            else
            {
                if (new_return_arg(&ret, argv[argn]))
                    ok=0;
            }
        }
    }
    if (!ok)
        delete_return(&ret);
    return ret;
}

