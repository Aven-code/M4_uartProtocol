#line 1 "..\\SYSTEM\\SRC\\data.c"







 
#line 1 "..\\SYSTEM\\INC\\data.h"







 



#line 1 "..\\SYSTEM\\INC\\global.h"


 
 






 
typedef unsigned char u8 ;
typedef unsigned short u16 ;
typedef unsigned int  u32;
typedef  unsigned long u64;
 



#line 26 "..\\SYSTEM\\INC\\global.h"




#line 13 "..\\SYSTEM\\INC\\data.h"






 
enum mix_sequence {h1=0,h2,h3,h4,h5,h6,
l1,l2,l3,l4,l5,l6,
digl,digr,bluel,bluer};






 
typedef struct 
{
    u8 agin;      
    u16 valueQ;     
    u16 Center_fre;
 
}eq_stru;




 
typedef struct 
{
   u8 type;
   u16 freque;
   u8 Order;
}Hlpass_Stru;




 
typedef struct 
{
    Hlpass_Stru hign;       
    Hlpass_Stru low;        
    u8 passMute;
     
    u8 Phase;   
    u8 value;   
}Pass_Stru;





 
typedef struct 
{
    u16 ms;   
    u16 cm;     
    u16 inch; 

    Pass_Stru passA;  
    u8 mix_all[16];  
    eq_stru eq[31]; 
}Ch_Stru;





 
typedef struct 
{
    u8 volume;       
    u8 Mute;        
    Ch_Stru CH[12]; 
}Stru_Data;








 
void StruInit(Stru_Data * all);






 
void printData(const Stru_Data * all);

 


#line 10 "..\\SYSTEM\\SRC\\data.c"
#line 1 "H:\\mdk5.14\\ARM\\ARMCC\\Bin\\..\\include\\stdio.h"
 
 
 





 






 







 




  
 








#line 47 "H:\\mdk5.14\\ARM\\ARMCC\\Bin\\..\\include\\stdio.h"


  



    typedef unsigned int size_t;    









 
 

 



    typedef struct __va_list __va_list;






   




 




typedef struct __fpos_t_struct {
    unsigned __int64 __pos;
    



 
    struct {
        unsigned int __state1, __state2;
    } __mbstate;
} fpos_t;
   


 


   

 

typedef struct __FILE FILE;
   






 

#line 136 "H:\\mdk5.14\\ARM\\ARMCC\\Bin\\..\\include\\stdio.h"


extern FILE __stdin, __stdout, __stderr;
extern FILE *__aeabi_stdin, *__aeabi_stdout, *__aeabi_stderr;

#line 166 "H:\\mdk5.14\\ARM\\ARMCC\\Bin\\..\\include\\stdio.h"
    

    

    





     



   


 


   


 

   



 

   


 




   


 





    


 






extern __declspec(__nothrow) int remove(const char *  ) __attribute__((__nonnull__(1)));
   





 
extern __declspec(__nothrow) int rename(const char *  , const char *  ) __attribute__((__nonnull__(1,2)));
   








 
extern __declspec(__nothrow) FILE *tmpfile(void);
   




 
extern __declspec(__nothrow) char *tmpnam(char *  );
   











 

extern __declspec(__nothrow) int fclose(FILE *  ) __attribute__((__nonnull__(1)));
   







 
extern __declspec(__nothrow) int fflush(FILE *  );
   







 
extern __declspec(__nothrow) FILE *fopen(const char * __restrict  ,
                           const char * __restrict  ) __attribute__((__nonnull__(1,2)));
   








































 
extern __declspec(__nothrow) FILE *freopen(const char * __restrict  ,
                    const char * __restrict  ,
                    FILE * __restrict  ) __attribute__((__nonnull__(2,3)));
   








 
extern __declspec(__nothrow) void setbuf(FILE * __restrict  ,
                    char * __restrict  ) __attribute__((__nonnull__(1)));
   




 
extern __declspec(__nothrow) int setvbuf(FILE * __restrict  ,
                   char * __restrict  ,
                   int  , size_t  ) __attribute__((__nonnull__(1)));
   















 
