## Pico Voide Recon Project 
With the help of Tensorflow and Tensorflow Lite for Microcotrollers, this project will help materialize a voice command recognizer running on Raspberry Pi Pico RP2040
This is a microcontroller developed by Raspberry Pi company and is super low cost.

The project is splitted in 4 parts:
* Python inference code
  - This code has a tensorflow code to train and generate C++ files to deploy the model in Microcontroller
* Pico-Pi I2S voice recorder
  - The I2S voice recorder will help to bring up the I2S microphone driver and collect data to train the model 
* Pico-Pi OLED screen
  - The OLED Display will be used to show the user if a YES or NO command was detected
* Master Pico-Pi Voicerecon
  - This glues all the code:
    - I2S Microphone will record and feed a circular buffer
    - A preprocessing task will be processing this buffer and extracting features
    - Inference code from TFLM will get the preprocessing data and run inference with the model
    - Postprocessing code will show if a command is recognized in the OLED Display
   
This is the list of materials:
- RP2040 Microcontroller
- SD1306 I2C OLED Display
- INMP441 I2S Microphone
- microSD adapter (to store recordings to train the model)
