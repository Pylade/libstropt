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
 *  User-destinated functions.
 *  @author Paul Bazin
 *  @date 2010
 *  @version 0.9-a2
 *
 *  This file contains the definitions of functions that are part of the
 *  API. Only the functions in this file should (and can) be called,
 *  except atropt. So, if you just want to use the API to write programs
 *  using Libstropt, you will find all the answers there. Please read
 *  the introduction text to getting started with Libstropt.
 */

#include "stropt.h"

void* smalloc(size_t);
void* srealloc(void*, size_t);

/** Creates an option structure.
 *  This function will alloc memory needed for a struct option,
 *  initialize it with the right values; and return a pointer to the
 *  newly created struct option on a success, and NULL on a failure. If
 *  the call success, you have to free the option structure allocated
 *  once you no longer need it. To do so, you have to call delete_option
 *  with the address of the pointer to the option structure as argument.
 *  @return A pointer to the newly allocated option structure, or NULL
 *  on a failure.
 */
struct option* new_option(void)
{
    struct option* opt = smalloc(sizeof *opt);
    if (opt)
    {
        opt->active=0;
        opt->takes_value=0;
        opt->valuec=0;
        opt->value=NULL;
        opt->short_act = smalloc(sizeof *opt->short_act);
        opt->short_unact = smalloc(sizeof *opt->short_unact);
        opt->long_act = smalloc(sizeof *opt->long_act);
        opt->long_unact = smalloc(sizeof *opt->long_unact);
        opt->valuev = smalloc(sizeof *opt->valuev);
        if (opt->short_act && opt->short_unact && opt->long_act && opt->long_unact && opt->valuev)
        {
            opt->short_act[0]='\0';
            opt->short_unact[0]='\0';
            opt->long_act[0]=NULL;
            opt->long_unact[0]=NULL;
            opt->valuev[0]=NULL;
        }
        else
        {
            free(opt->short_act);
            free(opt->short_unact);
            free(opt->long_act);
            free(opt->long_unact);
            free(opt->valuev);
            free(opt);
            opt=NULL;
        }
    }
    return opt;
}

/** Set the values for short options that activate or unactivate the
 *  option structure.
 *  This function tests the validity of the string passed as argument,
 *  and, if any, sets the option structure to fit to these requirements.
 *  If the option structure had already been set, then the preceding
 *  values will be discarded.
 *  @param[out] ptr A pointer to the option structure to be set.
 *  @param[in] act A string containing the short %option arguments
 *  which shall activate the option structure.
 *  @param[in] unact A string containing the short %option arguments
 *  which shall unactivate the option structure.
 *  @return 0 on a success; 1 if at least one of the strings contains
 *  invalid characters, in this case no operation is performed; -1 if an
 *  internal error occurs, in this case your program should set again
 *  the short options or exit.
 */
int set_short_options(struct option* ptr, const char* act, const char* unact)
{
    int r=0;
    int k;
    char ok=1;
    int i=strlen(act);
    int j=strlen(unact);
    free(ptr->short_act);
    free(ptr->short_unact);
    for (k=0;k<i;k++)
        if (!isgraph(act[k]) || act[k]=='-')
            ok=0;
    for (k=0;k<j;k++)
        if (!isgraph(unact[k]) || unact[k]=='-')
            ok=0;
    if (ok)
    {
        ptr->short_act = smalloc((sizeof *ptr->short_act)*(i+1));
        if (ptr->short_act)
            strcpy(ptr->short_act,act);
        else
            r=-1;
        ptr->short_unact = smalloc((sizeof *ptr->short_unact)*(j+1));
        if (ptr->short_unact)
            strcpy(ptr->short_unact,unact);
        else
            r=-1;
    }
    else
        r=1;
    return r;
}

