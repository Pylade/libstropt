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

#ifndef H_ATROPT
#define H_ATROPT
static int atrshortopt(char, const char**, int, char, struct option**, struct rtrn**);
static int atrlongopt(char**, int, struct option**, struct rtrn**);
static int short_activate(const char*, struct option*);
static int long_activate(const char*, int, struct option*);
static int str2cnt(const char*, const char*);
static int deleq(char*);
static struct rtrn* new_return(void);
static int new_return_arg(struct rtrn**, char*);
static int new_return_error(struct rtrn**, const char*, int);
#endif /* H_ATROPT */

