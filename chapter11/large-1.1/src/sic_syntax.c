/* sic_syntax.c -- syntax callbacks for sic shell
   Copyright (C) 2000 Gary V. Vaughan
  
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.
  
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
  
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include "sic.h"
#include "sic_syntax.h"
#include "sic_repl.h"

#define CHUNK_SIZE	256

/* List of builtin syntax. */
#define syntax_functions		\
	SYNTAX(escape,	"\\")		\
	SYNTAX(space,	" \f\n\r\t\v")	\
	SYNTAX(comment,	"#")		\
	SYNTAX(string,	"\"")		\
	SYNTAX(endcmd,	";")		\
	SYNTAX(endstr,	"")

/* Prototype Generator. */
#define SIC_SYNTAX(name)						\
	int name PARAMS((Sic *sic, BufferIn *in, BufferOut *out))
			
#define SYNTAX(name, string)		\
	extern SIC_SYNTAX(CONC(syntax_, name));
syntax_functions
#undef SYNTAX

/* Syntax handler mappings. */
Syntax syntax_table[] = {

#define SYNTAX(name, string)		\
	{ CONC(syntax_, name), string },
  syntax_functions
#undef SYNTAX
  
  { NULL, NULL }
};

int
syntax_init (sic)
     Sic *sic;
{
  int *pinstring = XMALLOC (int, 1);
  *pinstring = 0;
  
  sicstate_set (sic, "::syntax::instring", pinstring, free);
  
  return SIC_OKAY;
}

int
syntax_finish (sic, in, out)
     Sic *sic;
     BufferIn *in;
     BufferOut *out;
{
  if (in->start && &in->buf.start[in->buf.i] != in->start)
    {
      /* Record the pending entry from the calling loop. */
      out->offv = XREALLOC (int, out->offv, 1 + out->offc);
      out->offv[out->offc] = out->argind;
      out->buf.start[out->buf.i] = 0;
      ++out->offc;
    }

  return SIC_OKAY;
}    

SYNTAX_DECLARATION (escape)
{
  if (in->buf.start[1 + in->buf.i] == '\n')
    {
      /* escaped newlines are treated as if they weren't there */
      if (in->start == in->buf.start + in->buf.i)
	in->start += 2;
      in->buf.i += 2;
    }
  else
    {
      /* skip the escape character, and copy the escaped character */
      ++in->buf.i;
      out->buf.start[out->buf.i++] = in->buf.start[in->buf.i++];
    }

  return SIC_CONTINUE;
}

SYNTAX_DECLARATION (space)
{
  int *pinstring = sicstate_get (sic, "::syntax::instring");

  if (*pinstring)
    /* just copy spaces inside strings */
    out->buf.start[out->buf.i++] = in->buf.start[in->buf.i++];
  else if (in->start == in->buf.start + in->buf.i)
    {
      /* ignore leading whitespace at the start of an arg */
      ++in->start;
      ++in->buf.i;
    }
  else
    {
      /* otherwise save the previous argument offset */
      out->offv = XREALLOC (int, out->offv, 1 + out->offc);
      out->offv[out->offc] = out->argind;
      out->buf.start[out->buf.i] = 0;
      out->argind = ++out->buf.i;
      ++out->offc;
      in->start = &in->buf.start[++in->buf.i];
    }

  return SIC_CONTINUE;
}

SYNTAX_DECLARATION (comment)
{
  /* advance to the end of the line. */
  while (in->buf.start[++in->buf.i] && in->buf.start[in->buf.i] != '\n')
    /*NOWORK*/;

  return SIC_CONTINUE;
}

SYNTAX_DECLARATION (string)
{
  int *pinstring = sicstate_get (sic, "::syntax::instring");

  /* Toggle the INSTRING flag, and advance past the quote mark. */
  *pinstring = !*pinstring;
  ++in->buf.i;

  return SIC_CONTINUE;
}

SYNTAX_DECLARATION (endcmd)
{
  int *pinstring = sicstate_get (sic, "::syntax::instring");

  if (!*pinstring)
    {
      ++in->buf.i;
      return SIC_OKAY;
    }

  out->buf.start[out->buf.i++] = in->buf.start[in->buf.i++];
  return SIC_CONTINUE;
}

SYNTAX_DECLARATION (endstr)
{
  int *pinstring = sicstate_get (sic, "::syntax::instring");

  if (*pinstring)
    return SIC_INCOMPLETE;

  return SIC_OKAY;
}
