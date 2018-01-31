#ifndef _FILES_H
#define _FILES_H

#include "common.h"

#ifdef _LINUX
#include <dirent.h>
#endif

struct File {
	bool isdir;
	string file;
	string path;
	string base;
	string icon;
};

string dirGoUp(string pathName);
string dirname(string fileName);
string basename(string fileName);
string getExtension(string fileName);
string getWithoutExtension(string fileName);
File* createFile(string fileName, string basePath="");
void clearDirectory(vector<File *> *files);
void readDirectory(vector<File *> *files, string base, string path, vector<string> *extensions);
File* getNextFile(File *file, vector<string> *extensions=NULL,bool backward=false);
File* getPrevFile(File *file, vector<string> *extensions=NULL);
void fileclone(File *fdst, File *forg);

#endif
