// filesys.cc
//	Routines to manage the overall operation of the file system.
//	Implements routines to map from textual file names to files.
//
//	Each file in the file system has:
//	   A file header, stored in a sector on disk
//		(the size of the file header data structure is arranged
//		to be precisely the size of 1 disk sector)
//	   A number of data blocks
//	   An entry in the file system directory
//
// 	The file system consists of several data structures:
//	   A bitmap of free disk sectors (cf. bitmap.h)
//	   A directory of file names and file headers
//
//      Both the bitmap and the directory are represented as normal
//	files.  Their file headers are located in specific sectors
//	(sector 0 and sector 1), so that the file system can find them
//	on bootup.
//
//	The file system assumes that the bitmap and directory files are
//	kept "open" continuously while Nachos is running.
//
//	For those operations (such as Create, Remove) that modify the
//	directory and/or bitmap, if the operation succeeds, the changes
//	are written immediately back to disk (the two files are kept
//	open during all this time).  If the operation fails, and we have
//	modified part of the directory and/or bitmap, we simply discard
//	the changed version, without writing it back to disk.
//
// 	Our implementation at this point has the following restrictions:
//
//	   there is no synchronization for concurrent accesses
//	   files have a fixed size, set when the file is created
//	   files cannot be bigger than about 3KB in size
//	   there is no hierarchical directory structure, and only a limited
//	     number of files can be added to the system
//	   there is no attempt to make the system robust to failures
//	    (if Nachos exits in the middle of an operation that modifies
//	    the file system, it may corrupt the disk)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.
#ifndef FILESYS_STUB

#include "copyright.h"
#include "debug.h"
#include "disk.h"
#include "pbitmap.h"
#include "directory.h"
#include "filehdr.h"
#include "filesys.h"

//----------------------------------------------------------------------
// FileSystem::FileSystem
// 	Initialize the file system.  If format = TRUE, the disk has
//	nothing on it, and we need to initialize the disk to contain
//	an empty directory, and a bitmap of free sectors (with almost but
//	not all of the sectors marked as free).
//
//	If format = FALSE, we just have to open the files
//	representing the bitmap and the directory.
//
//	"format" -- should we initialize the disk?
//----------------------------------------------------------------------

FileSystem::FileSystem(bool format)
{
    /* MP4 */
    for (int i = 0; i < MAXFILENUM; i++)
        fileDescriptorTable[i] = NULL;
    openedNum = 0;

    DEBUG(dbgFile, "Initializing the file system.");
    if (format)
    {
        PersistentBitmap *freeMap = new PersistentBitmap(NumSectors);

        Directory *directory = new Directory(NumDirEntries);
        FileHeader *mapHdr = new FileHeader;
        FileHeader *dirHdr = new FileHeader;

        DEBUG(dbgFile, "Formatting the file system.");

        // First, allocate space for FileHeaders for the directory and bitmap
        // (make sure no one else grabs these!)
        freeMap->Mark(FreeMapSector);
        //cout<<"m1 "<<freeMap->NumClear()<<'\n';
        freeMap->Mark(DirectorySector);
        //cout<<"m2 "<<freeMap->NumClear()<<'\n';
        // Second, allocate space for the data blocks containing the contents
        // of the directory and bitmap files.  There better be enough space!

        ASSERT(mapHdr->Allocate(freeMap, FreeMapFileSize));
        //cout<<"map"<<freeMap->NumClear()<<'\n';
        ASSERT(dirHdr->Allocate(freeMap, DirectoryFileSize));

        // Flush the bitmap and directory FileHeaders back to disk
        // We need to do this before we can "Open" the file, since open
        // reads the file header off of disk (and currently the disk has garbage
        // on it!).

        DEBUG(dbgFile, "Writing headers back to disk.");
        mapHdr->WriteBack(FreeMapSector);
        dirHdr->WriteBack(DirectorySector);

        // OK to open the bitmap and directory files now
        // The file system operations assume these two files are left open
        // while Nachos is running.

        freeMapFile = new OpenFile(FreeMapSector);
        directoryFile = new OpenFile(DirectorySector);

        // Once we have the files "open", we can write the initial version
        // of each file back to disk.  The directory at this point is completely
        // empty; but the bitmap has been changed to reflect the fact that
        // sectors on the disk have been allocated for the file headers and
        // to hold the file data for the directory and bitmap.

        DEBUG(dbgFile, "Writing bitmap and directory back to disk.");
        freeMap->WriteBack(freeMapFile); // flush changes to disk
        directory->WriteBack(directoryFile);

        if (debug->IsEnabled('f'))
        {
            freeMap->Print();
            directory->Print();
        }
        delete freeMap;
        delete directory;
        delete mapHdr;
        delete dirHdr;
    }
    else
    {
        // if we are not formatting the disk, just open the files representing
        // the bitmap and directory; these are left open while Nachos is running
        freeMapFile = new OpenFile(FreeMapSector);
        directoryFile = new OpenFile(DirectorySector);
    }
}

