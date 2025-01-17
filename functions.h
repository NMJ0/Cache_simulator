
#define address_size 32
#include <math.h>


typedef struct {
    unsigned int tag;
    unsigned int index;
    unsigned int block_offset;
    unsigned int block_address; 
} AddressFields;



typedef struct {
    unsigned int tag;
    unsigned int index;
} TagIndex;

int extend_to_32_bits(int block_address, int current_address_size) {
    
    
    if (current_address_size < 32) {

        return block_address << (32 - current_address_size);
    } else {

        return block_address;
    }
}

TagIndex extract_tag_index(unsigned int block_address,int block_size,int num_sets) {
    int index_bits = 0, tag_bits = 0;
    int block_address_size=address_size-(int)log2(block_size);

    index_bits = (int)log2(num_sets);
    tag_bits = block_address_size - index_bits;

    unsigned int index_mask = (1 << index_bits) - 1;
    unsigned int tag_mask = (1 << tag_bits) - 1;

    unsigned int index = block_address & index_mask;
    unsigned int tag = (block_address >> index_bits) & tag_mask;
    TagIndex fields = {tag, index};

    return fields;
}

unsigned int calculate_block_address(unsigned int tag, unsigned int index, unsigned int address_size_without_offset,int num_sets) {
    int index_bits = (int)log2(num_sets);
    int tag_bits = address_size_without_offset - index_bits;

    unsigned int block_address = (tag << index_bits) | index;

    return block_address;
}
AddressFields extract_fields(unsigned int address,int num_sets,int block_size) {
    int block_offset_bits = 0, index_bits = 0, tag_bits = 0;


    block_offset_bits = (int)log2(block_size);
    index_bits = (int)log2(num_sets);
    tag_bits = address_size - block_offset_bits - index_bits;


    unsigned int block_offset_mask = (1 << block_offset_bits) - 1;
    unsigned int index_mask = (1 << index_bits) - 1;
    unsigned int tag_mask = (1 << tag_bits) - 1;


    unsigned int block_offset = address & block_offset_mask;
    unsigned int index = (address >> block_offset_bits) & index_mask;
    unsigned int tag = (address >> (block_offset_bits + index_bits)) & tag_mask;

 
    unsigned int block_address = address >> block_offset_bits;

 
    AddressFields fields = {tag, index, block_offset, block_address};

    return fields;
}
void initialize_plru( int assoc, int plru_tree[assoc-1]) {
    for (int i = 0; i < assoc - 1; i++) {
        plru_tree[i] = 0;  // Initialize all nodes to 0
    }
}
void initialize_cache(int assoc,int num_sets,int vc_num_blocks,int block_size,int cache[num_sets][assoc],int counters[num_sets][assoc],int dirty[num_sets][assoc],int vc[vc_num_blocks], int vc_counter[vc_num_blocks],int vc_dirty[vc_num_blocks],int policy,int plru_tree[num_sets][assoc-1]) {
    
    for (int i = 0; i < num_sets; i++) {
        for (int j = 0; j < assoc; j++) {
            cache[i][j] = NULL; 
            if (policy==0){
                counters[i][j] = 0;
            }
            dirty[i][j]=0;
        }    
            
        if (policy==1){
            for (int j = 0; j < assoc-1; j++) {
                plru_tree[i][j]=0;
            }

            //initialize_plru(assoc,plru_tree);
        }
    }
    
    for (int i = 0; i < vc_num_blocks; i++) {
    	vc[i]=0;
    	vc_counter[i]=0;
    	vc_dirty[i]=0;
    }
}

