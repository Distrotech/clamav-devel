/*
 *  Copyright (C) 2004 Nigel Horne <njh@bandsman.co.uk>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * $Log: line.c,v $
 * Revision 1.4  2004/09/21 14:55:26  nigelhorne
 * Handle blank lines in text/plain messages
 *
 * Revision 1.3  2004/08/25 12:30:36  nigelhorne
 * Use memcpy rather than strcpy
 *
 * Revision 1.2  2004/08/21 11:57:57  nigelhorne
 * Use line.[ch]
 *
 * Revision 1.1  2004/08/20 11:58:20  nigelhorne
 * First draft
 *
 */

static	char	const	rcsid[] = "$Id: line.c,v 1.4 2004/09/21 14:55:26 nigelhorne Exp $";

#if HAVE_CONFIG_H
#include "clamav-config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "line.h"
#include "others.h"

#ifndef	CL_DEBUG
#define	NDEBUG	/* map CLAMAV debug onto standard */
#endif

#ifdef	OLD
line_t *
lineCreate(const char *data)
{
	line_t *ret = (line_t *)li_malloc(sizeof(struct line));

	if(ret == NULL)
		return NULL;

	ret->l_str = strdup(data);
	if(ret->l_str == NULL) {
		free(ret);
		return NULL;
	}
	ret->l_refs = 1;

	return ret;
}

line_t *
lineLink(line_t *line)
{
	line->l_refs++;
	return line;
}

line_t *
lineUnlink(line_t *line)
{
	/*printf("%d:\n\t'%s'\n", line->l_refs, line->l_str);*/

	if(--line->l_refs == 0) {
		free(line->l_str);
		free(line);
		return NULL;
	}
	return line;
}

const char *
lineGetData(const line_t *line)
{
	return line ? line->l_str : NULL;
}
#else
line_t *
lineCreate(const char *data)
{
	const size_t size = strlen(data);
	line_t *ret = (line_t *)cli_malloc(size + 2);

	if(ret == NULL)
		return NULL;

	ret[0] = (char)1;
	/*strcpy(&ret[1], data);*/
	memcpy(&ret[1], data, size);
	ret[size + 1] = '\0';

	return ret;
}

line_t *
lineLink(line_t *line)
{
	assert(line != NULL);
	if(line[0] == 127) {
		cli_warnmsg("lineLink: linkcount too large\n");
		return NULL;
	}
	line[0]++;
	return line;
}

line_t *
lineUnlink(line_t *line)
{
	/*printf("%d:\n\t'%s'\n", (int)line[0], &line[1]);*/

	if(--line[0] == 0) {
		free(line);
		return NULL;
	}
	return line;
}

const char *
lineGetData(const line_t *line)
{
	return line ? &line[1] : NULL;
}
#endif
