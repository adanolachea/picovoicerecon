#include "stdio.h"
#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "string.h"
#include "inttypes.h"
#include "pico_audio_recorder.pio.h"
#include "pico_audio_recorder.h"
#include "tusb.h"
#include "hardware/clocks.h"
#include "hardware/rtc.h"
#include "spi_sdmmc.h"

static int32_t buff1[PIO_DMA_BUFFER_SIZE/4];
static int32_t buff2[PIO_DMA_BUFFER_SIZE/4];
static int dma_read_buff1_channel;
static int dma_read_buff2_channel;
static int32_t *fw_ptr = buff1;
static bool new_read_data=false;
static uint8_t recorder_state=STATE_STOP;

static uint16_t channels = INMP441_CHANNEL_MONO;
static uint16_t bitPerSample = INMP441_BIT_PER_SAMPLE_16;
static uint32_t sampleRate = INMP441_SAMPLE_RATE_44_1K;


void start_stop_button_cb(uint gpio, uint32_t event_mask) {

  if (gpio == BUTTON_PIN) {
    if (event_mask == GPIO_IRQ_EDGE_RISE) { 
        gpio_acknowledge_irq(BUTTON_PIN, GPIO_IRQ_EDGE_RISE);
        switch (get_recorder_state()) {
          case STATE_STOP:
            set_recorder_state(STATE_START_RECORDING);
          break;
          case STATE_RECORDING:
            set_recorder_state(STATE_STOPING);
          break;
          case STATE_START_RECORDING:
          break;
          case STATE_STOPING:
          break;

        }
    }
  
    
  }
}

void pio_irq_read_data() {
     if (pio_interrupt_get(INMP441_pio, 0)) {
        pio_interrupt_clear(INMP441_pio, 0);
        printf("irq 0:\n");
        //dma_channel_start(dma_read_buff1_channel);
     }
     if (pio_interrupt_get(INMP441_pio, 1)) {
        pio_interrupt_clear(INMP441_pio, 1);
        printf("irq:1\n");
     }
}


void dma_handler() {
   if (dma_channel_get_irq1_status(dma_read_buff1_channel)) {
      dma_channel_acknowledge_irq1(dma_read_buff1_channel);
      dma_channel_set_write_addr(dma_read_buff1_channel, buff1, false);
      fw_ptr = buff1;
      new_read_data=true;
   }
   if (dma_channel_get_irq1_status(dma_read_buff2_channel)) {
      dma_channel_acknowledge_irq1(dma_read_buff2_channel);
      dma_channel_set_write_addr(dma_read_buff2_channel, buff2, false);
      fw_ptr = buff2;
      new_read_data=true;
   }
}

void set_pin_LED(bool green) {
   if (green) {
      gpio_put(GREEN_PIN, true);
      gpio_put(RED_PIN, false);
   } else {
      gpio_put(GREEN_PIN, false);
      gpio_put(RED_PIN, true);
   }
}