int find_plru_block( int assoc,int num_sets,int plru_tree[num_sets][assoc-1],int index) {
    int node_index = 0;
    int num_level=(int)log2(assoc);

    
    for (int level = 0; level < num_level; level++) {
        int direction = plru_tree[index][node_index];
        node_index = 2 * node_index + 1 + direction; 
    }

    
    int block_index = node_index - (assoc - 1);
    return block_index;
}
void update_plru( int num_sets,int assoc,int index,int plru_tree[num_sets][assoc-1],int accessed_block) {
    int node_index = 0;
    int num_level=(int)log2(assoc);
    int a=assoc;
    node_index=0;


    for (int level = 0; level < num_level; level++) {
        int direction;
        
        
        if (accessed_block >=a/2){
            direction=0;
            plru_tree[index][node_index] = direction;
            node_index=node_index*2+2;
            


        }
        else{
            direction=1;
            plru_tree[index][node_index] = direction;
            node_index=node_index*2+1;


        }
        accessed_block=accessed_block % (a/2);
        a=a/2;


        
        
    }
}

void print_counters(int assoc,int num_sets,int counters[num_sets][assoc]) {
    
    for (int i = 0; i < num_sets; i++) {
        printf("Set %d: ", i);
        for (int j = 0; j < assoc; j++) {
            printf("%d ", counters[i][j]);
        }
        printf("\n");
    }
}
void print_vc(int vc_num_blocks,int vc[vc_num_blocks],int vc_dirty[vc_num_blocks]) {
    printf("===== VC contents ===== \n");
    printf("set 0:");
    for (int i = 0; i < vc_num_blocks; i++) {
        printf("%X  ", vc[i]);
        if (vc_dirty[i]==1){printf("D ");}
        
     
    }
    printf("\n");
}
void access_block(int num_sets,int assoc,int counters[num_sets][assoc], int index, int accessed_block) {
    for (int i = 0; i < assoc; i++) {
        if (i != accessed_block & counters[index][i]<counters[index][accessed_block] ) {
            counters[index][i]++;  
        }
    }
    counters[index][accessed_block] = 0;  
}

void access_miss(int num_sets,int assoc,int counters[num_sets][assoc], int index, int accessed_block) {
    for (int i = 0; i < assoc; i++) {
        if (i != accessed_block) {
            counters[index][i]++;  
        }
    }
    counters[index][accessed_block] = 0;  
}

void vc_access(int vc_num_blocks,int vc_counter[vc_num_blocks], int accessed_block) {
    for (int i = 0; i < vc_num_blocks; i++) {
        if (i != accessed_block & vc_counter[i]<vc_counter[accessed_block]) {
            vc_counter[i]++;  
        }
    }
    vc_counter[accessed_block] = 0; 
   
}
void vc_miss(int vc_num_blocks,int vc_counter[vc_num_blocks], int accessed_block) {
    for (int i = 0; i < vc_num_blocks; i++) {
        if (i != accessed_block) {
            vc_counter[i]++;  
        }
    }
    vc_counter[accessed_block] = 0; 
   
}
int find_lru_block(int num_sets,int assoc,int counters[num_sets][assoc], int index) {
    int max_counter = 0;
    int lru_block = 0;
    for (int i = 0; i < assoc; i++) {
        if (counters[index][i] > max_counter) {
            max_counter = counters[index][i];
            lru_block = i;
        }
    }
    return lru_block;
}

int find_vc_lru(int vc_num_blocks, int vc_counter[vc_num_blocks]) {
    int max_counter = 0;
    int lru_block = 0;
    for (int i = 0; i < vc_num_blocks; i++) {
        if (vc_counter[i] > max_counter) {
            max_counter = vc_counter[i];
            lru_block = i;
        }
    }
    return lru_block;
}
void print_cache(int num_sets,int assoc,int cache[num_sets][assoc],int dirty[num_sets][assoc]) {
    
    for (int i = 0; i < num_sets; i++) {
        printf("Set %d: ", i);
        for (int j = 0; j < assoc; j++) {
            printf("%X ", cache[i][j]);
            if (dirty[i][j]==1){
            	printf("D  ");
            }
        }
        
        printf("\n");
    }
}