#pragma __printf_args
extern __declspec(__nothrow) int fprintf(FILE * __restrict  ,
                    const char * __restrict  , ...) __attribute__((__nonnull__(1,2)));
   


















 
#pragma __printf_args
extern __declspec(__nothrow) int _fprintf(FILE * __restrict  ,
                     const char * __restrict  , ...) __attribute__((__nonnull__(1,2)));
   



 
#pragma __printf_args
extern __declspec(__nothrow) int printf(const char * __restrict  , ...) __attribute__((__nonnull__(1)));
   




 
#pragma __printf_args
extern __declspec(__nothrow) int _printf(const char * __restrict  , ...) __attribute__((__nonnull__(1)));
   



 
#pragma __printf_args
extern __declspec(__nothrow) int sprintf(char * __restrict  , const char * __restrict  , ...) __attribute__((__nonnull__(1,2)));
   






 
#pragma __printf_args
extern __declspec(__nothrow) int _sprintf(char * __restrict  , const char * __restrict  , ...) __attribute__((__nonnull__(1,2)));
   



 

#pragma __printf_args
extern __declspec(__nothrow) int snprintf(char * __restrict  , size_t  ,
                     const char * __restrict  , ...) __attribute__((__nonnull__(3)));
   















 

#pragma __printf_args
extern __declspec(__nothrow) int _snprintf(char * __restrict  , size_t  ,
                      const char * __restrict  , ...) __attribute__((__nonnull__(3)));
   



 
#pragma __scanf_args
extern __declspec(__nothrow) int fscanf(FILE * __restrict  ,
                    const char * __restrict  , ...) __attribute__((__nonnull__(1,2)));
   






























 
#pragma __scanf_args
extern __declspec(__nothrow) int _fscanf(FILE * __restrict  ,
                     const char * __restrict  , ...) __attribute__((__nonnull__(1,2)));
   



 
#pragma __scanf_args
extern __declspec(__nothrow) int scanf(const char * __restrict  , ...) __attribute__((__nonnull__(1)));
   






 
#pragma __scanf_args
extern __declspec(__nothrow) int _scanf(const char * __restrict  , ...) __attribute__((__nonnull__(1)));
   



 
#pragma __scanf_args
extern __declspec(__nothrow) int sscanf(const char * __restrict  ,
                    const char * __restrict  , ...) __attribute__((__nonnull__(1,2)));
   








 
#pragma __scanf_args
extern __declspec(__nothrow) int _sscanf(const char * __restrict  ,
                     const char * __restrict  , ...) __attribute__((__nonnull__(1,2)));
   



 

 
extern __declspec(__nothrow) int vfscanf(FILE * __restrict  , const char * __restrict  , __va_list) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) int vscanf(const char * __restrict  , __va_list) __attribute__((__nonnull__(1)));
extern __declspec(__nothrow) int vsscanf(const char * __restrict  , const char * __restrict  , __va_list) __attribute__((__nonnull__(1,2)));

extern __declspec(__nothrow) int _vfscanf(FILE * __restrict  , const char * __restrict  , __va_list) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) int _vscanf(const char * __restrict  , __va_list) __attribute__((__nonnull__(1)));
extern __declspec(__nothrow) int _vsscanf(const char * __restrict  , const char * __restrict  , __va_list) __attribute__((__nonnull__(1,2)));

extern __declspec(__nothrow) int vprintf(const char * __restrict  , __va_list  ) __attribute__((__nonnull__(1)));
   





 
extern __declspec(__nothrow) int _vprintf(const char * __restrict  , __va_list  ) __attribute__((__nonnull__(1)));
   



 
extern __declspec(__nothrow) int vfprintf(FILE * __restrict  ,
                    const char * __restrict  , __va_list  ) __attribute__((__nonnull__(1,2)));
   






 
extern __declspec(__nothrow) int vsprintf(char * __restrict  ,
                     const char * __restrict  , __va_list  ) __attribute__((__nonnull__(1,2)));
   






 

extern __declspec(__nothrow) int vsnprintf(char * __restrict  , size_t  ,
                     const char * __restrict  , __va_list  ) __attribute__((__nonnull__(3)));
   







 

extern __declspec(__nothrow) int _vsprintf(char * __restrict  ,
                      const char * __restrict  , __va_list  ) __attribute__((__nonnull__(1,2)));
   



 
