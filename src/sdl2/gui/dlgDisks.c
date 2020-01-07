/*
  Original code from Hatari, adapted for Teo

  This file is distributed under the GNU General Public License, version 2
  or at your option any later version. Read the file gpl.txt for details.

  Dialog to load/eject flopyy disks 
*/
#include <stdio.h>
#include <assert.h>

#include "dialog.h"
#include "file.h"
#include "sdlgui.h"

#include "errors.h"
#include "std.h"
#include "defs.h"
#include "teo.h"
#include "media/disk.h"
#include "media/disk/daccess.h"


#define DLGDSK_0_EJECT 5
#define DLGDSK_0_NAME 6
#define DLGDSK_0_SIDE 7
#define DLGDSK_0_BROWSE 8
#define DLGDSK_0_WPROT 9

#define DLGDSK_1_EJECT 11
#define DLGDSK_1_NAME 12
#define DLGDSK_1_SIDE 13
#define DLGDSK_1_BROWSE 14
#define DLGDSK_1_WPROT 15

#define DLGDSK_2_EJECT 17
#define DLGDSK_2_NAME 18
#define DLGDSK_2_SIDE 19
#define DLGDSK_2_BROWSE 20
#define DLGDSK_2_WPROT 21

#define DLGDSK_3_EJECT 23
#define DLGDSK_3_NAME 24
#define DLGDSK_3_SIDE 25
#define DLGDSK_3_BROWSE 26
#define DLGDSK_3_WPROT 27

#define DLGDSK_DIRECT_ACCESS 28
#define DLGDSK_OK 29

#define MAX_FLOPPYDRIVES 4

/*TODO: Don't waste memory and do better than that*/
static char sFiles[4][FILENAME_MAX]; /*Better use dynamic allocation*/
/* As floppy disks have no more than 2 sides,
 * we only need 2 pointers
 * */
static char snSides[4][2]; 

static SGOBJ diskdlg[]={
/*type, f,s        x    y    w    h  text */
{ SGBOX,0,0,       0,  0,   50, 16, NULL },
{ SGTEXT,0,0,      18,  1,   11,   1, "Disk drives" },

/*Header*/
{ SGTEXT,0,0,     30,  2,   4, 1,  "side" },
{ SGTEXT,0,0,     40,  2,   5, 1,  "prot." },

  /* disk 0 */
{ SGTEXT,0,0,      1,  4,   2,   1, "0:" },
{ SGBUTTON,0,0,    4,  4,   1,   1, "x" },
{ SGTEXT,0,0,      6,  4,  16,   1, sFiles[0] },
{ SGBUTTON,0,0,    32, 4,   1,   1, snSides[0] },
{ SGBUTTON,0,0,    35, 4,   3,   1, "..." },
{ SGCHECKBOX,0,0,  41, 4,   1,   1, "" },
  /* disk 1 */
{ SGTEXT,0,0,      1,  6,   2,   1, "1:" },
{ SGBUTTON,0,0,    4,  6,   1,   1, "x" },
{ SGTEXT,0,0,      6,  6,  16,   1, sFiles[1] },
{ SGBUTTON,0,0,    32, 6,   1,   1, snSides[1] },
{ SGBUTTON,0,0,    35, 6,   3,   1, "..." },
{ SGCHECKBOX,0,0,  41, 6,   1,   1, "" },
  /* disk 2 */
{ SGTEXT,0,0,      1,  8,   2,   1, "2:" },
{ SGBUTTON,0,0,    4,  8,   1,   1, "x" },
{ SGTEXT,0,0,      6,  8,  16,   1, sFiles[2] },
{ SGBUTTON,0,0,    32, 8,   1,   1, snSides[2] },
{ SGBUTTON,0,0,    35, 8,   3,   1, "..." },
{ SGCHECKBOX,0,0,  41, 8,   1,   1, "" },

  /* disk 3 */
{ SGTEXT,0,0,      1,  10,   2,   1, "3:" },
{ SGBUTTON,0,0,    4,  10,   1,   1, "x" },
{ SGTEXT,0,0,      6,  10,  16,   1, sFiles[3] },
{ SGBUTTON,0,0,    32, 10,   1,   1, snSides[3] },
{ SGBUTTON,0,0,    35, 10,   3,   1, "..." },
{ SGCHECKBOX,0,0,  41, 10,   1,   1, "" },

  /* direct disk */
{ SGBUTTON,0,0,     1, 12,  46,  1, "_Direct access" },

{ SGBUTTON,SG_DEFAULT,0,   37, 14, 10,  1,  "_OK" },
{ SGSTOP, 0, 0, 0,0, 0,0, NULL }
};



