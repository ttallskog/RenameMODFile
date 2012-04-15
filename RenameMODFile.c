#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#define pathlen 256
#define buffersize 10
#define intsize 3

#define fileReadOffset 6
#define filenameLengthNoSuffix 6

#define pathSeparator "/"

#define moiFileSuffix ".MOI"
#define modFileSuffix ".MOD"

int MoveFile(const char *source, const char *destination)
{
   int result = 0;

	result = rename(source, destination);
	if(result == 0)
		printf("Moved and renamed: %s to %s\n",oldFileName, newFileName);
	else
	{
		perror("Could not move the file");
		result = -1;
	}
   return result;
}

int  FileRead(const char *filePath, const char *sourcePath, const char *destinationPath,  const char *fileName)
{
   int result = 1;
   size_t fileReadResult;
   FILE *filePtr;
   unsigned char buffer[buffersize];
   char newFileName[pathlen], oldFileName[pathlen], fileNameNoSuffix[pathlen];
	char monthStr[intsize], dayStr[intsize], hourStr[intsize], minuteStr[intsize];

   filePtr = fopen(filePath, "rb");
   if(filePtr == NULL)
   {
      perror("Cann´t open file.");
      result = -1;
   }
   else
   {
      memset(buffer, '\0', buffersize);
      fseek(filePtr, fileReadOffset, SEEK_SET);
      fileReadResult = fread(buffer, sizeof(unsigned char), buffersize, filePtr);
      fclose(filePtr);
      
      if(fileReadResult == buffersize)
      {
			((int)buffer[2] < 10) ? sprintf(monthStr, "0%d", buffer[2]) : sprintf(monthStr, "%d", buffer[2]);
			((int)buffer[3] < 10) ? sprintf(dayStr, "0%d", buffer[3]) : sprintf(dayStr, "%d", buffer[3]);
			((int)buffer[4] < 10) ? sprintf(hourStr, "0%d", buffer[4]) : sprintf(hourStr, "%d", buffer[4]);
			((int)buffer[5] < 10) ? sprintf(minuteStr, "0%d", buffer[5]) : sprintf(minuteStr, "%d", buffer[5]);
			memset(oldFileName, '\0', pathlen);
			memset(fileNameNoSuffix, '\0', pathlen);
			strncat(fileNameNoSuffix, fileName, filenameLengthNoSuffix);
	 		sprintf(oldFileName, "%s%s%s%s", sourcePath, pathSeparator, fileNameNoSuffix, modFileSuffix);
	 		sprintf(newFileName, "%s%s%d%s%sT%s%s%s", destinationPath, pathSeparator, buffer[0] << 8 | buffer[1], monthStr, dayStr, hourStr, minuteStr, modFileSuffix); // position 6 and 7 is seconds.

			result = MoveFile(oldFileName, newFileName);
      }
   }
   return result;
}

int ListDir(const char *sourcePath, const char *destinationPath)
{
   int result = 1, resultOk = 0, resultFailed = 0,  fileReadResult = 0;
   DIR * dir;
   struct dirent *entry;
   char filePath[pathlen];

   dir = opendir(sourcePath);
   if(dir == NULL) 
   {
      perror("Cann´t open directory.");
      result = -1;
   }
   else
   {
      while((entry = readdir(dir)))	
      {
			if(strstr(entry -> d_name, moiFileSuffix) != NULL)
			{
				memset(filePath, '\0', pathlen);
				sprintf(filePath, "%s%s%s", sourcePath, pathSeparator, entry -> d_name);
				fileReadResult = FileRead(filePath, sourcePath, destinationPath, entry -> d_name);
				if(fileReadResult > 0)
					resultOk++;
				else
					resultFailed++;
			}
      }
      printf("Files handled: %d, Ok: %d, Failed: %d\n\n", resultOk + resultFailed, resultOk, resultFailed);
   }
   closedir(dir);
   return result;
}

int main(int argc, char **argv)
{
   int counter = 1;

   if(argc <= 1 && argc >= 2)
   {
      perror("You need to provide one or more source paths and one destination.\n Format: source [...] destination");
      exit(1);
   }

   while(++counter < argc) 
   {
      printf("\nListing %s...\n", argv[counter - 1]);
      ListDir(argv[counter - 1], argv[argc - 1]);
   }
   return counter;
}
