#include "m_pd.h"
#include "dirent.h"
#include <string.h>

// #ifdef _WIN32

//  A variable of type t_class that holds the class definition for our external
static t_class *directory_class;

// This is where the data of our external will live. It is mandatory 
// to have a x_obj of type t_object and it must be the first data member 
// in the structure.
typedef struct _directory {
   t_object x_obj;
   t_outlet *outlet_1;
   t_outlet *outlet_2;
   DIR *dir;
   char* working_dir;
} directory;

// Called by pd when we receive a bang
void directory_bang(directory *this){
   if(!this->dir){
      return;
   }
      
   t_atom out[1];
   struct dirent *entry;
   if(NULL != (entry = readdir(this->dir))){
      SETSYMBOL(&out[0], gensym(entry->d_name));
      outlet_list(this->outlet_1, &s_list, 1, &out[0]);
   }
   else{
      outlet_bang(this->outlet_2);
   }
}

// Called by pd when we receive a rewind symbol
void directory_rewind(directory *this) {
   if(!this->dir) return;
   rewinddir(this->dir);
}

// Called by pd when we receive a read symbol
void directory_read(directory *this, t_symbol *s) {
   if(!s && !s->s_name) {
      error("You must specify a directory");
      return;
   }
   
   char path[FILENAME_MAX];
   if(sys_isabsolutepath(s->s_name)){
      strcpy(path, s->s_name);
   }
   else{
      strcpy(path, this->working_dir);
      strcat(path, "/");
      strcat(path, s->s_name);
   }

   this->dir = opendir(path);
   if(!this->dir){
      error("Could not open directory %s \n", s->s_name);
      return;
   }
}

// Called by pd when destroying an instance of this external
void directory_free(directory *this){
   closedir(this->dir);
}

// Called by pd when creating an instance of this external
void* directory_new(t_symbol* s, int argc, t_atom *argv){
   // Get rid of compiler warnings (unused parameters)
   (void)s;
   (void)argc;
   (void)argv;

   directory *instance = (directory *)pd_new(directory_class);

   instance->outlet_1 = outlet_new(&instance->x_obj, &s_list);
   instance->outlet_2 = outlet_new(&instance->x_obj, &s_bang);
   
   instance->working_dir = canvas_getcurrentdir()->s_name;

   return (void *)instance;
}

// Called by pd when loading the external. 
void directory_setup(void){
   directory_class = class_new( gensym("directory"), (t_newmethod)directory_new, (t_method)directory_free, sizeof(directory), CLASS_DEFAULT, A_GIMME, 0);
   class_addbang(directory_class, directory_bang);
   class_addmethod(directory_class, (t_method)directory_read, gensym("read"), A_DEFSYMBOL, 0);
   class_addmethod(directory_class, (t_method)directory_rewind, gensym("rewind"), 0);
}