/*!
* \brief sdio memory card pio initialize
* \param pio: pio number
* \param sm state machine
* \param cmd_pin command pin
* \param clk_pin CLK pin
* \param data_pin_base data 0~4 pins(D0~D3)
* \param clk_int Integer part of the divisor
* \param clk_frac – Fractional part in 1/256ths
*/
void inmp441_pio_init(PIO pio,  uint sm, uint sd_pin, uint sideset_pin) { // sideset_pin ws_clk
  
    pio_gpio_init(pio, sd_pin);
    pio_gpio_init(pio, sideset_pin);
    pio_gpio_init(pio, sideset_pin+1);
    gpio_pull_down(sd_pin);
    //gpio_pull_up(sideset_pin);
    //gpio_pull_up(sideset_pin+1);
    
    //== INMP441 SM ==
    uint offset = pio_add_program(pio, &inmp441_program);
    pio_sm_config c = inmp441_program_get_default_config(offset);
    pio_sm_set_consecutive_pindirs(pio, sm, sd_pin, 1, false);
    pio_sm_set_consecutive_pindirs(pio, sm, sideset_pin, 2, true); // WS, SCK
    sm_config_set_in_pins(&c, sd_pin);
    sm_config_set_set_pins(&c, sideset_pin,2);
    sm_config_set_sideset_pins(&c, sideset_pin);
    
    sm_config_set_in_shift(&c, false, true, bitPerSample);// 16 or 32
   
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);
    float div = clock_get_hz(clk_sys)/(sampleRate*64*2); // 64 clocks, 2 PIO clocks
    sm_config_set_clkdiv(&c, div);
 
    pio_sm_init(pio, sm, offset, &c);

      // initial state machine but not run
    pio_sm_set_enabled(pio, sm, false);

    //** for test only
    uint pio_irq = pio_get_index(pio)? PIO1_IRQ_0:PIO0_IRQ_0;
    pio_set_irq0_source_enabled(pio, pis_interrupt0, true);
    //pio_set_irq0_source_enabled(pio, pis_interrupt1, true);
    irq_add_shared_handler(pio_irq, pio_irq_read_data, PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY);
    irq_set_enabled(pio_irq, true);
   
    uint pio_base = (pio==pio0)?PIO0_BASE:PIO1_BASE;

    //==== READ DMA ===
    dma_read_buff1_channel = dma_claim_unused_channel(true);
    dma_read_buff2_channel = dma_claim_unused_channel(true);

    dma_channel_config dc1 = dma_channel_get_default_config(dma_read_buff1_channel);
    channel_config_set_write_increment(&dc1, true);
    channel_config_set_read_increment(&dc1, false);
    channel_config_set_dreq(&dc1, pio_get_dreq(pio, sm, false));
    channel_config_set_chain_to(&dc1, dma_read_buff2_channel);  
    //  for 32 bit frame
    if (bitPerSample == INMP441_BIT_PER_SAMPLE_32) {
         channel_config_set_transfer_data_size(&dc1, DMA_SIZE_32); //DMA_SIZE_8,16,32
         dma_channel_configure(dma_read_buff1_channel,
                  &dc1, buff1, (void*) (pio_base+PIO_RXF0_OFFSET),  // 
                  PIO_DMA_BUFFER_SIZE>> DMA_SIZE_32, false); //DMA_SIZE_8 or 16 or 32
    }
  // for 16 bit frame
  if (bitPerSample == INMP441_BIT_PER_SAMPLE_16) {
         channel_config_set_transfer_data_size(&dc1, DMA_SIZE_16); //DMA_SIZE_8,16,32
         dma_channel_configure(dma_read_buff1_channel,
               &dc1, buff1, (void*) (pio_base+PIO_RXF0_OFFSET),  // 
               PIO_DMA_BUFFER_SIZE>> DMA_SIZE_16, false); //DMA_SIZE_8 or 16 or 32
  }

   dma_channel_config dc2 = dma_channel_get_default_config(dma_read_buff2_channel);
    channel_config_set_write_increment(&dc2, true);
    channel_config_set_read_increment(&dc2, false);
    //channel_config_set_bswap(&dc2, true);
    channel_config_set_dreq(&dc2, pio_get_dreq(pio, sm, false));
    channel_config_set_chain_to(&dc2, dma_read_buff1_channel); 

   // for 32 bit frame
   if (bitPerSample == INMP441_BIT_PER_SAMPLE_32) {
         channel_config_set_transfer_data_size(&dc2, DMA_SIZE_32); //DMA_SIZE_8,16,32
         dma_channel_configure(dma_read_buff2_channel,
                  &dc2, buff2, (void*) (pio_base+PIO_RXF0_OFFSET),  // SDIO_DATA_READ_SM = 1
                  PIO_DMA_BUFFER_SIZE>> DMA_SIZE_32, false); //DMA_SIZE_8 or 16 or 32
   }
   // for 16 bit frame
   if (bitPerSample == INMP441_BIT_PER_SAMPLE_16) {
         channel_config_set_transfer_data_size(&dc2, DMA_SIZE_16); //DMA_SIZE_8,16,32
         dma_channel_configure(dma_read_buff2_channel,
                  &dc2, buff2, (void*) (pio_base+PIO_RXF0_OFFSET),  // SDIO_DATA_READ_SM = 1
                  PIO_DMA_BUFFER_SIZE>> DMA_SIZE_16, false); //DMA_SIZE_8 or 16 or 32
   }

   dma_channel_set_irq1_enabled(dma_read_buff1_channel, true);
   dma_channel_set_irq1_enabled(dma_read_buff2_channel, true);

    // Configure the processor to run dma_handler() when DMA IRQ 0 is asserted
    //irq_set_exclusive_handler(DMA_IRQ_1, dma_handler);
    irq_add_shared_handler(DMA_IRQ_1, dma_handler, PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY);
    irq_set_enabled(DMA_IRQ_1, true);
   
   gpio_init(BUTTON_PIN);
   gpio_set_dir(BUTTON_PIN, false);
   gpio_pull_down(BUTTON_PIN);
   gpio_init(RED_PIN);
   gpio_set_dir(RED_PIN, true);
   gpio_init(GREEN_PIN);
   gpio_set_dir(GREEN_PIN, true);
   recorder_state = STATE_STOP;
   set_pin_LED(true);

   gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, start_stop_button_cb);
}