//----------------------------------------------------------------------
// MP4 mod tag
// FileSystem::~FileSystem
//----------------------------------------------------------------------
FileSystem::~FileSystem()
{
    delete freeMapFile;
    delete directoryFile;
}

//----------------------------------------------------------------------
// FileSystem::Create
// 	Create a file in the Nachos file system (similar to UNIX create).
//	Since we can't increase the size of files dynamically, we have
//	to give Create the initial size of the file.
//
//	The steps to create a file are:
//	  Make sure the file doesn't already exist
//        Allocate a sector for the file header
// 	  Allocate space on disk for the data blocks for the file
//	  Add the name to the directory
//	  Store the new file header on disk
//	  Flush the changes to the bitmap and the directory back to disk
//
//	Return TRUE if everything goes ok, otherwise, return FALSE.
//
// 	Create fails if:
//   		file is already in directory
//	 	no free space for file header
//	 	no free entry for file in directory
//	 	no free space for data blocks for the file
//
// 	Note that this implementation assumes there is no concurrent access
//	to the file system!
//
//	"name" -- name of file to be created
//	"initialSize" -- size of file to be created
//----------------------------------------------------------------------

bool FileSystem::Create(char *path, int initialSize, bool isDir)
{
    Directory *directory;
    PersistentBitmap *freeMap;
    FileHeader *hdr;
    int sector;
    bool success;

    DEBUG(dbgFile, "Creating file " << path << " size " << initialSize);

    if (isDir)
        initialSize = DirectoryFileSize;

    directory = new Directory(NumDirEntries);
    /* MP4 */
    char targetPath[500];
    strcpy(targetPath, path);
    cout << "targetPath: " << targetPath << endl;
    OpenFile *curDirFile = FindSubDir(targetPath);
    if (curDirFile == NULL)
    {
        delete directory;
        return FALSE;
    }
    directory->FetchFrom(curDirFile);

    cout << "Start creating file: " << targetPath << "\n";

    if (directory->Find(targetPath) != -1)
        success = FALSE; // file is already in directory
    else
    {
        freeMap = new PersistentBitmap(freeMapFile, NumSectors);
        sector = freeMap->FindAndSet(); // find a sector to hold the file header
        if (sector == -1)
        {
            success = FALSE; // no free block for file header
            cout << "   ---> Create fail\n";
        }
        else if (!directory->Add(targetPath, sector, isDir))
        {
            success = FALSE; // no space in directory
            cout << "   ---> Create fail\n";
        }
        else
        {
            hdr = new FileHeader;
            /* MP4 */
            int totalSize = hdr->Allocate(freeMap, initialSize);
            if (totalSize == 0)
            {
                success = FALSE; // no space on disk for data
                cout << "   ---> Create fail\n";
            }
            else
            {
                success = TRUE;
                // everthing worked, flush all changes back to disk
                hdr->WriteBack(sector);
                directory->WriteBack(curDirFile);
                freeMap->WriteBack(freeMapFile);
                cout << "   ---> Create success, total header's size:  " << totalSize << " B"
                     << "\n\n";
            }
            delete hdr;
        }
        delete freeMap;
    } /* MP4 */

    if (curDirFile != directoryFile)
        delete curDirFile;
    delete directory;

    return success;
}

//----------------------------------------------------------------------
// FileSystem::Open
// 	Open a file for reading and writing.
//	To open a file:
//	  Find the location of the file's header, using the directory
//	  Bring the header into memory
//
//	"name" -- the text name of the file to be opened
//----------------------------------------------------------------------

