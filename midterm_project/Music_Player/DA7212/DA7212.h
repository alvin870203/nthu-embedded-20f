#ifndef _DA7212_H
#define _DA7212_H

#define kAudioSampleFrequency 16000
#define kAudioTxBufferSize 1000

#include "fsl_clock_config.h"
#include "fsl_common.h"
#include "fsl_dmamux.h"
#include "fsl_edma.h"
#include "fsl_gpio.h"
#include "fsl_i2c.h"
#include "fsl_lmem_cache.h"
#include "fsl_port.h"
#include "fsl_sai.h"
#include "fsl_sai_edma.h"
#include "mbed.h"

class DA7212
{
  private:
    // DA7212 configuration
    static constexpr uint16_t overSampleRate = 384;
    static constexpr uint8_t da7212I2cAddress = 0x1A;
    static constexpr uint8_t da7212ConfigurationSize = 48;
    static uint8_t da7212_register_config[da7212ConfigurationSize][2];

    // Buffer management
    static int16_t* capture_buffer;
    static uint16_t capture_buffer_size;
    static int16_t* tx_buffer;
    static uint16_t tx_buffer_size;
    static int16_t* rx_buffer;
    static uint16_t rx_buffer_size;

    // SAI and EDMA handler
    static sai_transfer_t da7212_sai_transfer;
    static sai_edma_handle_t da7212_tx_sai_handle;
    static sai_edma_handle_t da7212_rx_sai_handle;
    static edma_handle_t da7212_tx_dma_handle;
    static edma_handle_t da7212_rx_dma_handle;

    // DA7212 status
    static bool is_audio_initialized;
    static bool is_speaker_enable;
    static bool is_loopback;

    // Write DA7212 registers using I2C
    status_t Da7212WriteRegister(uint8_t register_address, uint8_t register_data);
    // Initalize audio interface
    int InitAudioInterface(void);

    static void CaptureSamples(int16_t *sample_data);

    // Callback function for SAI RX transfer (Microphone)
    static void SaiRxCallback(I2S_Type *base, sai_edma_handle_t *handle,
        status_t status, void *userData);

    // Callback function for SAI TX transfer (Speaker)
    static void SaiTxCallback(I2S_Type *base, sai_edma_handle_t *handle,
        status_t status, void *userData);

  public:
    DA7212();

    struct speaker
    {
      void status(bool enable) { is_speaker_enable = enable; }
      bool status(void) { return is_speaker_enable; }
      int play(int16_t *input, const int16_t input_size);
      void play(void) {status(true);};
      void pause(void) {memset(tx_buffer, 0, kAudioTxBufferSize);}//{ status(false);}
    } spk;

    struct microphone
    {
      int prepare(void);
      int get(int16_t *output);
    } mic;

    static void loopback(bool enable);
};
#endif
