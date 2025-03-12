#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

bool ListDirectoryContents(const char *sDir)
{
    WIN32_FIND_DATA fdFile;
    HANDLE hFind = NULL;

    char sPath[2048];

    //Specify a file mask. *.* = We want everything!
    sprintf(sPath, "%s\\*.*", sDir);

    if((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE)
    {
        printf("Path not found: [%s]\n", sDir);
        return false;
    }

    do
    {
        //Find first file will always return "."
        //    and ".." as the first two directories.
        if(strcmp(fdFile.cFileName, ".") != 0
                && strcmp(fdFile.cFileName, "..") != 0)
        {
            //Build up our file path using the passed in
            //  [sDir] and the file/foldername we just found:
            sprintf(sPath, "%s\\%s", sDir, fdFile.cFileName);

            //Is the entity a File or Folder?
            if(fdFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY)
            {
                printf("Directory: %s\n", sPath);
                ListDirectoryContents(sPath); //Recursion, I love it!
            }
            else{                
                char *suffix = strrchr(sPath, '.');
                if (strcmp(suffix + 1, "mp3") == 0)
                    printf("File: %s\n", sPath);
                    suffix = suffix + 1;

            }
        }
    }
    while(FindNextFile(hFind, &fdFile)); //Find the next file.

    FindClose(hFind); //Always, Always, clean things up!

    return true;
}

int main(){
    const char *url = "file:C:\\Stalker Dev\\Tools\\Sound Editing\\OGGCEditor\\Audi 5000 (feat. Medhane)_994028794 - ANKHLEJOHN.mp3";
    AVFormatContext *s = NULL;
    AVDictionary *options = NULL;

    av_dict_set(&options, "");

    if (avformat_open_input(&s, url, NULL, &options) < 0)
        abort();


    av_dict_free(&options);
    avformat_close_input();
}

