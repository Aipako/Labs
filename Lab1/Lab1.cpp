#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


const int BUFFER_SIZE = 1024;
const int CHAR_SIZE = 1; //in byte

int i = 1, j;
char* helpString = "Options:\n\t-h - show options menu.\n\t-l, --lines - print number of lines in file.\n\t-c, --bytes - print number of bytes in file.\n\t-w, --words - print number of words in file.\n\nExample of using arguments:   WordCount.exe [OPTIONS] filename\n";
char* filename;
char* filetext;
bool byteFlag, lineFlag, wordFlag;
bool isWord, isEndOfLine, isTextExist = true;
int countOfCharReaded;
unsigned long long wordCount, lineCount, byteCount;

FILE* mainFile;

int main(int argc, char* arg[])
{
    while (arg[i])
    {
        if (strcmp(arg[i], "-h") == 0)
        {
            printf("%s\n", helpString);
            return 0;
        }
        if ((strcmp(arg[i], "-l") == 0) || (strcmp(arg[i], "--lines") == 0))
        {
            lineFlag = true;
            i++;
            continue;
        }
        if ((strcmp(arg[i], "-c") == 0) || (strcmp(arg[i], "--bytes") == 0))
        {
            byteFlag = true;
            i++;
            continue;
        }
        if ((strcmp(arg[i], "-w") == 0) || (strcmp(arg[i], "--words") == 0))
        {
            wordFlag = true;
            i++;
            continue;
        }
        if (strstr(arg[i], "-"))
        {
            printf("%s%s", arg[i], " - isn't a programm option, please use '-h' for help.");
            return 0;
        }
        else
        {
            filename = arg[i];
            break;
        }
        i++;
    }
    if (!filename)
    {
        printf("%s", "You didn't choose file.");
        return 0;
    }
    mainFile = fopen(filename, "rb+");
    if (!mainFile)
    {
        printf("%s  %s", filename, "This file does not exist.");
        return -1;
    }
    if ((!lineFlag) && (!wordFlag) && (!byteFlag))
    {
        printf("%s", "None of options chose.\n");
        return -1;
    }
    filetext = malloc(1024);
    while (feof(mainFile) == 0)
    {
        countOfCharReaded = fread(filetext, CHAR_SIZE, BUFFER_SIZE, mainFile);
        if (countOfCharReaded == 0)
        {
            isTextExist = false;
            break;
        }
        for (i = 0; i < countOfCharReaded; i++)
        {
            if (((filetext[i] == ' ') || (filetext[i] == '\t') || (filetext[i] == '\n') || (filetext[i] == '\r')) && (isWord))
            {
                wordCount++;
                if (filetext[i] == '\n')
                {
                    lineCount++;
                    isEndOfLine = true;

                }
                else
                    isEndOfLine = false;
                isWord = false;

            }
            else
            {

                if (!((filetext[i] == ' ') || (filetext[i] == '\t') || (filetext[i] == '\n') || (filetext[i] == '\r')))
                {
                    isWord = true;
                    isEndOfLine = false;
                }
                else
                    if (filetext[i] == '\n')
                    {
                        lineCount++;
                        isEndOfLine = true;
                    }


            }
            byteCount += CHAR_SIZE;
        }
    }
    if (isTextExist)
    {
        if (!isEndOfLine)
            wordCount++;
        lineCount++;
    }
    printf("%s", "In this file:\n");
    if (lineFlag)
    {
        printf("%llu%s", lineCount, " lines.\n");
    }
    if (byteFlag)
    {
        printf("%llu%s", byteCount, " bytes.\n");
    }
    if (wordFlag)
    {
        printf("%llu%s", wordCount, " words.\n");
    }
    fclose(mainFile);
    return 0;
}