std::pair<OpenFile *, OpenFileId> FileSystem::Open(char *path)
{
    Directory *directory = new Directory(NumDirEntries);
    OpenFile *openFile = NULL;
    int sector;

    /* MP4 */
    char targetPath[500];
    strcpy(targetPath, path);
    OpenFile *curDirFile = FindSubDir(targetPath);
    if (curDirFile == NULL)
    {
        delete directory;
        return make_pair((OpenFile *)NULL, -1);
    }
    cout << "Start opening file: " << targetPath << "\n";
    DEBUG(dbgFile, "Opening file" << targetPath);
    directory->FetchFrom(curDirFile);
    sector = directory->Find(targetPath);
    /* MP4 */
    if (openedNum == MAXFILENUM) // fileDescriptorTable has no space
    {
        cout << "   ---> Open fail\n\n";
        if (curDirFile != directoryFile)
            delete curDirFile; /* MP4 */
        delete directory;
        return make_pair((OpenFile *)NULL, -1);
    }

    if (sector >= 0)
        openFile = new OpenFile(sector); // name was found in directory

    if (openFile == NULL)
    {
        cout << "   ---> Open fail\n\n";
        if (curDirFile != directoryFile)
            delete curDirFile; /* MP4 */
        delete directory;
        return make_pair((OpenFile *)NULL, -1);
    }
    cout << "   ---> Open success\n\n";

    for (int ID = 1; ID <= MAXFILENUM; ID++)
    {
        if (fileDescriptorTable[ID] == NULL)
        {
            fileDescriptorTable[ID] = openFile;
            openedNum++;
            cout << "openedNum = " << openedNum << " fileDescriptorTable[ID] = " << ID << '\n';
            if (curDirFile != directoryFile)
                delete curDirFile; /* MP4 */
            delete directory;
            return make_pair((OpenFile *)openFile, ID);
        }
    }

    if (curDirFile != directoryFile)
        delete curDirFile; /* MP4 */
    delete directory;
    return make_pair((OpenFile *)NULL, -1); // return NULL if not found
}

//----------------------------------------------------------------------
// FileSystem::Remove
// 	Delete a file from the file system.  This requires:
//	    Remove it from the directory
//	    Delete the space for its header
//	    Delete the space for its data blocks
//	    Write changes to directory, bitmap back to disk
//
//	Return TRUE if the file was deleted, FALSE if the file wasn't
//	in the file system.
//
//	"name" -- the text name of the file to be removed
//----------------------------------------------------------------------

bool FileSystem::Remove(bool recursion, char *path)
{
    Directory *directory;
    PersistentBitmap *freeMap;
    FileHeader *fileHdr;
    int sector;

    directory = new Directory(NumDirEntries);

    /* MP4 */
    char targetPath[500];
    strcpy(targetPath, path);
    OpenFile *curDirFile = FindSubDir(targetPath);
    if (curDirFile == NULL)
    {
        delete directory;
        return FALSE;
    }

    cout << "Start removing file: " << targetPath << "\n";

    directory->FetchFrom(curDirFile);
    sector = directory->Find(targetPath);
    if (sector == -1)
    {
        cout << "   ---> Remove fail\n\n";
        if (curDirFile != directoryFile)
            delete curDirFile; /* MP4 */
        delete directory;
        return FALSE; // file not found
    }

    if (directory->IsDir(targetPath) && recursion)
    {
        // fetch subdirectory from disk
        Directory *subDir = new Directory(NumDirEntries);
        OpenFile *subDirFile = new OpenFile(sector);
        subDir->FetchFrom(subDirFile);

        char targetPath[500];
        strcpy(targetPath, path);
        int offset = strlen(targetPath);
        targetPath[offset] = '/';

        // Remove all things in the target directory
        for (int i = 0; i < subDir->tableSize; i++)
        {
            if (subDir->table[i].inUse)
            {
                //update tragetPath
                strcpy(targetPath + offset + 1, subDir->table[i].name);
                Remove(recursion, targetPath);
            }
        }
        delete subDir;
        delete subDirFile;
    }

    fileHdr = new FileHeader;
    fileHdr->FetchFrom(sector);

    freeMap = new PersistentBitmap(freeMapFile, NumSectors);

    fileHdr->Deallocate(freeMap); // remove data blocks
    freeMap->Clear(sector);       // remove header block
    directory->Remove(targetPath);

    freeMap->WriteBack(freeMapFile);  // flush to disk
    directory->WriteBack(curDirFile); // flush to disk

    delete fileHdr;
    if (curDirFile != directoryFile)
        delete curDirFile; /* MP4 */
    delete directory;
    delete freeMap;
    cout << "   ---> Remove success\n\n";
    return FALSE;
}

