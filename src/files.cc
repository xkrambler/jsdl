#include "files.h"

// interna: cadena a minúsculas
string _strtolower(string s) {
	string l=s;
	for (unsigned int i=0; i<l.length(); i++)
		l[i]=tolower(l[i]);
	return l;
}

// devuelve el padre de una cadena de ruta especificada
string dirGoUp(string pathName) {
  int i;
  for (i=pathName.size()-2;i>=0;i--)
	  if ( pathName[i]=='/')
		  return pathName.substr(0,i+1);
  return "";
}

// devuelve la carpeta de una ruta y fichero
string dirname(string fileName) {
	int i;
	for (i=fileName.size()-1;i>=0;i--) {
		if (fileName[i]=='/')
			return fileName.substr(0,i+1);
	}
	return "";
}

// devuelve el fichero de una ruta y fichero
string basename(string fileName) {
	for (int i=fileName.size()-1; i>=0; i--)
		if (fileName[i]=='/')
			return fileName.substr(i+1);
	return fileName;
}

// devuelve solo la extensión del fichero
string getExtension(string fileName) {
	for (int i=fileName.size()-1; i>=0; i--)
		if (fileName[i]=='.')
			return fileName.substr(i+1);
	return "";
}

// devuelve el fichero sin la extensión
string getWithoutExtension(string fileName) {
	for (int i=fileName.size()-1; i>=0; i--) {
		if (fileName[i]=='.')
			return fileName.substr(0, i);
		else if (fileName[i]=='/' || fileName[i]=='\\')
			break;
	}
	return fileName;
}

// realizar una copia de una estructura de fichero
void fileclone(File *fdst, File *forg) {
	fdst->isdir=forg->isdir;
	fdst->file=forg->file;
	fdst->path=forg->path;
	fdst->base=forg->base;
	fdst->icon=forg->icon;
}

// crear fichero
File *createFile(string fileName, string basePath) {
	File *f=new File;
	f->isdir=false;
	f->file=basename(fileName);
	f->path=dirname(fileName).substr(basePath.size());
	f->base=basePath;
	f->icon="";
	return f;
}

// predicado de ordenación
bool fileSortPredicate(const File* f1, const File* f2) {
	if (f1->isdir && !f2->isdir) return true;
	else {
		if ((f1->isdir && f2->isdir) || (!f1->isdir && !f2->isdir))
			if (_strtolower(f1->file) < _strtolower(f2->file)) return true;
	}
  return false;
}

// borrar directorio para evitar memory leaks
void clearDirectory(vector<File *> *files) {
	for (unsigned int i=0;i<files->size();i++)
		if (files->at(i))
			delete files->at(i);
	files->clear();
}

