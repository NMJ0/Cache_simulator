#include <stdio.h>
#include <stdlib.h>
#include <math.h> 
#include <time.h>
#include "functions.h"
typedef struct {
     int read_hits;
     int read_miss;
     int write_hits;
     int write_miss;
     int swap_requests;
     int no_swaps;
     int wb;
     int wb_address;
     
} Cache_block;


Cache_block cache_block(int assoc, int num_sets, int vc_num_blocks,int block_size, char operation,unsigned int address,int vc_enable,int cache[num_sets][assoc],int counters[num_sets][assoc],int dirty[num_sets][assoc],int vc[vc_num_blocks],int vc_counter[vc_num_blocks],int vc_dirty[vc_num_blocks],int policy,int plru_tree[num_sets][assoc-1],int *x) {
       
    int read_hits=0;
    int read_miss=0;
    int write_hits=0;
    int write_miss=0;
    int wb=0;
    int swap_requests=0;
    int no_swaps=0;
    int wb_address=0;
    if (1==1){
        
        AddressFields fields = extract_fields(address,num_sets,block_size);
        unsigned int index;
        unsigned int tag;
        unsigned int block_address;
        index=fields.index;
        tag=fields.tag;
        block_address=fields.block_address;
                
        int found=0;
        int vc_found=0;
        int filled=1;
        
        if (operation=='r') {
        	for (int i = 0; i < assoc; i++) {
        		
        	 	if (cache[index][i]==tag){
        	 		read_hits+=1;
					if (policy==0){
						access_block(num_sets,assoc,counters,index, i);
						

					}
					else if (policy==1){
						update_plru(num_sets,assoc,index,plru_tree,i);
					}
					else if (policy==2){
						int f=0;
					
					}
        	 		
        	 		found=1;
        	 		break;	
        	 	}
        	 	if (cache[index][i]==NULL){filled=0;}
        	        		
    		}
    		if (found==0){
    			read_miss+=1;
    			
    			if (vc_enable==1 & filled==1 ){
    				swap_requests++;
    				for (int i = 0; i < vc_num_blocks; i++) {
    					if (vc[i]==block_address){
    						vc_found=1;
    						no_swaps++;
    						
    					
    						int lru=find_lru_block(num_sets,assoc,counters,index);
    						unsigned int address_size_without_offset=address_size-(int)log2(block_size);
    						int block_addr=calculate_block_address(cache[index][lru],index,address_size_without_offset,num_sets);
    						
    						
    						vc[i]=block_addr;
    						
    						
    						TagIndex address=extract_tag_index(block_address,block_size,num_sets);
    						cache[index][lru]=address.tag;
    						int a=vc_dirty[i];
    						vc_dirty[i]=dirty[index][lru];
    						dirty[index][lru]=a;
    						
    						vc_access(vc_num_blocks,vc_counter,i);
    						access_block(num_sets,assoc,counters,index,lru);
    						break;
    						
    					}
    				}
    				if (vc_found==0){

    					int lru=find_lru_block(num_sets,assoc,counters,index);
    					
    					int vc_lru=find_vc_lru(vc_num_blocks,vc_counter);
    					unsigned int address_size_without_offset=address_size-(int)log2(block_size);
    					
	    				if (vc_dirty[vc_lru]==1) {
	    					wb+=1;
	    					wb_address=extend_to_32_bits(vc[vc_lru],address_size_without_offset);
	    					
	    				}
	    				vc[vc_lru]=calculate_block_address(cache[index][lru],index,address_size_without_offset,num_sets);
	    				cache[index][lru]=tag;
	    				
	    				vc_dirty[vc_lru]=dirty[index][lru];
	    				dirty[index][lru]=0;
	    				access_miss(num_sets,assoc,counters,index, lru);
	    				vc_miss(vc_num_blocks,vc_counter,vc_lru);
    					
    				}
    				
    			
    				
    				
    			}
    			else {
    				int lru=1;
	    			if (policy==0){ lru=find_lru_block(num_sets,assoc,counters,index);}
	    			else if (policy==1){lru=find_plru_block(assoc,num_sets,plru_tree,index);}
	    			else if (policy==2){
	    				
	    				int random_number = rand() % (assoc);
	    				lru=random_number;
	    			}
	    			else if (policy==3){
	    				lru=*x;
	    				*x+=1;
	    				*x=*x % assoc;
	    			}
			
	    			
	    			if (dirty[index][lru]==1) {
	    				wb+=1;
	    				unsigned int address_size_without_offset=address_size-(int)log2(block_size);
	    				wb_address=calculate_block_address(cache[index][lru],index,address_size_without_offset,num_sets);
	    				wb_address=extend_to_32_bits(wb_address,address_size_without_offset );
	    				
	    			}
	    			cache[index][lru]=tag;
	    			dirty[index][lru]=0;
				if (policy==0){access_miss(num_sets,assoc,counters,index, lru);}
				else if (policy==1){update_plru(num_sets,assoc,index,plru_tree,lru);}
	    			
    			}
    		}
        
        }
        if (operation=='w') {
        	for (int i = 0; i < assoc; i++) {
        		
        	 	if (cache[index][i]==tag){
        	 		write_hits+=1;
        	 		if (policy==0){
					access_block(num_sets,assoc,counters,index, i);

				}
				else if (policy==1){
					update_plru(num_sets,assoc,index,plru_tree,i);
				}
        	 		dirty[index][i]=1;
        	 		found=1;
        	 		break;	
        	 	}
        	 	if (cache[index][i]==NULL){filled=0;}
        	        		
    		}
    		if (found==0){
    			
    			write_miss+=1;
    			if (vc_enable==1 & filled==1){
    				swap_requests++;
    				for (int i = 0; i < vc_num_blocks; i++) {
    					if (vc[i]==block_address){
    						vc_found=1;
    						no_swaps++;
    						
    					
    						int lru=find_lru_block(num_sets,assoc,counters,index);
    						unsigned int address_size_without_offset=address_size-(int)log2(block_size);
    						int block_addr=calculate_block_address(cache[index][lru],index,address_size_without_offset,num_sets);
    						
    						vc[i]=block_addr;
    						TagIndex address=extract_tag_index(block_address,block_size,num_sets);
    						cache[index][lru]=address.tag;
    						
    						
    						vc_dirty[i]=dirty[index][lru];
    						dirty[index][lru]=1;
    						
    						vc_access(vc_num_blocks,vc_counter,i);
    						access_block(num_sets,assoc,counters,index,lru);
    						break;
    						
    					}
    				}
    				if (vc_found==0){
    					int lru=find_lru_block(num_sets,assoc,counters,index);
    					
    					int vc_lru=find_vc_lru(vc_num_blocks,vc_counter);
    					unsigned int address_size_without_offset=address_size-(int)log2(block_size);
    					
	    				if (vc_dirty[vc_lru]==1) {
	    					wb+=1;
	    					wb_address=extend_to_32_bits(vc[vc_lru],address_size_without_offset);
	    					
	    				}
	    				vc[vc_lru]=calculate_block_address(cache[index][lru],index,address_size_without_offset,num_sets);
	    				cache[index][lru]=tag;
	    				vc_dirty[vc_lru]=dirty[index][lru];
	    				dirty[index][lru]=1;
	    				access_miss(num_sets,assoc,counters,index, lru);
	    				vc_miss(vc_num_blocks,vc_counter,vc_lru);
    					
    				}
    				
    			
    				
    				
    			}
    			else {
    				int lru=1;
    		
	    			
	    			if (policy==0){ lru=find_lru_block(num_sets,assoc,counters,index);}
	    			else if (policy==1){ lru=find_plru_block(assoc,num_sets,plru_tree,index);}
	    			else if (policy==2){
	    				
	    				int random_number = rand() % (assoc);
	    				lru=random_number;
	    			}
	    			else if (policy==3){
	    				lru=*x;
	    				*x+=1;
	    				*x=*x % assoc;
	    			}
	    			if (dirty[index][lru]==1) {
	    				wb+=1;
	    				unsigned int address_size_without_offset=address_size-(int)log2(block_size);
	    				wb_address=calculate_block_address(cache[index][lru],index,address_size_without_offset,num_sets);
	    				wb_address=extend_to_32_bits(wb_address,address_size_without_offset );
	    				
	    			}
	    			cache[index][lru]=tag;
	    			dirty[index][lru]=1;
	    			if (policy==0){access_miss(num_sets,assoc,counters,index, lru);}
				else if (policy==1){update_plru(num_sets,assoc,index,plru_tree,lru);}
    			}
    		}
        
        }
        	
        
 
       
    }
    


    Cache_block result;
    result.read_hits=read_hits;
    result.read_miss=read_miss;
    result.write_miss=write_miss;
    result.write_hits=write_hits;
    result.swap_requests=swap_requests;
    result.no_swaps=no_swaps;
    result.wb=wb;
    result.wb_address=wb_address;
    
    return result;
}
