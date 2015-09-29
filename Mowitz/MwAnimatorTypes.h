/*
   Egon Animator
   Copyright (C) 1997-2002  Ulric Eriksson <ulric@siag.nu>

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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston,
   MA 02111-1307, USA.
 */

/*
 * MwAnimatorTypes.h
 */

#ifndef MW_ANIMATOR_TYPES
#define MW_ANIMATOR_TYPES

/* misc stuff originally in animator.h */

#define MW_ANI_OK 0
#define MW_ANI_ERR 1

#define MW_ANI_NONE 0

#define MW_ANI_STOP 1
#define MW_ANI_NEXT 2
#define MW_ANI_PREVIOUS 3
#define MW_ANI_CONTINUE 4
#define MW_ANI_QUIT 5

#define MW_ANI_LINE 1
#define MW_ANI_RECTANGLE 2
#define MW_ANI_ARC 3
#define MW_ANI_ELLIPSE 4
#define MW_ANI_PIXMAP 5
#define MW_ANI_STRING 6
#define MW_ANI_POINT 7
#define MW_ANI_FILLRECT 8
#define MW_ANI_FILLARC 9
#define MW_ANI_FILLELLIPSE 10
#define MW_ANI_PLUGIN 11

#define MW_ANI_X 1
#define MW_ANI_Y 2
#define MW_ANI_WIDTH 3
#define MW_ANI_HEIGHT 4
#define MW_ANI_VISIBLE 5
#define MW_ANI_FORMAT 7
#define MW_ANI_TEXT 8


/* linked list of positions, sizes et al of an object */
typedef struct MwAniScript {
        int time;
        int x, y;       /* position */
        int width, height;
        int visible;
        struct MwAniScript *next;
} MwAniScript;

/* linked list of all the objects on the stage */
typedef struct MwAniObject {
        int type;
        char *name;
        MwAniScript *script;
        int fmt;
        char *string;
        struct MwAniObject *next;
} MwAniObject;

#endif	/* MW_ANIMATOR_TYPES */
