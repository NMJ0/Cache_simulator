#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include  "cache_block.h"
#include "parse.h"



int main(int argc, char *argv[]){
	FILE *trace_file = fopen("gcc_trace.txt", "r");
        if (trace_file == NULL) {
        	printf("Error opening file.\n");
        	return 1;
        }
        
        if (argc != 8) {
        printf("Usage:  <l1_size> <l1_assoc> <l1_block_size> <l1_vc_num_blocks>  <l2_size> <l2_assoc>  <policy> \n");
        return 1;
        }
        srand(12345);

    	
    	int assoc = atoi(argv[2]);
    	int size = atoi(argv[1]);
    	int block_size = atoi(argv[3]);
    	int vc_num_blocks=atoi(argv[4]);
        int vc_enable =0   ;
        //if (vc_num_blocks>0){vc_enable=1;}
        int assoc_l2=atoi(argv[6]);
        int size_l2=atoi(argv[5]);
        int policy=atoi(argv[7]);
        int block_size_l2=block_size;        
        
        Cache_block L1;
        Cache_block L1_results;
        
        int num_sets=(size / (block_size * assoc));
       
        L1_results.read_hits=0;
        L1_results.read_miss=0;
        L1_results.write_hits=0;
        L1_results.write_miss=0;
        L1_results.swap_requests=0;
        L1_results.no_swaps=0;
        L1_results.wb=0;
        
        int cache[num_sets][assoc];
        int counters[num_sets][assoc];
        int dirty[num_sets][assoc];
        int vc[vc_num_blocks];
        int vc_counter[vc_num_blocks];
        int vc_dirty[vc_num_blocks];
        int plru_tree[num_sets][assoc-1];
        int x=0;

    
        initialize_cache(assoc,num_sets,vc_num_blocks,block_size,cache,counters,dirty,vc,vc_counter,vc_dirty,policy, plru_tree);
        

        Cache_block L2;
        Cache_block L2_results;

        int num_sets_l2=0;
        if (size_l2 !=0){num_sets_l2=(size_l2 / (block_size_l2 * assoc_l2));}
        int vc_num_blocks_l2=0;
        int vc_enable_l2=0;
        
        L2_results.read_hits=0;
        L2_results.read_miss=0;
        L2_results.write_hits=0;
        L2_results.write_miss=0;
        L2_results.swap_requests=0;
        L2_results.no_swaps=0;
        L2_results.wb=0;
        
        int cache_[num_sets_l2][assoc_l2];
        int counters_[num_sets_l2][assoc_l2];
        int dirty_[num_sets_l2][assoc_l2];
        int vc_[vc_num_blocks_l2];
        int vc_counter_[vc_num_blocks_l2];
        int vc_dirty_[vc_num_blocks_l2];
        int plru_tree_2[num_sets_l2][assoc_l2-1];
        int y=0;

    
        initialize_cache(assoc_l2,num_sets_l2,vc_num_blocks_l2,block_size_l2,cache_,counters_,dirty_,vc_,vc_counter_,vc_dirty_,policy,plru_tree_2);        
        
        
        char operation;
        unsigned int address;
        while (fscanf(trace_file, "%c %x\n", &operation, &address) != EOF){
        	
       	L1=cache_block(assoc,num_sets, vc_num_blocks, block_size,operation,address,vc_enable,cache,counters,dirty,vc,vc_counter,vc_dirty,policy,plru_tree,&x) ;
       	L1_results.read_hits+=L1.read_hits;
       	L1_results.read_miss+=L1.read_miss;
       	L1_results.write_hits+=L1.write_hits;
       	L1_results.write_miss+=L1.write_miss;
       	L1_results.swap_requests+=L1.swap_requests;
       	L1_results.no_swaps+=L1.no_swaps;
       	L1_results.wb+=L1.wb;
       	char op;
       	
       	
       	if ((L1.read_miss+L1.write_miss-L1.no_swaps==1 || L1.wb==1) & (size_l2 !=0)){
       	
       		if (L1.read_miss==1 || L1.write_miss==1){op='r';}
	       	L2=cache_block(assoc_l2,num_sets_l2, vc_num_blocks_l2, block_size_l2,op,address,0,cache_,counters_,dirty_,vc_,vc_counter_,vc_dirty_,policy,plru_tree_2,&y) ;
	       	L2_results.read_hits+=L2.read_hits;
	       	L2_results.read_miss+=L2.read_miss;
	       	L2_results.write_hits+=L2.write_hits;
	       	L2_results.write_miss+=L2.write_miss;
	       	L2_results.wb+=L2.wb;
	       	if (L1.wb==1){
	       		op='w';
	       		address=L1.wb_address;
	       		L2=cache_block(assoc_l2,num_sets_l2, vc_num_blocks_l2, block_size_l2,op,address,0,cache_,counters_,dirty_,vc_,vc_counter_,vc_dirty_,policy,plru_tree_2,&y) ;
	       		L2_results.read_hits+=L2.read_hits;
	       		L2_results.read_miss+=L2.read_miss;
	       		L2_results.write_hits+=L2.write_hits;
	       		L2_results.write_miss+=L2.write_miss;
	       		L2_results.wb+=L2.wb;
	       	}
       	
       	}
       	
       	
        
       }
       printf("===== Simulator configuration =====\n");
       printf("L1_SIZE      :%d \n",size);
       printf("L1_ASSOC     :%d \n",assoc);
       printf("L1_BLOCKSIZE :%d \n",block_size);
       printf("VC_NUM_BLOCKS:%d \n",vc_num_blocks);
       printf("L2_SIZE      :%d \n",size_l2);
       printf("L2_ASSOC     :%d \n",assoc_l2);
       printf("TRACE_FILE   :gcc_trace.txt \n\n");
       printf("===== L1 contents =====\n");
       print_cache(num_sets,assoc,cache,dirty);
       printf("\n");
       if (vc_enable==1){
       	print_vc(vc_num_blocks,vc,vc_dirty);
       	printf("\n");
       }
       if (size_l2!=0){
       	printf("===== L2 contents =====\n");
       	print_cache(num_sets_l2,assoc_l2,cache_,dirty_);
             
       	printf("\n");
       }
       fclose(trace_file);
       printf("===== Simulation results (raw) =====\n");
       
       
       
       float sum=(L1_results.read_hits+L1_results.read_miss+L1_results.write_hits+L1_results.write_miss);
       float combined_miss_rate=(L1_results.read_miss+L1_results.write_miss-L1_results.no_swaps)/sum;
	    
       float swap_req_rate=(L1_results.swap_requests/sum);
       int tmt;
       if (size_l2 ==0){tmt=L1_results.read_miss+L1_results.write_miss-L1_results.no_swaps+L1_results.wb;}
       else{tmt=L2_results.read_miss+L2_results.write_miss+L2_results.wb;}
       float s=L2_results.read_hits+L2_results.read_miss;
       
       float L2_miss_rate=L2_results.read_miss/s;
       if (size_l2==0){L2_miss_rate=0;}
       printf("a. number  of L1  reads                : %d \n",L1_results.read_hits+L1_results.read_miss);
       printf("b. number  of L1  read misses          : %d \n",L1_results.read_miss);
       printf("c. number  of L1  writes               : %d \n",L1_results.write_hits+L1_results.write_miss);
       printf("d. number  of L1  write misses         : %d \n",L1_results.write_miss);
       printf("e. number  of swap requests            : %d \n",L1_results.swap_requests);
       printf("f. swap request rate                   : %4f \n",swap_req_rate);
       printf("g. number  of swaps                    : %d \n",L1_results.no_swaps);
       printf("h. combined L1+VC miss rate            : %f \n",combined_miss_rate);
       printf("i. number  of write backs from L1/VC   : %d \n",L1_results.wb);
       printf("j. number  of L2  reads                : %d \n",L2_results.read_hits+L2_results.read_miss);
       printf("k. number  of L2  read misses          : %d \n",L2_results.read_miss);
       printf("l. number  of L2  writes               : %d \n",L2_results.write_hits+L2_results.write_miss);
       printf("m. number  of L2  write misses         : %d \n",L2_results.write_miss);
       printf("n. L2 miss rate                        : %f \n",L2_miss_rate);
       printf("o. number  of write backs from L2      : %d \n",L2_results.wb);
       printf("p. Total memory traffic                : %d \n",tmt);

      
       printf("%d %d %d %d %d %d %f %f %f %d %d %d %d %d %d %d %d %d %d %d %d %d %d",size, block_size, assoc,vc_num_blocks,size_l2,assoc_l2,swap_req_rate,combined_miss_rate,L2_miss_rate,L1_results.read_hits+L1_results.read_miss,L1_results.write_hits+L1_results.write_miss,L1_results.read_miss,L1_results.write_miss,L2_results.read_hits+L2_results.read_miss,L2_results.write_hits+L2_results.write_miss,L2_results.read_miss,L2_results.write_miss,L1_results.no_swaps,L1_results.wb,L2_results.wb,tmt,L1_results.swap_requests);
       return 0;
       
        
        
        
        
}




