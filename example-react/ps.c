/**
 * psearch.c
 *
 * Performs a parallel directory search over the file system.
 *
 * Author:
 */

#define _GNU_SOURCE
#define default_path "."

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <strings.h>

void match_result(char *path,char *search_term, char *file_name,bool isExact,bool isCWD);
void listdir(char *name, char *search_term,bool isExact, bool isCWD);
void print_usage(char *argv[]);
void *search_thread(void *opts);

sem_t thread_semaphore;

/* Declare struct 'setFlag' */
struct setFlag{

	char *path;
	char *term;
	int maxThread;
	bool hasExact;
	bool isCurrentDir; 

};

/* search and print path if search term is matched to file name */
void match_result(char *path, char *search_term, char *file_name,bool isExact, bool isCWD){
   
   char cwd[1024];
   char real_path[1024];
    
   if(isCWD == 1){
       //getcwd(cwd,sizeof(cwd));
       //snprintf(real_path, sizeof(real_path),"%s/%s", cwd, file_name);
       //path = real_path;       
       realpath(path, real_path);
       
   }else{
       //realpath = path;
       snprintf(real_path, sizeof(real_path),"%s", path );
       

   }
   
   if((strcasecmp(file_name,search_term) == 0) && ( strcasestr(file_name,search_term) != NULL && isExact != 1) ){
   
	   printf("%s\n",real_path);
   }

   else if((strcasestr(file_name,search_term) != NULL && isExact != 1) || (strcasecmp(file_name,search_term) == 0)){
       printf("%s\n",real_path);
   }

}

/* set up default value to struct if path or maximum thread number is not explicitly specified. */
void set_default_value(struct setFlag *f, int cpu){
    f->path = default_path; 
    f->hasExact = false;
    f->maxThread = get_nprocs();
    f->isCurrentDir = true;
}

/*function to travers directory and file recursively.*/
void listdir(char *name,char *search_term,bool isExact, bool isCWD)
{
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(name)))
        return;
 
    char entryName[1024];
    char pra_path[1024];
    char chil_path[1024];
    char path[1024];
    
    while ((entry = readdir(dir)) != NULL) {

        //snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
        //snprintf(entryName,sizeof(entryName),"%s",entry->d_name);
        
	/* If it is a directory  */
        if (entry->d_type == 4) {
            
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            
            else{
	        snprintf(chil_path, sizeof(chil_path), "%s/%s", name, entry->d_name);
                snprintf(entryName,sizeof(entryName),"%s",entry->d_name);
    		
	      
	    }	    
	    //snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
            //snprintf(entryName,sizeof(entryName),"%s",entry->d_name);

            match_result(chil_path,search_term,entryName,isExact, isCWD); 
            listdir(chil_path,search_term,isExact,isCWD);

        } 

	/* If it is a file  */
	else{
            snprintf(pra_path, sizeof(pra_path), "%s/%s", name, entry->d_name);
            snprintf(entryName,sizeof(entryName),"%s",entry->d_name);
	    
	    //printf("listdir\n");
            match_result(pra_path,search_term,entryName,isExact, isCWD); 
	}
        
    }
    closedir(dir);
}

/* search method for thread to run */
void *search_thread(void *opts)
{
    
    struct setFlag *f = (struct setFlag *)opts;
    listdir(f->path,f->term,f->hasExact,f->isCurrentDir);

    sem_post(&thread_semaphore);

    return 0;
}

/* print usage if '-h' is on command line. */
void print_usage(char *argv[])
{
    printf("Usage: %s [-eh] [-d directory] [-t threads] "
            "search_term1 search_term2 ... search_termN\n" , argv[0]);
    printf("\n");
    printf("Options:\n"
           "    * -d directory    specify start directory (default: CWD)\n"
           "    * -e              print exact name matches only\n"
           "    * -h              show usage information\n"
           "    * -t threads      set maximum threads (default: # procs)\n");
    printf("\n");
}

/* parsing command line and create threads to carry out tasks for searching. */
int main(int argc, char *argv[])
{
    int c;
  
    opterr = 0;
    struct setFlag *flag; 
    int CPU_num = get_nprocs();
    
    flag = (struct setFlag *) malloc(sizeof(struct setFlag));
    set_default_value(flag, CPU_num);

    /* Handle command line options */
    while ((c = getopt(argc, argv, "d:eht:")) != -1) {
        switch (c) {
            case 'd':
		flag->path = optarg;
		flag->isCurrentDir = false;
		break;

	    case 't':
		flag->maxThread = atoi(optarg);
		break;

	    case 'e':
		flag->hasExact = true;
		break;

	    case 'h':
		printf("******************************************************************************************\n\n");
		print_usage(argv);
		printf("******************************************************************************************\n");
		
		
		break;

	    case '?':
		if(optopt == 'd' || optopt == 't'){
		    fprintf(stderr,"Option -%c requires an argument.\n", optopt);
	 	} 
	
	    default:
	        abort();

	}
    }
        
    int i;
      
    sem_init(&thread_semaphore, 0, flag->maxThread);
    pthread_t thread[argc - optind]; 

    for (i = optind; i < argc; i++) {
	
	flag->term = argv[i];
	sem_wait(&thread_semaphore);
	pthread_create(&thread[i - optind], NULL, search_thread,(void *)flag);
        pthread_join(thread[ i - optind],NULL);	
	
    }

    free(flag); /* free up space */
    sem_destroy(&thread_semaphore);

    return 0;
}