extern __declspec(__nothrow) int _vfprintf(FILE * __restrict  ,
                     const char * __restrict  , __va_list  ) __attribute__((__nonnull__(1,2)));
   



 
extern __declspec(__nothrow) int _vsnprintf(char * __restrict  , size_t  ,
                      const char * __restrict  , __va_list  ) __attribute__((__nonnull__(3)));
   



 
extern __declspec(__nothrow) int fgetc(FILE *  ) __attribute__((__nonnull__(1)));
   







 
extern __declspec(__nothrow) char *fgets(char * __restrict  , int  ,
                    FILE * __restrict  ) __attribute__((__nonnull__(1,3)));
   










 
extern __declspec(__nothrow) int fputc(int  , FILE *  ) __attribute__((__nonnull__(2)));
   







 
extern __declspec(__nothrow) int fputs(const char * __restrict  , FILE * __restrict  ) __attribute__((__nonnull__(1,2)));
   




 
extern __declspec(__nothrow) int getc(FILE *  ) __attribute__((__nonnull__(1)));
   







 




    extern __declspec(__nothrow) int (getchar)(void);

   





 
extern __declspec(__nothrow) char *gets(char *  ) __attribute__((__nonnull__(1)));
   









 
extern __declspec(__nothrow) int putc(int  , FILE *  ) __attribute__((__nonnull__(2)));
   





 




    extern __declspec(__nothrow) int (putchar)(int  );

   



 
extern __declspec(__nothrow) int puts(const char *  ) __attribute__((__nonnull__(1)));
   





 
extern __declspec(__nothrow) int ungetc(int  , FILE *  ) __attribute__((__nonnull__(2)));
   






















 

extern __declspec(__nothrow) size_t fread(void * __restrict  ,
                    size_t  , size_t  , FILE * __restrict  ) __attribute__((__nonnull__(1,4)));
   











 

extern __declspec(__nothrow) size_t __fread_bytes_avail(void * __restrict  ,
                    size_t  , FILE * __restrict  ) __attribute__((__nonnull__(1,3)));
   











 

extern __declspec(__nothrow) size_t fwrite(const void * __restrict  ,
                    size_t  , size_t  , FILE * __restrict  ) __attribute__((__nonnull__(1,4)));
   







 

extern __declspec(__nothrow) int fgetpos(FILE * __restrict  , fpos_t * __restrict  ) __attribute__((__nonnull__(1,2)));
   








 
extern __declspec(__nothrow) int fseek(FILE *  , long int  , int  ) __attribute__((__nonnull__(1)));
   














 
extern __declspec(__nothrow) int fsetpos(FILE * __restrict  , const fpos_t * __restrict  ) __attribute__((__nonnull__(1,2)));
   










 
extern __declspec(__nothrow) long int ftell(FILE *  ) __attribute__((__nonnull__(1)));
   











 
extern __declspec(__nothrow) void rewind(FILE *  ) __attribute__((__nonnull__(1)));
   





 

extern __declspec(__nothrow) void clearerr(FILE *  ) __attribute__((__nonnull__(1)));
   




 

extern __declspec(__nothrow) int feof(FILE *  ) __attribute__((__nonnull__(1)));
   


 
extern __declspec(__nothrow) int ferror(FILE *  ) __attribute__((__nonnull__(1)));
   


 
extern __declspec(__nothrow) void perror(const char *  );
   









 

extern __declspec(__nothrow) int _fisatty(FILE *   ) __attribute__((__nonnull__(1)));
    
 

extern __declspec(__nothrow) void __use_no_semihosting_swi(void);
extern __declspec(__nothrow) void __use_no_semihosting(void);
    





 











#line 985 "H:\\mdk5.14\\ARM\\ARMCC\\Bin\\..\\include\\stdio.h"



 

