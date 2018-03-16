/** this been gitted no branched
*   From Molloy Work
*   Holly Shit
*/
#include <stdio.h>
#include <stdlib.h>
#include <prussdrv.h>
#include <pruss_intc_mapping.h>

#define PRU_NUM   0

int main (int argc, int *argv[] ){
   if(argc!=2){
      printf("\nYou must specify the %% of duty  cycle you stupid piece of shit \n\n");
      printf("Like so: ./pwm 25\n\n\n");
      exit(EXIT_FAILURE);
 }
   printf("argc:  %d\n", argc);
   printf("argv[0]: \n", argv[0]);
   printf("argv[1]: %s\n", argv[1]);
   unsigned int percent = atoi (argv[1]);
   printf("%d\n", percent);

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