// leer un directorio
void readDirectory(vector<File *> *files, string base, string path, vector<string> *extensions) {

	File *f;
	string ext;

	clearDirectory(files);

	#ifdef _WIN32

		WIN32_FIND_DATA fdata;
		HANDLE dhandle;

		for (int searchfiles=0; searchfiles<=1; searchfiles++) {

			string findfilesin=base+path+"*";
			if ((dhandle=FindFirstFile((char *)findfilesin.c_str(),&fdata))==INVALID_HANDLE_VALUE)
				return;

			while (true) {
				if (FindNextFile(dhandle, &fdata)) {
					f=new File;
					f->file=fdata.cFileName;
					if (f->file=="..") continue;
					if (searchfiles && (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) continue;
					if (!searchfiles && !(fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) continue;
					f->isdir=(searchfiles?false:true);
					f->base=base;
					f->path=path;
					ext=getExtension(f->file);
					// *** TEMPORAL: Debe comprobar las extensiones RECONOCIDAS (IF inferior) y posteriormente
					// *** comprobar que existe el fichero item_EXTENSION.png en la carpeta del skin.
					// *** por ahora, va directo
					if (f->file.size()>4 && f->file.substr(f->file.size()-4)==".mp3")
						f->icon="item_mp3.png";
					// filtrado de extensiones
					if (!(fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && extensions!=NULL && extensions->size()) {
						vector<string>::iterator it;
						it=find(extensions->begin(), extensions->end(), _strtolower(ext));
						if (it==extensions->end())
							continue;
					}
					files->push_back(f);
				} else
					break;
			}

			FindClose(dhandle);

		}

	#endif

	#ifdef _LINUX

		//cout<<base+path<<endl;

		DIR *dp;
		struct dirent *dirp;
		if ((dp=opendir((base+path+".").c_str())) == NULL) {
			//cout<<"No se puede hacer opendir("<<base<<path<<".)"<<endl;
			return;
		}
		while ((dirp=readdir(dp)) != NULL) {
			f=new File;
			f->isdir=(dirp->d_type & DT_DIR);
			f->file=dirp->d_name;
			f->path=path;
			f->base=base;
			ext=getExtension(f->file);
			//cout << f->file << endl;
			// filtrado de extensiones
			if (!f->isdir && extensions!=NULL && extensions->size()) {
				vector<string>::iterator it;
				it=find(extensions->begin(), extensions->end(), _strtolower(ext));
				if (it==extensions->end()) {
					delete f;
					continue;
				}
			}
			if (f->isdir && (f->file=="." || f->file=="..")) delete f;
			else files->push_back(f);
		}
		closedir(dp);

		sort(files->begin(), files->end(), fileSortPredicate);

	#endif

}

// obtiene el anterior fichero de un directorio
File *getPrevFile(File *file, vector<string> *extensions) {
	return getNextFile(file,extensions,true);
}

// obtiene el siguiente fichero de un directorio
File *getNextFile(File *file, vector<string> *extensions, bool backward) {
	int i,iStart,iEnd,iStep;
	string subPath="";
	string subPathAux="";
	bool looped=false;
	bool searchNext;

	// precondiciones
	if (file->file=="") return NULL;

	// localizar el fichero actual
	bool fileFound=false;
	vector<File *> files;
	readDirectory(&files, file->base, file->path, extensions);
	for (i=0;i<(int)files.size();i++) {
		File *f=files[i];
		if (f->file==file->file) {
			fileFound=true;
			break;
		}
	}

	// si no se localiza, terminar ejecución aquí
	if (!fileFound) return NULL;

	// bucle de localización iterativo,
	// evita cargas innecesarias de localización de ficheros en disco
	// que pudiesen producir rutinas recursivas o scans de indexación,
	// permitiendo además que las listas puedan ser dinámicas.
	subPath=files[i]->path;
	while (true) {

		// movimiento, inicio y fin
		searchNext=false;
		if (backward) {
			iStart=i-1;
			iEnd=0;
			iStep=-1;
		} else {
			iStart=i+1;
			iEnd=files.size()-1;
			iStep=1;
		}

		// recorrido en la posición actual
		//alert(str(i)+" (DESDE "+str(iStart)+" HASTA "+str(iEnd)+" STEP "+str(iStep)+")");
		for (i=iStart; (iStep>0?i<=iEnd:i>=0); i+=iStep) {
			//alert("Buscando: "+files[i]->base+files[i]->path+files[i]->file);
			if (files[i]->isdir) {
				subPath=subPath+files[i]->file+"/";
				readDirectory(&files,file->base,subPath,extensions);
				i=(backward?files.size():-1); //i = IIf(backward, UBound(selectedFiles) + 1, 0)
				searchNext=true;
				break;
			} else {
				return files[i];
			}
		}
		if (searchNext) continue;

		// si no se ha llegado a la raíz, bajar
		if (subPath.size()) {
			subPathAux=subPath;
			subPath=dirGoUp(subPath);
			readDirectory(&files, file->base, subPath, extensions);
			//alert("BAJO A "+subPath);
			for (i=0; i<(int)files.size(); i++) {
				if (files[i]->path+files[i]->file+"/"==subPathAux) {
					searchNext=true;
					//alert("AQUI ESTOY:"+files[i]->path+files[i]->file+"/"+" anterior era: "+subPathAux);
					break;
				}
			}
			if (searchNext) continue;
			// aquí no debe llegar a ejecutarse nunca, pero por "because if the flies",
			// se presupone que se ha vuelto a la raíz para no quedar en un bucle infinito
			subPath="";
		}

		// si se ha hecho todavía una vuelta, detener,
		// sino, buscar siguiente desde el principio,
		if (looped) {
			break;
		} else {
			looped=true;
			readDirectory(&files, file->base, subPath, extensions);
			i=(backward?files.size():-1);
		}

	}

	return NULL;

}