#line 11 "..\\SYSTEM\\SRC\\data.c"







 
void StruInit(Stru_Data * all)
{
    u8 i,j;
    all->volume = 0x35;     
    all->Mute = 0x01;
    
    for(i=0;i<12;i++)
    {
        
        all->CH[i].ms = 0;
        all->CH[i].cm = 0;
        all->CH[i].inch = 0;
        
        
        all->CH[i].passA.hign.type = 0x00;         
        all->CH[i].passA.hign.freque = 0x14;       
        all->CH[i].passA.hign.Order = 0x04;         
        
        
        all->CH[i].passA.low.type = 0x00;           
        all->CH[i].passA.low.freque = (0x01<<8)|0xf4;
        all->CH[i].passA.low.Order = 0x04;          
       
        all->CH[i].passA.passMute = 0x02;           
        all->CH[i].passA.Phase = 0x01;             
        all->CH[i].passA.value = 0x32;              

         
        for(j=0;j<31;j++)
        {
            all->CH[i].eq[j].agin = 0x00;           
            all->CH[i].eq[j].valueQ = 0x02;         
            all->CH[i].eq[j].Center_fre = 0x32;    
        }

         
        for(j=0;j<16;j++)
        {
             all->CH[i].mix_all[j] = 0X7F;  
        }
        
    
    }

}






 
static void printMix(const u8 * mix)
{
    printf("h1: %d\t",mix[h1]);
    printf("h2: %d\t",mix[h2]);
    printf("h3: %d\t",mix[h3]);
    printf("h4: %d\t",mix[h4]);
    printf("h5: %d\n",mix[h5]);
    printf("h6: %d\t",mix[h6]);
    printf("l1: %d\t",mix[l1]);
    printf("l2: %d\t",mix[l2]);
    printf("l3: %d\t",mix[l3]);
    printf("l4: %d\n",mix[l4]);
    printf("l5: %d\t",mix[l5]);
    printf("l6: %d\t",mix[l6]);

    printf("digl: %d\t",mix[digl]);
    printf("digr: %d\n",mix[digr]);
    printf("bluel: %d\t",mix[bluel]);
    printf("bluer: %d\t",mix[bluer]);
    
}
 




 
void printData(const Stru_Data * all)
{ 
    
    u8 i,j; 
      
    printf("all->volume: %d\n",  all->volume);   
    printf("all->mute:%d\n",  all->Mute);
    for(i=0;i<12;i++)
    {
        printf("\nNo.%d--------------------------------Start----------------------------------------------\n",i+1);
        printf("ms: 0x%02X\t", all->CH[i].ms );
        printf("cm: 0x%02X\t", all->CH[i].cm );
        printf("inch: 0x%02X\t", all->CH[i].inch); 

				printf("\n__________hign________\n");
        printf("all->CH[i].passA.hign.type: 0x%02X\t",all->CH[i].passA.hign.type);
        printf("all->CH[i].passA.hign.freque: 0x%04X\t",all->CH[i].passA.hign.freque);
        printf("all->CH[i].passA.hign.Order: 0x%02X",all->CH[i].passA.hign.Order);

        printf("\n__________low________\n");
        printf("all->CH[i].passA.low.type: 0x%02X\t",all->CH[i].passA.low.type);
        printf("all->CH[i].passA.low.freque: 0x%04X\t",all->CH[i].passA.low.freque);
        printf("all->CH[i].passA.low.Order: 0x%02X",all->CH[i].passA.low.Order);

        printf("\n__________other________\n");
        printf("all->CH[i].passA.passMute: %c\t",(all->CH[i].passA.passMute == 0x01)?'Y':'N');
        printf("all->CH[i].passA.Phase: 0x%02X\t", all->CH[i].passA.Phase);
        printf("all->CH[i].passA.value: 0x%02X", all->CH[i].passA.value);
 
     
        printf("\n__________________________EQ____________________________\n");
        
        for(j=0;j<31;j++)
        {
            printf("index: %d all->CH[i].eq[%d].agin: %d\t",j,j,all->CH[i].eq[j].agin);
            printf("all->CH[i].eq[%d].valueQ: %d\t",j,all->CH[i].eq[j].valueQ);
            printf("all->CH[i].eq[%d].Center_fre: %d\t",j,all->CH[i].eq[j].Center_fre);
        }
  
        printf("\n --Mix---------------------\n");
        printMix(all->CH[i].mix_all);
        
        printf("\nNo.%d--------------------------------End----------------------------------------------\n",i+1);

    }
    
       
 
  
}