/** Configures an option structure to match a long %option.
 *  This will add a new long %option argument which will activate or
 *  unactivate an option structure. It will make sure the string to use
 *  is valid; and update the option structure. If the string is illegal,
 *  the call will fail. You must also specify if the string activate or
 *  unactivate the option structure. The long_act and long_unact members
 *  of the option structure sould _never_ be updated in another way that
 *  calling this function.
 *  @param[out] ptr A pointer to the option structure to update.
 *  @param[in] act A boolean, which determines whether the used string
 *  shall be an activator (when true), or an unactivator (when false).
 *  @param[in] str The string that should be matched as a long %option
 *  argument.
 *  @return 0 on a success, -1 if an error occurs.
 */
int new_long_option(struct option* ptr, char act, const char* str)
{
    int r=0;
    int i=-1;
    const char*** long_;
    while (str[++i] != '\0')
        if (str[i] == '=')
            r=-1;
    if (!i)
        r=-1;
    i=0;
    if (!r)
    {
        const char** tmp;
        if (act)
            long_ = & ptr->long_act;
        else
            long_ = & ptr->long_unact;
        while ((*long_)[i++] != NULL);
        tmp = srealloc(*long_, (sizeof **long_)*(i+1));
        if (tmp)
        {
            *long_=tmp;
            (*long_)[i-1]=str;
            (*long_)[i]=NULL;
        }
        else
            r=-1;
    }
    return r;
}

/** Deletes safely an option structure.
 *  This function deletes safely the option structure which is specified
 *  as argument, freeing all members of the structure. It needs to take
 *  the address of the pointer used, because it will assign it to NULL,
 *  in order to prevent from any further use of the deleted structure.
 *  Consequently, you are not supposed to use multiples copies of the
 *  pointer to any option structure. If NULL is passed as pointer, no
 *  action is performed.
 *  @param[in,out] ptr The address of the pointer to the option
 *  structure.
 */
void delete_option(struct option** ptr)
{
    if (*ptr)
    {
        int i=0;
        free((*ptr)->short_act);
        free((*ptr)->short_unact);
        free((*ptr)->long_act);
        free((*ptr)->long_unact);
        while (((*ptr)->valuev)[i])
            free(((*ptr)->valuev)[i++]);
        free((*ptr)->valuev);
        free(*ptr);
        *ptr=NULL;
    }
}

/** Creates a table of option structures.
 *  This function should be used if you do know how many option
 *  structures you will need; and if you do not worry about accessing to
 *  the pointers to option structures only through an array. This
 *  function works calling new_option as it is necessary; and return a
 *  pointer to an array of struct option* on a success, and NULL on an
 *  error. If the call success, you will have to free the array and the
 *  structures allocated calling delete_option_table, as soon as you
 *  have done the job. If the call fails, there is no need to free
 *  anything.
 *  You should use this functions to create your of struct option*.
 *  @param[in] n Number of option structures you want to use.
 *  @return A pointer to an array of pointers to option structures, or
 *  NULL on a failure.
 */
struct option** new_option_table(int n)
{
    int i;
    struct option** opt_tab = smalloc((sizeof *opt_tab)*(n+1));
    if (opt_tab)
    {
        for (i=0;i<n;i++)
        {
            opt_tab[i] = new_option();
            if (!opt_tab[i])
            {
                delete_option_table(&opt_tab);
                return NULL;
            }
        }
        opt_tab[n]=NULL;
    }
    return opt_tab;
}

/** Deletes safely a table of option structures.
 *  This function deletes safely an array of option structures created
 *  with new_option_table. It takes the struct option** returned by
 *  new_option_structure’s adress as argument. If NULL is passed, no
 *  action is performed. Please note that you must not use it if you
 *  also use delete_option: then you will get memory leaks.
 *  @param[in,out] ptr The pointer to the array’s adress.
 */
void delete_option_table(struct option*** ptr)
{
    if (*ptr)
    {
        int i;
        for (i=0;(*ptr)[i]!=NULL;i++)
            delete_option(*ptr+i);
        free(*ptr);
        *ptr=NULL;
    }
}

void delete_return(struct rtrn** ptr)
{
    free((*ptr)->argsv);
    free((*ptr)->errsv);
    free((*ptr)->errsarg);
    free(*ptr);
    *ptr=NULL;
}

