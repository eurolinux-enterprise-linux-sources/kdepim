// format.h --
// $Id: format.h 789251 2008-03-23 19:01:07Z osterfeld $
// This is part of Metakit, see http://www.equi4.com/metakit.html

/** @file
 * Encapsulation of all format handlers
 */

#ifndef __FORMAT_H__
#define __FORMAT_H__

/////////////////////////////////////////////////////////////////////////////
// Declarations in this file

class c4_Handler; // not defined here

extern c4_Handler *f4_CreateFormat(const c4_Property &, c4_HandlerSeq &);
extern int f4_ClearFormat(char);
extern int f4_CompareFormat(char, const c4_Bytes &, const c4_Bytes &);

/////////////////////////////////////////////////////////////////////////////

#endif
