/** \file
 * LED Library for the BeagleBone (Black).
 *
 * Drives up to 32 ws281x LED strips using the PRU to have no CPU overhead.
 * Allows easy double buffering of frames.
 */

#ifndef _pixelbone_hpp
#define _pixelbone_hpp_

#include "pru.h"

/** LEDscape pixel format is BRGA.
 *
 * data is laid out with BRGA format, since that is how it will
 * be translated during the clock out from the PRU.
 */
typedef struct {
  uint8_t b;
  uint8_t r;
  uint8_t g;
  uint8_t a;
} __attribute__((__packed__)) pixel_t;

/** Command structure shared with the PRU.
 *
 * This is mapped into the PRU data RAM and points to the
 * frame buffer in the shared DDR segment.
 *
 * Changing this requires changes in ws281x.p
 */
typedef struct ws281x_command_t {
  // in the DDR shared with the PRU
  uintptr_t pixels_dma;

  // Length in pixels of the longest LED strip.
  unsigned num_pixels;

  // write 1 to start, 0xFF to abort. will be cleared when started
  volatile unsigned command;

  // will have a non-zero response written when done
  volatile unsigned response;
  ws281x_command_t(unsigned _num_pixels)
      : pixels_dma(0), num_pixels(_num_pixels), command(0), response(0) {};

} __attribute__((__packed__)) ws281x_command_t;

class PixelBone {
  pru_t *pru0;
  uint16_t num_pixels;
  ws281x_command_t *ws281x;
  size_t buffer_size;
  uint8_t current_buffer_num;

public:
  PixelBone(uint16_t pixel_count);
  ~PixelBone();
  void show(void);
  void setPixelColor(uint8_t n, uint8_t r, uint8_t g, uint8_t b);
  void setPixelColor(uint8_t n, pixel_t c);
  void moveToNextBuffer();
  uint32_t wait();
  uint16_t numPixels() const;
  pixel_t *getCurrentBuffer() const;
  pixel_t *getPixel(uint16_t n) const;
  static pixel_t Color(uint8_t r, uint8_t g, uint8_t b);
};

#endif