static void DlgDisks_EjectDisk(char *dlgname, int drive)
{
	assert(drive >= 0 && drive < MAX_FLOPPYDRIVES);
    int pIdx;

    pIdx = DLGDSK_0_WPROT + drive * (DLGDSK_1_WPROT-DLGDSK_0_WPROT);
    diskdlg[pIdx].state &= ~SG_SELECTED;
    snprintf(dlgname, FILENAME_MAX-1, "%s", "(None)");

    disk_Eject(drive);
}

/**
 * Let user browse given disk, insert disk if one selected.
 * @dlgname: pointer to where the text displayed on screen is stored
 * @drive: drive index
 * @diskid: id of the dialog object displaying the text (index the window
 * array)
 */
static void DlgDisks_BrowseDisk(char *dlgname, int drive)
{
	assert(drive >= 0 && drive < MAX_FLOPPYDRIVES);

	char *selname, *zip_path;
	const char *tmpname;
    int objIdx;
    int diskid;

	if (teo.disk[drive].file)
	    tmpname = teo.disk[drive].file;
	else
        tmpname = teo.default_folder ? teo.default_folder : "/";

	selname = SDLGui_FileSelect("Floppy image:", tmpname, &zip_path, false);
	if (!selname)
		return;

	if (File_Exists(selname))
	{
        int rv;
        /* This will copy filename over
         * and set teo.disk[drive].write_protect
         * accordingly
         * */
        rv = disk_Load(drive, selname); 
        if(rv < 0){
            DlgAlert_Notice(teo_error_msg);
        }else{
            diskid = DLGDSK_0_NAME + drive * (DLGDSK_1_NAME-DLGDSK_0_NAME);
            snprintf(dlgname, FILENAME_MAX-1, "%s", std_BaseName(selname));
            if(!teo.default_folder){
                teo.default_folder = strdup(selname);
                std_CleanPath (teo.default_folder); /*CleanPath works like dirname(3)*/
            }

            objIdx = DLGDSK_0_WPROT + drive * (DLGDSK_1_WPROT-DLGDSK_0_WPROT);
            diskdlg[objIdx].state &= ~SG_SELECTED;
            if(rv == true){
                DlgAlert_Notice("Warning: Writing unavailable.");
                diskdlg[objIdx].state |= SG_SELECTED;
            }
            if (teo.disk[drive].side >= disk[drive].side_count)
                teo.disk[drive].side = disk[drive].side_count - 1;
            snprintf(snSides[drive], 2, "%d", teo.disk[drive].side);
        }
	}
	else
	{
		dlgname[0] = '\0';
	}
	free(zip_path);
	free(selname);
}

static void DlgDisks_SideNext(int drive)
{
    teo.disk[drive].side++;
    if (teo.disk[drive].side >= disk[drive].side_count)
        teo.disk[drive].side = 0;
    snprintf(snSides[drive], 2, "%d", teo.disk[drive].side);
}


static void DlgDisks_ToggleWriteProtection(int drive)
{
    int pIdx;

    pIdx = DLGDSK_0_WPROT + drive * (DLGDSK_1_WPROT-DLGDSK_0_WPROT);
    if(teo.disk[drive].write_protect){ //Disk is currently protected
        if(disk_Protection(drive, false) == true){
            DlgAlert_Notice(is_fr? "Ecriture impossible sur ce support."
                                   :"Writing unavailable on this device.");
            diskdlg[pIdx].state |= SG_SELECTED;
        }else{
            diskdlg[pIdx].state &= ~SG_SELECTED;
        }
    }else{
        teo.disk[drive].write_protect = true;
    }
}



