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
 *  Main header to be included.
 *  @author Paul Bazin
 *  @date 2010
 *  @version 0.9-a2
 *
 *  This file constains the main declarations of structures and
 *  functions which are needed for use Libstropt in a program. It also
 *  includes needed headers. So this is the only file one should include
 *  when using Libstropt. Other declarations (but for internal use) can
 *  be found in atropt.h.
 */

/**
 *  @mainpage Getting Started
 *
 *  Libstropt: a free and easy to use library about %option parsing.
 *
 *  Libstropt’s purpose is simple: it is a tool which allows you process
 *  lots of options at a low cost. Libstropt is based on structures: for
 *  each option you want to include in your program, you create and
 *  configure a simple structure, specifying default values. After
 *  calling a function that will process all the command-line arguments,
 *  you can view if any option is set on or off, what is the assignated
 *  value if the option takes a value, and access to the list of
 *  arguments that are not options. Of course, Libstropt supports the
 *  ‘--’ flag. Libstropt also tells you where did the user call your
 *  program the wrong way, and briefly describes the error. Libopt is
 *  easy to learn and to use, and compliant with the most common Unix
 *  standard for the command-line options.
 *
 *  Any new option structure should be created by the new_option() call.
 *  If you do know how many options you will need, and you do not care
 *  about having the option structure pointers only accessibles through
 *  an array, then you can use the new_option_table() call; which
 *  creates the specified number of option structures, and then will
 *  return a table terminated by NULL containing pointers to each option
 *  structure.
 *
 *  Then you can modify the value for each structure item to your needs,
 *  except for long_act and long_unact. Those items should be updated
 *  using the new_long_option() call, specifying if you want an
 *  activator or an unactivator. If you do want add default values for
 *  valuev[] (nevertheless you should not have to), you must write the
 *  values into a valuev[] array created by memory allocation, and
 *  terminate it by NULL. Do not worry about valuec, its purpose is just
 *  to give you some indication. If no value for the option involved is
 *  passed trhough the command-line, valuec is not updated.
 *
 *  Any value passed through a parameter is ignored for each option
 *  which is unactivated by this parameter, as well as that value is
 *  actually assignated to any options which is activated by this
 *  parameter. For a short parameter unactivating an option, there are
 *  no assignement, the following argument is processed as a regular
 *  one, unless if this parameter is also activating any option. If so,
 *  the following argument is processed as value for option which have
 *  been activated, and just ignored for options which have been
 *  unactivated.
 *
 *  One parameter should not be both activator and unactivator for the
 *  same option. If so, then Libstropt behaves as if the parameter was
 *  only unactivator.
 *
 *  To launch the comparison of the command-line arguments against the
 *  options you declared, you should use the atropt() call.
 *
 *  As soon as you no longer need the rtrn structure returned by
 *  atropt(), you must free it using delete_return(). As soon as you no
 *  longer need an option structure, you must free it using
 *  delete_option(). If you have created the table of option structure
 *  pointers using new_option_table(), you must use delete_option_table
 *  instead; which will free all the option strucutre together.
 *
 *  We hope you will enjoy Libstropt.
 */

#ifndef H_STROPT
#define H_STROPT
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct option
{
    char active;
    char* short_act;
    const char** long_act;
    char* short_unact;
    const char** long_unact;
    char takes_value;
    char* value;
    char** valuev;
    int valuec;
};
struct rtrn
{
    int argsc;
    char** argsv;
    int errsc;
    const char** errsv;
    int* errsarg;
};

struct option* new_option(void);
int new_long_option(struct option*,char,const char*);
int set_short_options(struct option*,const char*,const char*);
void delete_option(struct option**);
struct option** new_option_table(int);
void delete_option_table(struct option***);
void delete_return(struct rtrn**);
struct rtrn* atropt(int,char**,struct option**);

#endif /* H_STROPT */

