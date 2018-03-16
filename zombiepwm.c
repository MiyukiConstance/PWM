/** this been gitted no branched
*   From Molloy Work
*   Holly Shit
*/
#include <stdio.h>
#include <stdlib.h>
#include <prussdrv.h>
#include <pruss_intc_mapping.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>

#include  "shm-02.h"

#define PRU_NUM   0

int main (int argc, int *argv[] ){
   if(argc!=2){
      printf("\nYou must specify the %% of duty  cycle you stupid piece of shit \n\n");
      printf("Like so: ./pwm 25\n\n\n");
      exit(EXIT_FAILURE);
 }
// ca c'est comme safe.. apres le argv a modifier

   printf("argc:  %d\n", argc);
   printf("argv[0]: \n", argv[0]);
   printf("argv[1]: %s\n", argv[1]);
   unsigned int percent = atoi (argv[1]);
   printf("%d\n", percent);

// New Stuff
ShmKEY = ftok(".", 'x');
     ShmID = shmget(ShmKEY, sizeof(struct Memory), 0666);
     if (ShmID < 0) {
          printf("*** shmget error (client) ***\n");
          exit(1);
     }
     printf("   Client has received a shared memory of four integers...\n");
     
     ShmPTR = (struct Memory *) shmat(ShmID, NULL, 0);
     if ((int) ShmPTR == -1) {
          printf("*** shmat error (client) ***\n");
          exit(1);
     }
     printf("   Client has attached the shared memory...\n");
     
     while (ShmPTR->status != FILLED)
          ;
     printf("   Client found the data is ready...\n");
     printf("   Client found %d %d %d %d in shared memory...\n",
                ShmPTR->data[0], ShmPTR->data[1], 
                ShmPTR->data[2], ShmPTR->data[3]);

     ShmPTR->status = TAKEN;
     printf("   Client has informed server data have been taken...\n");
     shmdt((void *) ShmPTR);
     printf("   Client has detached its shared memory...\n");
     printf("   Client exits...\n");
// end of new stuff


   if(getuid()!=0){
      printf("You must run this program as root. Exiting.\n");
      exit(EXIT_FAILURE);
   }
   // Initialize structure used by prussdrv_pruintc_intc
   tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;

   // Allocate and initialize memory
   prussdrv_init ();
   prussdrv_open (PRU_EVTOUT_0);

   // Map PRU intrrupts
   prussdrv_pruintc_init(&pruss_intc_initdata);

   // Copy the PWM percentage and delay factor into PRU memory
   //unsigned int percent = 18 ;  //(0-100)
   prussdrv_pru_write_memory(PRUSS0_PRU0_DATARAM, 0, &percent, 4);
   unsigned int sampletimestep = 10;  //delay factor
   // write it into the next word location in memory (i.e. 4-bytes later)
   prussdrv_pru_write_memory(PRUSS0_PRU0_DATARAM, 1, &sampletimestep, 4);

   // Load and execute binary on PRU
   prussdrv_exec_program (PRU_NUM, "./pwm.bin");
exit(EXIT_FAILURE);

   // Wait for event completion from PRU
   int n = prussdrv_pru_wait_event (PRU_EVTOUT_0);
   printf("PRU program completed, event number %d.\n", n);
//exit(EXIT_FAILURE);
   // Disable PRU and close memory mappings
   prussdrv_pru_disable(PRU_NUM);
   prussdrv_exit ();
   return EXIT_SUCCESS;
//exit(EXIT_FAILURE);
}