//----------------------------------------------------------------------
// FileSystem::List
// 	List all the files in the file system directory.
//----------------------------------------------------------------------

void FileSystem::List(bool recursion, char *dirPath)
{
    if (strcmp(dirPath, "/") == 0)
    { // root directory
        Directory *directory = new Directory(NumDirEntries);
        directory->FetchFrom(directoryFile);
        cout << "List  \"/\"" << endl;
        directory->List(recursion, 0);
        delete directory;
        return;
    }
    else
    {
        char targetPath[500]; // FindSubDir will modified path into filename
        strcpy(targetPath, dirPath);

        // subDirFile is the directory containing the target directory file
        OpenFile *conDirFile = FindSubDir(targetPath);
        if (conDirFile == NULL) // no such dir
            return;
        Directory *conDir = new Directory(NumDirEntries);
        conDir->FetchFrom(conDirFile);

        int targetSector = conDir->Find(targetPath);
        ASSERT(targetSector >= 0);
        OpenFile *targetDirFile = new OpenFile(targetSector);
        Directory *targetDir = new Directory(NumDirEntries);
        targetDir->FetchFrom(targetDirFile);

        cout << "List \"" << targetPath << "\"" << endl;
        targetDir->List(recursion, 0);

        delete targetDirFile;
        delete targetDir;
        if (conDirFile != directoryFile)
            delete conDirFile; // not delete root directory file
        delete conDir;
    }
}

//----------------------------------------------------------------------
// FileSystem::Print
// 	Print everything about the file system:
//	  the contents of the bitmap
//	  the contents of the directory
//	  for each file in the directory,
//	      the contents of the file header
//	      the data in the file
//----------------------------------------------------------------------

void FileSystem::Print()
{
    FileHeader *bitHdr = new FileHeader;
    FileHeader *dirHdr = new FileHeader;
    PersistentBitmap *freeMap = new PersistentBitmap(freeMapFile, NumSectors);
    Directory *directory = new Directory(NumDirEntries);

    printf("Bit map file header:\n");
    bitHdr->FetchFrom(FreeMapSector);
    bitHdr->Print();

    printf("Directory file header:\n");
    dirHdr->FetchFrom(DirectorySector);
    dirHdr->Print();

    freeMap->Print();

    directory->FetchFrom(directoryFile);
    directory->Print();

    delete bitHdr;
    delete dirHdr;
    delete freeMap;
    delete directory;
}

OpenFile *FileSystem::FindSubDir(char *subDirPath)
{
    char *delim = "/";
    char *token = strtok(subDirPath, delim);
    char *nextToken = "";

    OpenFile *curDirFile = directoryFile;
    Directory *curDir = new Directory(NumDirEntries);
    curDir->FetchFrom(directoryFile);
    if (token == NULL)
    {
        delete curDir;
        return NULL;
    }
    else
    {
        nextToken = strtok(NULL, delim);

        while (nextToken != NULL && curDir->IsDir(token))
        {
            int sector = curDir->Find(token);
            if (sector == -1)
            {
                delete curDir;
                if (curDirFile != directoryFile)
                    delete curDirFile;
                return NULL;
            }
            else
            {
                if (curDirFile != directoryFile)
                    delete curDirFile;

                curDirFile = new OpenFile(sector);
                curDir->FetchFrom(curDirFile);
            }
            token = nextToken;
            nextToken = strtok(NULL, delim);
        }
        // end file
        strcpy(subDirPath, token);
        delete curDir;
        return curDirFile;
    }
}
#endif // FILESYS_STUB