bool write_file_wave_header(FIL *fil) {
   UINT bw;
	WAVE_HEADER wave_header;
	wave_header.riff[0] = 'R';wave_header.riff[1] = 'I';
	wave_header.riff[2] = 'F';wave_header.riff[3] = 'F';
	wave_header.size = (uint32_t)0;
	wave_header.wave[0] = 'W';wave_header.wave[1] = 'A';
	wave_header.wave[2] = 'V';wave_header.wave[3] = 'E';
	wave_header.fmt[0] = 'f';wave_header.fmt[1] = 'm';
	wave_header.fmt[2] = 't';wave_header.fmt[3] = ' ';
	wave_header.fmt_size = 16;
	wave_header.format = 1; // PCM
	wave_header.channels = channels; // channels
	wave_header.sampleRate=sampleRate;  // sample rate
	wave_header.rbc = sampleRate*bitPerSample*channels/8;
	wave_header.bc =  bitPerSample*channels/8;
	wave_header.bitsPerSample = bitPerSample; //bitsPerSample
	wave_header.data[0] = 'd'; wave_header.data[1] = 'a';
	wave_header.data[2] = 't'; wave_header.data[3] = 'a';
	wave_header.data_size = 0;
	if (f_write(fil, (uint8_t*)&wave_header, sizeof(wave_header), &bw)!= FR_OK) return false;
   return true;
}
bool modify_file_wave_header(FIL *fil, uint32_t data_length) {
   uint bw;
	uint32_t total_len = data_length+36;
	f_lseek(fil, 4);
	f_write(fil, (uint8_t*)&total_len, 4, &bw);
	f_lseek(fil, 40);
	f_write(fil, (uint8_t*)&data_length, 4, &bw);

   return true;
}

void inmp441_starting_recording_to_file_wav() { 
   uint32_t wav;
   uint bw;
   FIL fil;
   uint8_t filename[100];
   datetime_t t;
   FRESULT res;
   uint32_t data_length;
   FATFS fs;
   
   rtc_get_datetime(&t);
   tud_disconnect();
   sleep_ms(1);
   if (f_mount(&fs, SDMMC_PATH,1) != FR_OK) {
      tud_connect();
      sleep_ms(1);
      return;
   }
   
   sprintf(filename, "%s/V%02d%02d%02d-%02d%02d%02d.wav", SDMMC_PATH,t.year%100,t.month,t.day,t.hour,t.min,t.sec);
   if (f_open(&fil, filename, FA_CREATE_ALWAYS|FA_WRITE) != FR_OK) {
      printf("open file error:%s\n", filename);
      tud_connect();
      recorder_state = STATE_STOP;
      set_pin_LED(true);
      return;
   }

   if (!write_file_wave_header(&fil)) {
      tud_connect();
      recorder_state = STATE_STOP;
      set_pin_LED(true);
      return;
   }

   recorder_state = STATE_RECORDING;
   set_pin_LED(false);
   
   pio_sm_exec(INMP441_pio, INMP441_SM, pio_encode_mov(pio_isr, pio_null)); // enpty ISR
   pio_sm_clear_fifos(INMP441_pio, INMP441_SM);
   pio_sm_exec(INMP441_pio, INMP441_SM, pio_encode_jmp(inmp441_offset_inmp441_start));
   //dma_channel_start(dma_read_buff1_channel);
   dma_channel_set_write_addr(dma_read_buff2_channel, buff2, false);
   dma_channel_set_write_addr(dma_read_buff1_channel, buff1, true);
   pio_sm_set_enabled(INMP441_pio, INMP441_SM, true);

   new_read_data=false;
   
   sleep_ms(300);
   data_length=0;
   while (recorder_state == STATE_RECORDING) { 
      if (new_read_data) {
         new_read_data=false;
         f_write(&fil, (uint8_t*)fw_ptr, PIO_DMA_BUFFER_SIZE, &bw);
         data_length += PIO_DMA_BUFFER_SIZE;
      }
        
   } 
   modify_file_wave_header(&fil, data_length);
   f_close(&fil);
   f_unmount(SDMMC_PATH);

   pio_sm_set_enabled(INMP441_pio, INMP441_SM, false);
      
   dma_channel_abort(dma_read_buff1_channel);
   dma_channel_abort(dma_read_buff2_channel);
   
   
   recorder_state = STATE_STOP;
   set_pin_LED(true);
printf("Stop recording\n");
   tud_connect();
}

uint8_t get_recorder_state() {
   return recorder_state;
}
void    set_recorder_state(uint8_t state) {
   recorder_state = state;
}
