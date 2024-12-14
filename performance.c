#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse.h"

int get_cacti_results(int SIZE, int BLOCKSIZE, int ASSOC, float *AccessTime, float *Energy, float *Area);


#define OUTPUT_FILE "results.txt"


#define MAX_LINE_LENGTH 1024

void remove_last_line(const char *filename) {
    FILE *file = fopen(filename, "r");
    FILE *temp = fopen("temp.txt", "w");
    char line[MAX_LINE_LENGTH];
    char prev_line[MAX_LINE_LENGTH] = {0};

    if (!file || !temp) {
        perror("Error opening file");
        if (file) fclose(file);
        if (temp) fclose(temp);
        exit(EXIT_FAILURE);
    }

  
    while (fgets(line, sizeof(line), file)) {
        
        if (strlen(prev_line) > 0) {
            fputs(prev_line, temp);
        }
        
        strcpy(prev_line, line);
    }

    
    fclose(file);
    fclose(temp);

    
    if (rename("temp.txt", filename) != 0) {
        perror("Error renaming temp file");
        exit(EXIT_FAILURE);
    }
}


void parse_last_line(const char *filename, int *size, int *block_size, int *assoc,int *vc_size,int *size_l2,int *assoc_l2,float *swap_rate,float *miss_rate_l1,float *miss_rate_l2,int *l1reads,int *l1writes,int *l1readmiss,int *l1writemiss,int *l2reads,int *l2writes,int *l2readmiss,int *l2writemiss,int *swaps,int *wb1,int *wb2,int *tmt,int *swap_requests) {
    FILE *file = fopen(filename, "r");
    char line[MAX_LINE_LENGTH] = {0};

    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    
    while (fgets(line, sizeof(line), file)) {
        
    }

    
    if (sscanf(line, "%d %d %d %d %d %d %f %f %f %d %d %d %d %d %d %d %d %d %d %d %d %d ",size, block_size, assoc,vc_size,size_l2,assoc_l2,swap_rate,miss_rate_l1,miss_rate_l2,l1reads,l1writes,l1readmiss,l1writemiss,l2reads,l2writes,l2readmiss,l2writemiss,swaps,wb1,wb2,tmt,swap_requests)  != 22) {
        fprintf(stderr, "Error parsing values from the last line\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    



    fclose(file);
}


int main() {
    int size, block_size, assoc,vc_size,size_l2,assoc_l2,l1reads,l1writes,l1readmiss,l1writemiss,l2reads,l2writes,l2readmiss,l2writemiss,swaps,wb1,wb2,tmt,swap_requests;
    float swap_rate,miss_rate_l1,miss_rate_l2;
    float AccessTime;
    float Energy;
    float Area;
    int q;

  
    parse_last_line(OUTPUT_FILE, &size, &block_size, &assoc,&vc_size,&size_l2,&assoc_l2,&swap_rate,&miss_rate_l1,&miss_rate_l2,&l1reads,&l1writes,&l1readmiss,&l1writemiss,&l2reads,&l2writes,&l2readmiss,&l2writemiss,&swaps,&wb1,&wb2,&tmt,&swap_requests);
    remove_last_line(OUTPUT_FILE);

    
   
  

    float l1Er = 0, l2Er = 0, vcEr = 0, memEr = .05;
    float l1Ar = 0, l2Ar = 0, vcAr = 0;
    float l1Tm = 0, l2Tm = 0, vcTm = 0, memTm = 0;

    memTm = 20 + (float)block_size / 16;



    int err = get_cacti_results(size, block_size, assoc, &l1Tm, &l1Er, &l1Ar);

    vc_size=vc_size*block_size;

    if (vc_size > 0)
    {
        
        err = get_cacti_results(vc_size, block_size, 1, &vcTm, &vcEr, &vcAr);
         
        if(vcTm == 0)
            vcTm = 0.2;
    }

    if (size_l2 > 0)
    {
        err = get_cacti_results(size_l2, block_size, assoc_l2, &l2Tm, &l2Er, &l2Ar);
    }
 
  

   
     float miss_penalty;
    if (size_l2==0){
     	miss_penalty=memTm;
     	Energy=(l1reads + l1writes) * l1Er + (l1readmiss+l1writemiss)*l1Er + 2*swap_requests*vcEr + (l1readmiss+l1writemiss-swaps)*memEr +wb1*memEr;
     
     }
    else{
     	miss_penalty=l2Tm + miss_rate_l2 * memTm;
     	Energy=(l1reads + l1writes) * l1Er + (l1readmiss+l1writemiss)*l1Er + 2*swap_requests*vcEr  +(l2reads + l2writes) * l2Er + (l2readmiss+l2writemiss)*l2Er + (l2readmiss+l2writemiss)*memEr +  +wb2*memEr ;
     	
     	
     	
     }

    AccessTime = (l1Tm + swap_rate *  vcTm) + miss_rate_l1 * (miss_penalty);
   


   
    Area = vcAr + l1Ar + l2Ar;
    float EDP = Energy * AccessTime * (l1reads + l1writes);

    printf( "\n===== Simulation results (performance) =====\n");
    printf( " 1. average access time: %.4f   \n" , AccessTime);
    printf("  2. energy-delay product:  %.4f \n" , EDP );
    printf("  3. total area:  %.4f \n" ,Area );
    //printf("%d %d %d %d %d %d  ",l1reads,l1writes,l1readmiss,l1writemiss,swap_requests,wb1);
    //printf("%d",size);
    //remove_last_line(OUTPUT_FILE);
    //printf("%d %d %d %d %d %d %f %f %f %d %d %d %d %d %d %d %d %d %d %d %d %d ",size, block_size, assoc,vc_size,size_l2,assoc_l2,swap_rate,miss_rate_l1,miss_rate_l2,l1reads,l1writes,l1readmiss,l1writemiss,l2reads,l2writes,l2readmiss,l2writemiss,swaps,wb1,wb2,tmt,swap_requests);
    return 0;
}


