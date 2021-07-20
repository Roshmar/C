#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "options.h"




void PrintCopymasterOptions(struct CopymasterOptions* cpm_options);

void FatalError(char c, const char* msg, int exit_status){
    fprintf(stderr, "%c:%d\n", c, errno);
    fprintf(stderr, "%c:%s\n", c, strerror(errno));
    fprintf(stderr, "%c:%s\n", c, msg);
    exit(exit_status);
}
void SlowCopy(int inputFile,int outputFile,const char* secondFile, struct stat sb){
    fstat(outputFile,&sb);
    if(inputFile < 0 || sb.st_mode == 32768){
        FatalError('s',"Subor infile neexistuje",21);
    }
    if(outputFile < 0){
        fstat(inputFile,&sb);
        outputFile = open(secondFile,O_CREAT | O_TRUNC | O_RDWR );
        chmod(secondFile,sb.st_mode);
    }
    fstat(outputFile,&sb);
//    if(){
//
//    }

    char buf1 = 0;
    int n;
    while((n = read(inputFile, &buf1, 1))>0){
        write(outputFile,&buf1,1);
    }
    close(inputFile);
    close(outputFile);
}
void Fast(int inputFile,int outputFile,const char* secondFile, struct stat sb){
    fstat(outputFile,&sb);
    if(inputFile < 0 || sb.st_mode == 32768){
        FatalError('s',"Subor infile neexistuje",21);
    }
    if(outputFile < 0){
        fstat(inputFile,&sb);
        outputFile = open(secondFile,O_CREAT | O_TRUNC | O_RDWR );
        chmod(secondFile,sb.st_mode);
    }
    fstat(outputFile,&sb);
//    if(){
//
//    }

    char buf1[40000];
    memset(buf1,'\0',40000);
    int n;
    while((n = read(inputFile, &buf1, 40000))>0){
        write(outputFile,&buf1,40000);
    }
    close(inputFile);
    close(outputFile);
}


int main(int argc, char* argv[])
{
    struct CopymasterOptions cpm_options = ParseCopymasterOptions(argc, argv);
    struct stat sb;
    int inputFile = open(cpm_options.infile,O_RDONLY);
    int outputFile;
    //-------------------------------------------------------------------
    // Kontrola hodnot prepinacov

    //-------------------------------------------------------------------

    // Vypis hodnot prepinacov odstrante z finalnej verzie
    
    PrintCopymasterOptions(&cpm_options);
    
    //-------------------------------------------------------------------
    // Osetrenie prepinacov pred kopirovanim
    //-------------------------------------------------------------------
    
    if (cpm_options.fast && cpm_options.slow) {
        fprintf(stderr, "CHYBA PREPINACOV\n"); 
        exit(EXIT_FAILURE);
    }
    
    // TODO Nezabudnut dalsie kontroly kombinacii prepinacov ...
    
    //-------------------------------------------------------------------
    // Kopirovanie suborov
//    if(cpm_options.umask){
//        if((outputFile = open(cpm_options.outfile,O_RDWR) >0)){
//            fstat(outputFile,&sb);
//            st_mode = cpm_options.umask_options;
//            umask(sb.st_mode);
//        }else{
//            FatalError('u',"Subor outfile neexistuje",32);
//        }
//    }

    if(cpm_options.slow){
            outputFile = open(cpm_options.outfile,O_TRUNC | O_RDWR );
            SlowCopy(inputFile,outputFile,cpm_options.outfile,sb);
    } else if(cpm_options.fast){
            outputFile = open(cpm_options.outfile,O_TRUNC | O_RDWR );
            Fast(inputFile,outputFile,cpm_options.outfile,sb);
    } else if(argv[3] == 0){
        outputFile = open(cpm_options.outfile, O_TRUNC | O_RDWR );
        SlowCopy(inputFile,outputFile,cpm_options.outfile,sb);
    }

    //-------------------------------------------------------------------
    
    // TODO Implementovat kopirovanie suborov
    
    // cpm_options.infile
    // cpm_options.outfile

    //-------------------------------------------------------------------
    // Vypis adresara
    //-------------------------------------------------------------------
    
    if (cpm_options.directory) {
        // TODO Implementovat vypis adresara
    }
        
    //-------------------------------------------------------------------
    // Osetrenie prepinacov po kopirovani
    //-------------------------------------------------------------------
    
    // TODO Implementovat osetrenie prepinacov po kopirovani
    if(cpm_options.create){
        inputFile = open(cpm_options.infile,O_RDONLY);
        if(open(cpm_options.outfile,O_RDWR) == -1){
            outputFile = open(cpm_options.outfile,O_CREAT | O_EXCL |O_RDWR);
            SlowCopy(inputFile,outputFile,cpm_options.outfile,sb);
            chmod(cpm_options.outfile,cpm_options.create_mode);
        } else{
            close(inputFile);
            FatalError('c',"Subor outfile existuje",23);
        }

    }
    if(cpm_options.overwrite){
        inputFile = open(cpm_options.infile,O_RDONLY);
        outputFile = open(cpm_options.outfile,O_RDWR);
        if(outputFile == 0){
            SlowCopy(inputFile,outputFile,cpm_options.outfile,sb);
        } else{
            close(inputFile);
            FatalError('o',"Subor outfile neexistuje",24);
        }
    }
    if(cpm_options.lseek){
        char buf = 0;
        if((inputFile = open(cpm_options.infile,O_RDONLY))<0){
            FatalError('a',"Subor infile neexistuje",33);
        }
        lseek(inputFile,cpm_options.lseek_options.pos1,SEEK_SET);
        read(inputFile,&buf,cpm_options.lseek_options.num);

        if((outputFile = open(cpm_options.outfile,O_RDWR))<0){
            close(inputFile);
            FatalError('a',"Subor outfile neexistuje",33);
        }

        lseek(outputFile,cpm_options.lseek_options.pos2,cpm_options.lseek_options.x);
        write(outputFile,&buf,cpm_options.lseek_options.num);
        close(inputFile);
        close(outputFile);
    }
    if(cpm_options.delete_opt){
            inputFile = open(cpm_options.infile,O_RDONLY);
        if((cpm_options.slow && cpm_options.fast) == 0){
            outputFile = open(cpm_options.outfile,O_RDWR);
            SlowCopy(inputFile,outputFile,cpm_options.outfile,sb);
        }
            unlink(cpm_options.infile);

            if((open(cpm_options.infile,O_RDONLY)) == 0){
                FatalError('d',"Subor infile ne bol zmazany",26);
            }
    }
    if(cpm_options.append){
        inputFile  = open(cpm_options.infile,O_RDONLY);
        if((outputFile = open(cpm_options.outfile,O_RDWR | O_APPEND))>=0){
            SlowCopy(inputFile,outputFile,cpm_options.outfile,sb);
        } else{

            close(inputFile);
            FatalError('a',"Subor outfile neexistuje",22);
        }
    }
    if(cpm_options.chmod){
        inputFile = open(cpm_options.infile,O_RDONLY);
        if((cpm_options.slow || cpm_options.fast) == 0){
            outputFile = open(cpm_options.outfile,O_RDWR);
            SlowCopy(inputFile,outputFile,cpm_options.outfile,sb);
        }
        chmod(cpm_options.outfile,cpm_options.chmod_mode);
    }
    if (cpm_options.inode){
        fstat(inputFile,&sb);
        if(cpm_options.inode_number == sb.st_ino){
            outputFile = open(cpm_options.outfile,O_RDWR);
            SlowCopy(inputFile,outputFile,cpm_options.outfile,sb);
        } else{

            close(inputFile);
            FatalError('i',"Zly inode",27);
        }
    }
    if(cpm_options.link){
        if(link(cpm_options.infile,cpm_options.outfile) == -1){
            FatalError('K',"SPRAVA",30);
        }
    }
    if(cpm_options.truncate){
        if((cpm_options.slow || cpm_options.fast) == 0){
            inputFile = open(cpm_options.infile,O_RDONLY);
            outputFile = open(cpm_options.outfile,O_RDWR);
            SlowCopy(inputFile,outputFile,cpm_options.outfile,sb);
        }
        if(truncate(cpm_options.infile,cpm_options.truncate_size) == -1){
            FatalError('K',"SPRAVA",31);
        }
    }
    
    return 0;
}




