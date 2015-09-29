/****************************************************************************

        Directory.c

	This file contains the C code that implements the directory
	iteration and file information subsystem.

	This code is intended to be used as a convenient, machine
	independent interface to iterate through the contents of a
	directory.

 ****************************************************************************/

/*
 * Author:
 * 	Brian Totty
 * 	Department of Computer Science
 * 	University Of Illinois at Urbana-Champaign
 *	1304 West Springfield Avenue
 * 	Urbana, IL 61801
 * 
 * 	totty@cs.uiuc.edu
 * 	
 */
 
/* Converted to POSIX by Rob McMullen */

#include "Directory.h"

/*--------------------------------------------------------------------------*

        L O W    L E V E L    D I R E C T O R Y    I N T E R F A C E

 *--------------------------------------------------------------------------*/

int DirectoryOpen(char *dir_name,Directory *dp)
{
	if (DirectoryPathExpand(dir_name,DirectoryPath(dp)) == NULL)
	{
		return(FALSE);
	}
	DirectoryDir(dp) = opendir(DirectoryPath(dp));
	if (DirectoryDir(dp) == NULL) return(FALSE);
	return(TRUE);
} /* End DirectoryOpen */


void DirectoryRestart(Directory *dp)
{
	rewinddir(DirectoryDir(dp));
} /* End DirectoryRestart */


void DirectoryClose(Directory *dp)
{
	closedir(DirectoryDir(dp));
} /* End DirectoryClose */


int DirectoryReadNextEntry(Directory *dp,DirEntry *de)
{
	static struct dirent *_ep;
	static struct stat _lstats,_stats;
	char full_path[MAXPATHLEN + 2];

	_ep = readdir(DirectoryDir(dp));
	if (_ep == NULL) return(FALSE);
	strcpy(DirEntryFileName(de),_ep->d_name);
	strcpy(full_path,DirectoryPath(dp));
	strcat(full_path,DirEntryFileName(de));

	if (lstat(full_path,&_lstats) != 0) return(FALSE);

	if (S_ISDIR(_lstats.st_mode)) DirEntryType(de) = F_TYPE_DIR;
	else if (S_ISREG(_lstats.st_mode)) DirEntryType(de) = F_TYPE_FILE;
	else if (S_ISCHR(_lstats.st_mode)) DirEntryType(de) = F_TYPE_CHAR_SPECIAL;
	else if (S_ISBLK(_lstats.st_mode)) DirEntryType(de) = F_TYPE_BLOCK_SPECIAL;
	else if (S_ISFIFO(_lstats.st_mode)) DirEntryType(de) = F_TYPE_FIFO;
	else DirEntryType(de) = F_TYPE_UNKNOWN;
	
/* Warning: POSIX can't test for symbolic links */	
/* Can't POSIXly test for these cases:
	    case S_IFLNK:
		DirEntryType(de) = F_TYPE_SYM_LINK;
		break;
	    case S_IFSOCK:
		DirEntryType(de) = F_TYPE_SOCKET;
		break;
*/

	DirEntryIsBrokenLink(de) = FALSE;
	DirEntryIsDirectoryLink(de) = FALSE;
	if (DirEntryIsSymLink(de)) {			/* Symbolic Link */
		if (stat(full_path,&_stats) != 0) {	/* Can't Stat File */
			DirEntryIsBrokenLink(de) = TRUE;
			_stats = _lstats;
		}
		else {					/* Link Not Broken */
			if (S_ISDIR(_stats.st_mode)) {
				DirEntryIsDirectoryLink(de) = TRUE;
			}

		}
	}
	else {						/* Not Symbolic Link */
		_stats = _lstats;
	}

	FileInfoOrigMode(DirEntrySelfInfo(de)) = _lstats.st_mode;
	FileInfoProt(DirEntrySelfInfo(de)) = _lstats.st_mode & 0777;
	FileInfoUserID(DirEntrySelfInfo(de)) = _lstats.st_uid;
	FileInfoGroupID(DirEntrySelfInfo(de)) = _lstats.st_gid;
	FileInfoFileSize(DirEntrySelfInfo(de)) = _lstats.st_size;
	FileInfoLastAccess(DirEntrySelfInfo(de)) = _lstats.st_atime;
	FileInfoLastModify(DirEntrySelfInfo(de)) = _lstats.st_mtime;
	FileInfoLastStatusChange(DirEntrySelfInfo(de)) = _lstats.st_ctime;

	FileInfoOrigMode(DirEntryActualInfo(de)) = _stats.st_mode;
	FileInfoProt(DirEntryActualInfo(de)) = _stats.st_mode & 0777;
	FileInfoUserID(DirEntryActualInfo(de)) = _stats.st_uid;
	FileInfoGroupID(DirEntryActualInfo(de)) = _stats.st_gid;
	FileInfoFileSize(DirEntryActualInfo(de)) = _stats.st_size;
	FileInfoLastAccess(DirEntryActualInfo(de)) = _stats.st_atime;
	FileInfoLastModify(DirEntryActualInfo(de)) = _stats.st_mtime;
	FileInfoLastStatusChange(DirEntryActualInfo(de)) = _stats.st_ctime;

	return(TRUE);
} /* End DirectoryReadNextEntry */


char *DirectoryPathExpand(char *old_path,char *new_path)
{
	register char *p;
	char path[MAXPATHLEN + 2];

	if (getcwd(path, MAXPATHLEN) == NULL) return(NULL);
	if (chdir(old_path) != 0) return(NULL);
	if (getcwd(new_path, MAXPATHLEN) == NULL) strcpy(new_path,old_path);
	if (chdir(path) != 0) return(NULL);
	for (p = new_path; *p != '\0'; p++);
	if ((p != new_path) && *(p - 1) != '/')
	{
		*p++ = '/';
		*p = '\0';
	}
	return(new_path);
} /* End DirectoryPathExpand */
