// Allocate 256 bytes as a cyclic buffer
// Received SPI data will be enqueued into this buffer through ISR
// We will dequeue the data and transmit it to serial monitor in the main loop
#define FULL 256
byte queue[FULL];
int head=0;
int tail=0;

// Global Constants
const char h1_sep[15] = "--------------";
const char asciimap[34][4] = {
  "NUL",
  "SOH",
  "STX",
  "ETX",
  "EOT", 
  "ENQ",
  "ACK",
  "BEL",
  "BS ",
  "TAB",
  "LF ",
  "VT ",
  "FF ",
  "CR ",
  "SO ",
  "SI ",
  "DLE",
  "DC1",
  "DC2",
  "DC3",
  "DC4",
  "NAK",
  "SYN",
  "ETB",
  "CAN",
  "EM ",
  "SUB",
  "ESC",
  "FS ",
  "GS ",
  "RS ",
  "US ",
  "SPC",
  "DEL"
};

volatile boolean process_it;

void setup (void) {
  // Serial for debugging
  Serial.begin(115200);

  // Act as slave, MISO should be OUTPUT
  pinMode(MISO, OUTPUT);

  // Turn on SPI in slave mode
  SPCR |= _BV(SPE);

  // Turn on interrupts
  SPCR |= _BV(SPIE);

  // CLK = 8 MHz 
  // (Master should transmit data 0.25x speed or less)
  //SPCR |= _BV(SPR1);
  SPSR |= _BV(SPI2X);

  process_it = false;
  Serial.println(h1_sep);
  Serial.println(" Echo Chamber");
  Serial.println(" Started!");
  Serial.println(h1_sep);
}


// SPI ISR
ISR (SPI_STC_vect) {
  process_it = false;
  byte c = SPDR;
  queue[tail] = c;
  tail = nextIndex(tail);
  process_it = true;
}

void loop (void) {
  // wait for flag set in ISR
  if (process_it) {
    // print header
    Serial.println(h1_sep);
    Serial.println(" ASCII\tChar");
    Serial.println(h1_sep);

    // print buffer
    while(head != tail) {
      int ascii_code = queue[head];

      // print row
      Serial.print(" ");
      Serial.print(ascii_code);
      Serial.print("\t");
      if (ascii_code <= 32) {
        Serial.println(asciimap[ascii_code]);
      } else if (ascii_code == 127) {
        Serial.println(asciimap[33]);
      } else {
        Serial.println((char) queue[head]);
      }

      queue[head] = 0;
      head = nextIndex(head);
    }

    process_it = false;
  }
}


// This will allow us to cycle through the 
// 256 bytes buffer
int nextIndex(int index) {
  if (index == FULL - 1) {
    return 0;
  } else {
    return index + 1;
  }
}