static void DlgDisks_EnableDirectAccess(int direct_disk)
{
    int drive;
    int objIdx;

    for (drive=0; drive<4; drive++)
    {
        if (direct_disk & (1<<drive))
        {
            objIdx = DLGDSK_0_NAME + (DLGDSK_1_NAME - DLGDSK_0_NAME) * drive;
            snprintf(sFiles[drive], FILENAME_MAX-1, "Direct Access");
            teo.disk[drive].file = std_free (teo.disk[drive].file);

            objIdx = DLGDSK_0_WPROT + (DLGDSK_1_WPROT - DLGDSK_0_WPROT) * drive;
            diskdlg[objIdx].state &= ~SG_SELECTED;
            if(daccess_LoadDisk(drive, "") == true){
                diskdlg[objIdx].state |= SG_SELECTED;
                teo.disk[drive].write_protect = true;
            }
        }
    }
}

/*-----------------------------------------------------------------------*/
/**
 * Show and process the "Disks" dialog
 * @da_mask: bitmask that shows which host drives support direct access
 */
void DlgDisks_Main(int da_mask)
{
	int i;
	int but;
    int objIdx;

    
	SDLGui_CenterDlg(diskdlg);

    /*Here get disks files from TEO config and put them in
     * also check/uncheck write protection
     *
     * */

    for (i = 0; i < MAX_FLOPPYDRIVES; i++){

        /*Init disk name*/
        objIdx = DLGDSK_0_NAME + i * (DLGDSK_1_NAME-DLGDSK_0_NAME);
        if(teo.disk[i].file){
            printf("drive %d: %s\n",i, teo.disk[i].file);
            snprintf(sFiles[i], FILENAME_MAX-1, "%s", std_BaseName(teo.disk[i].file));
        }else{
            snprintf(sFiles[i], FILENAME_MAX-1, "%s", "(None)");
            teo.disk[i].side = 0;
            disk[i].side_count = 1; /*Exported by disk.h*/
        }

        /* init disk protection */
        objIdx = DLGDSK_0_WPROT + i * (DLGDSK_1_WPROT-DLGDSK_0_WPROT);
        if (teo.disk[i].write_protect)
            diskdlg[objIdx].state |= SG_SELECTED;
        else
            diskdlg[objIdx].state &= ~SG_SELECTED;

        /* init disk side */
        objIdx = DLGDSK_0_SIDE + i * (DLGDSK_1_SIDE-DLGDSK_0_SIDE);
        if (teo.disk[i].side >= disk[i].side_count)
            teo.disk[i].side = disk[i].side_count - 1;
        snprintf(snSides[i], 2, "%d", teo.disk[i].side);

    }

	/* Show the dialog: */
	do{
        but = SDLGui_DoDialog(diskdlg, NULL, false);
		switch(but){
         /* Choose a new disk*/
		 case DLGDSK_0_BROWSE:                        
		 case DLGDSK_1_BROWSE:                     
		 case DLGDSK_2_BROWSE:                    
		 case DLGDSK_3_BROWSE:                  
            objIdx = (but - DLGDSK_0_BROWSE)/(DLGDSK_1_BROWSE-DLGDSK_0_BROWSE);
			DlgDisks_BrowseDisk(sFiles[objIdx], objIdx);
			break;
		 case DLGDSK_0_EJECT:
		 case DLGDSK_1_EJECT:
   		 case DLGDSK_2_EJECT:
   		 case DLGDSK_3_EJECT:
            objIdx = (but - DLGDSK_0_EJECT)/(DLGDSK_1_EJECT-DLGDSK_0_EJECT);
            DlgDisks_EjectDisk(sFiles[objIdx], objIdx);
			break;
         
         case DLGDSK_0_SIDE:
         case DLGDSK_1_SIDE:
         case DLGDSK_2_SIDE:
         case DLGDSK_3_SIDE:
            objIdx=(but - DLGDSK_0_SIDE)/(DLGDSK_1_SIDE - DLGDSK_0_SIDE);
            DlgDisks_SideNext(objIdx);
            break;
         case DLGDSK_0_WPROT:
         case DLGDSK_1_WPROT:
         case DLGDSK_2_WPROT:
         case DLGDSK_3_WPROT:
            objIdx=(but - DLGDSK_0_WPROT)/(DLGDSK_1_WPROT - DLGDSK_0_WPROT);
            DlgDisks_ToggleWriteProtection(objIdx);
            break;
         case DLGDSK_DIRECT_ACCESS:
            if(da_mask)
                DlgDisks_EnableDirectAccess(da_mask);
            break;

        }
    }while (but != DLGDSK_OK && but != SDLGUI_QUIT
	        && but != SDLGUI_ERROR && !bQuitProgram );
}
