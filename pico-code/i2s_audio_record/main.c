

#include 
                        
#include 
                         
#include 
                          
#include "pico/stdlib.h"

#include "bsp/board.h"
#include "tusb.h"

#include "msc_storage_driver.h"

#include "pico_storage.h"
#include "hardware/rtc.h"
#include "pico_audio_recorder.h"


/*------------- MAIN -------------*/
int main(void)
{
  stdio_init_all();
  rtc_init();

  datetime_t t = {
    .year=2023, .month=10, .day=29, 
    .dotw=6,
    .hour=14, .min=05, .sec=50
  }; 
  
   if (!rtc_set_datetime(&t)) printf("set rtc error\n");
     
  storage_driver_init();
  
  board_init();
  // init device stack on configured roothub port
  tud_init(BOARD_TUD_RHPORT);

  inmp441_pio_init(INMP441_pio, INMP441_SM, INMP441_SD, INMP441_SCK);

  while (1)
  {
      if (get_recorder_state() == STATE_START_RECORDING) {
        printf("Start recording\n");
        inmp441_starting_recording_to_file_wav();
      }
    
             
      tud_task(); // tinyusb device task
      tight_loop_contents();
  }

  return 0;
}


                          
                         
                        