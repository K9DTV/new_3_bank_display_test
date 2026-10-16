/*
  new_3_bank_display
  ------------------
  Scrolls the character set across three PDSP1880 displays.

  Hardware (unchanged from the 2013 Three_bank_display_test):
    OSEPP / Arduino Pro Mini, ATmega328, 5 V, 16 MHz
    Three PDSP1880 8-digit alphanumeric displays (24 digits total)
    10-turn pot wiper on A0 for scroll delay

  Pin map (Pro Mini):
    D0  PD0  Display data bit 0
    D1  PD1  Display data bit 1
    D2  PD2  Display data bit 2
    D3  PD3  Display data bit 3
    D4  PD4  Display data bit 4
    D5  PD5  Display data bit 5
    D6  PD6  Display data bit 6
    D7  PD7  Display WR (active low)
    D8  PB0  Digit address A0
    D9  PB1  Digit address A1
    D10 PB2  Digit address A2
    D11 PB3  Bank select 1 (left,  active low)
    D12 PB4  Bank select 2 (mid,   active low)
    D13 PB5  Bank select 3 (right, active low)
    A0  PC0  Scroll-speed pot wiper
    VIN +5 V
    GND Ground

  Note: D0/D1 are data lines, so hardware Serial is not available.

  Original sketch: Jim Burney, 07/08/2013
  This rewrite keeps the same bus timing and scroll behavior, with
  clearer structure and a fixed scroll-delay range (20..500 ms).
*/

#include <Arduino.h>
#include <avr/io.h>

// Geometry
static const uint8_t kDigitsPerBank = 8;
static const uint8_t kBankCount     = 3;
static const uint8_t kTotalDigits   = kDigitsPerBank * kBankCount; // 24
static const uint8_t kLastDigitIdx  = kTotalDigits - 1;            // 23 (was TotalDig)

// Character stream: codes 0x00 .. 0x7E (127 glyphs). Wrap threshold matches
// original TotalDisChar = 0x7F.
static const uint8_t kCharsetCount = 0x7F;

// PORTD: D0..D6 = character, D7 = WR (1 = idle, 0 = write)
static const uint8_t kWrIdle   = 0x80;
static const uint8_t kWrActive = 0x00;
static const uint8_t kBlankChar = 0x20; // ASCII space

// PORTB bank masks (same values as the original sketch)
static const uint8_t kBankLeft  = 0xF0; // PB3 low,  PB4/PB5 high
static const uint8_t kBankMid   = 0xE8; // PB4 low,  PB3/PB5 high
static const uint8_t kBankRight = 0xD8; // PB5 low,  PB3/PB4 high

// Scroll pot
static const uint8_t kScrollPotPin = A0;
static const int kScrollMsMin = 20;
static const int kScrollMsMax = 500; // original used byte(500) which truncated to 244

// Stream index (was Char2Count)
static uint8_t gScrollCount = 0;

static uint8_t bankMaskForPosition(uint8_t pos)
{
  // Same selection order as the original cascading ifs.
  if (pos <= 7) {
    return kBankLeft;
  }
  if (pos <= 15) {
    return kBankMid;
  }
  return kBankRight;
}

static uint8_t digitAddress(uint8_t pos)
{
  return (uint8_t)(pos & 0x07);
}

static uint8_t mappedCharacter(uint8_t count)
{
  if (count >= kCharsetCount) {
    return (uint8_t)(count - kCharsetCount);
  }
  return count;
}

static void writeDigit(uint8_t pos, uint8_t character)
{
  const uint8_t data = (uint8_t)(character & 0x7F);

  // 1) Present data with WR high (idle)
  PORTD = (uint8_t)(kWrIdle | data);
  // 2) Select bank + digit address
  PORTB = (uint8_t)(bankMaskForPosition(pos) | digitAddress(pos));
  // 3) Drop WR to latch
  PORTD = (uint8_t)(kWrActive | data);
}

static int readScrollDelayMs(void)
{
  return map(analogRead(kScrollPotPin), 0, 1023, kScrollMsMin, kScrollMsMax);
}

static void advanceScrollCount(void)
{
  // Original wrap: reset after TotalDisChar + TotalDig - 1
  const uint8_t wrapAt = (uint8_t)(kCharsetCount + kLastDigitIdx - 1);
  if (gScrollCount < wrapAt) {
    gScrollCount++;
  } else {
    gScrollCount = 0;
  }
}

void setup(void)
{
  // Entire Port D and Port B are outputs (data, WR, address, bank selects).
  DDRD = 0xFF;
  DDRB = 0xFF;

  // Blank bus, WR idle, so no garbage on first latch.
  PORTD = (uint8_t)(kWrIdle | kBlankChar);
}

void loop(void)
{
  // After writing 24 digits the stream advanced by 24. Subtract 23 so each
  // frame starts one character further along (same correction as original).
  if (gScrollCount >= kLastDigitIdx) {
    gScrollCount = (uint8_t)(gScrollCount - kLastDigitIdx);
  }

  for (uint8_t pos = 0; pos < kTotalDigits; pos++) {
    writeDigit(pos, mappedCharacter(gScrollCount));
    advanceScrollCount();
  }

  delay(readScrollDelayMs());
}