void PrintCopymasterOptions(struct CopymasterOptions* cpm_options){
    if (cpm_options == 0)
        return;
    
    printf("infile:        %s\n", cpm_options->infile);
    printf("outfile:       %s\n", cpm_options->outfile);
    
    printf("fast:          %d\n", cpm_options->fast);
    printf("slow:          %d\n", cpm_options->slow);
    printf("create:        %d\n", cpm_options->create);
    printf("create_mode:   %o\n", (unsigned int)cpm_options->create_mode);
    printf("overwrite:     %d\n", cpm_options->overwrite);
    printf("append:        %d\n", cpm_options->append);
    printf("lseek:         %d\n", cpm_options->lseek);
    
    printf("lseek_options.x:    %d\n", cpm_options->lseek_options.x);
    printf("lseek_options.pos1: %ld\n", cpm_options->lseek_options.pos1);
    printf("lseek_options.pos2: %ld\n", cpm_options->lseek_options.pos2);
    printf("lseek_options.num:  %lu\n", cpm_options->lseek_options.num);
    
    printf("directory:     %d\n", cpm_options->directory);
    printf("delete_opt:    %d\n", cpm_options->delete_opt);
    printf("chmod:         %d\n", cpm_options->chmod);
    printf("chmod_mode:    %o\n", (unsigned int)cpm_options->chmod_mode);
    printf("inode:         %d\n", cpm_options->inode);
    printf("inode_number:  %lu\n", cpm_options->inode_number);
    
    printf("umask:\t%d\n", cpm_options->umask);
    for(unsigned int i=0; i<kUMASK_OPTIONS_MAX_SZ; ++i) {
        if (cpm_options->umask_options[i][0] == 0) {
            // dosli sme na koniec zoznamu nastaveni umask
            break;
        }
        printf("umask_options[%u]: %s\n", i, cpm_options->umask_options[i]);
    }
    
    printf("link:          %d\n", cpm_options->link);
    printf("truncate:      %d\n", cpm_options->truncate);
    printf("truncate_size: %ld\n", cpm_options->truncate_size);
    printf("sparse:        %d\n", cpm_options->sparse);
}

