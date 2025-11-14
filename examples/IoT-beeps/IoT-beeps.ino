#include <anyrtttl.h>
#include <pitches.h>

/*
This example is a test project designed to evaluate and demonstrate a variety of feedback melodies for IoT and embedded devices.
The goal is to let you explore how short RTTTL melodies can effectively communicate different device states.
For example:
  * startup
  * ready
  * processing
  * error
  * notifications
  * shutdown
*/

// Define the BUZZER_PIN for current board
#if defined(ESP32)
#define BUZZER_PIN 25 // Using GPIO25 (pin labeled D25)
#elif defined(ESP8266)
#define BUZZER_PIN  2 // Using GPIO2  (pin labeled D2)
#else // base arduino models
#define BUZZER_PIN 9
#endif

// Generic RTTTL melodies
const char two_short[] PROGMEM = "two_short:d=4,o=5,b=100:16e6,32p,16e6";
const char three_short[] PROGMEM = "three_short:d=4,o=5,b=100:16e6,32p,16e6,32p,16e6";
const char three_short_burst[] PROGMEM = "three_short_burst:d=4,o=5,b=100:32c,32p,32c,32p,32c";
const char siren[] PROGMEM = "siren:d=8,o=5,b=100:e,d,e,d,e,d,e,d";
const char scale_up[] PROGMEM = "scale_up:d=32,o=5,b=100:c,c#,d#,e,f#,g#,a#,b";
const char scale_down[] PROGMEM = "scale_up:d=32,o=5,b=100:b,a#,g#,f#,e,d#,c#,c";
const char s_o_s[] PROGMEM = "sos:d=16,o=6,b=120:32c6,32p,32c6,32p,32c6,32p,8c6,32p,8c6,32p,8c6,32p,32c6,32p,32c6,32p,32c6,32p";
const char alert[] PROGMEM = "alert:d=16,o=5,b=180:g5,32p,g5,32p,g5,32p,g5,32p";

// Super mario based melodies
const char smw_1up[] PROGMEM = "smw_1up:d=16,o=7,b=200:e.6,g.6,e.,c.,d.,g.";
const char smw_coin[] PROGMEM = "smw_coin:d=32,o=4,b=112:p,b.5,8e6";
const char smw_game_over[] PROGMEM = "smw_game_over:d=4,o=4,b=355:8c.5,32p.,p,p,8g.,32p.,2p,e.";
const char smw_game_over_reversed[] PROGMEM = "smw_game_over_reversed:d=4,o=4,b=355:e.,2p,32p.,8g.,p,p,32p.,8c.5";
const char smw_life[] PROGMEM = "smw_life:d=8,o=4,b=450:e.5,32p.,g.5,32p.,e.6,32p.,c.6,32p.,d.6,32p.,g.6,32p.";
const char smw_life_reversed[] PROGMEM = "smw_life_reversed:d=8,o=4,b=450:g.6,32p.,32p.,d.6,32p.,c.6,32p.,e.6,32p.,g.5,32p.,4e.5";
const char smw_mushroom_powerup[] PROGMEM = "smw_mushroom_powerup:d=32,o=5,b=200:c,g4,c,e,g,c6,g,g#4,c,d#,g#,d#,g#,c6,d#6,g#6,d#6,d,f,a#,f,a#,d6,f6,d6,f6,a#6,f6";
const char smw_mushroom_powerup_reversed[] PROGMEM = "smw_mushroom_powerup_reversed:d=32,o=5,b=200:f6,a#6,f6,d6,f6,d6,a#,f,a#,f,d,d#6,g#6,d#6,c6,g#,d#,g#,d#,c,g#4,g,c6,g,e,c,g4,c";

// Ready melodies
const char ready01[] PROGMEM = "ready01:d=8,o=6,b=240:c,e,c7";
const char ready02[] PROGMEM = "ready02:d=2,o=5,b=900:a,32p,b,32p,c6,32p";

// Notification melodies
const char notification01[] PROGMEM = "notification01:d=16,o=6,b=180:e,g,a,b,4g";
const char notification02[] PROGMEM = "notification02:d=16,o=6,b=180:c6,e6,g6,e6,4c6";
const char notification03[] PROGMEM = "notification03:d=16,o=5,b=170:g5,b5,g6,b5,4g5";
const char notification04[] PROGMEM = "notification04:d=8,o=6,b=160:e6,g6,e7,4g6";
const char notification05[] PROGMEM = "notification05:d=16,o=6,b=160:e6,32p,e6,32p";

// Success melodies
const char bright_ping_cascade[] PROGMEM = "bright_ping_cascade:d=4,o=4,b=715:c.6,32p,e.6,32p,g.6,32p,c.7,32p";
const char success01[] PROGMEM = "success01:d=8,o=6,b=150:c,e,g,4c7";
const char success02[] PROGMEM = "success02:d=8,o=6,b=150:c,g,e7,4c7";
const char success03[] PROGMEM = "success03:d=16,o=6,b=180:c,d,e,g,4c7";
const char success04[] PROGMEM = "success04:d=8,o=5,b=160:g,b,d6,4g6";
const char success05[] PROGMEM = "success05:d=8,o=5,b=160:d,g,b,4d6";
const char success06[] PROGMEM = "success06:d=8,o=5,b=150:f,a,c6,4f6";
const char success07[] PROGMEM = "success07:d=16,o=6,b=190:d,f,g,a,8d7";
const char success08[] PROGMEM = "success08:d=8,o=6,b=240:c,e,g";
const char success09[] PROGMEM = "success09:d=8,o=6,b=250:c,c7";
const char success10[] PROGMEM = "success10:d=8,o=6,b=240:e,g,c7";
const char success11[] PROGMEM = "success11:d=8,o=6,b=240:g,b,c7";
const char success12[] PROGMEM = "success12:d=16,o=6,b=220:c,e,g,c7";
const char success13[] PROGMEM = "success13:d=16,o=6,b=200:e,g,a,b,c7";
const char success14[] PROGMEM = "success14:d=16,o=6,b=230:c,e,c,e,g,c7";
const char success15[] PROGMEM = "success15:d=16,o=6,b=160:c6,32p,c6,32p,c6,32p,4e6,32p";

// Failure melodies
const char failure[] PROGMEM = "failure:d=3,o=4,b=900:a.,32p,g.,32p,f.,32p";
const char low_buzz_drop[] PROGMEM = "low_buzz_drop:d=2,o=4,b=900:e,32p,d,32p,c,32p,c.,32p";
const char fail01[] PROGMEM = "fail01:d=8,o=5,b=240:f,d,c";
const char fail02[] PROGMEM = "fail02:d=16,o=5,b=220:g,f,d,c";
const char fail03[] PROGMEM = "fail03:d=16,o=5,b=230:g,f,e,d,c";
const char fail04[] PROGMEM = "fail04:d=32,o=4,b=355:c.5,p,c#.5,p,16d.5";

// Error melodies
const char error01[] PROGMEM = "error01:d=8,o=5,b=240:c,a,f";
const char error02[] PROGMEM = "error02:d=8,o=4,b=240:g,f,d#";
const char error03[] PROGMEM = "error03:d=8,o=5,b=140:g5,e5,c5";
const char error04[] PROGMEM = "error04:d=8,o=4,b=130:c#,c,b3";
const char error05[] PROGMEM = "error05:d=8,o=4,b=260:f,d#,d,c";
const char error06[] PROGMEM = "error06:d=8,o=4,b=225:a,g,f,4d#";

// Turn-off melodies
const char turnoff01[] PROGMEM = "turnoff01:d=16,o=5,b=140:c6,b5,a5,g5,f5,4c5";
const char turnoff02[] PROGMEM = "turnoff02:d=8,o=5,b=250:e6,d6,b5,g5,4d5";
const char turnoff03[] PROGMEM = "turnoff03:d=16,o=6,b=150:g6,e6,d6,c6,4a5";
const char turnoff04[] PROGMEM = "turnoff04:d=16,o=5,b=140:f6,e6,d6,c6,b5,4g5";
const char turnoff05[] PROGMEM = "turnoff05:d=16,o=5,b=150:c6,b5,a5,g5,f5,e5,4c5";

// Processing data melodies
const char processing1[] PROGMEM = "processing1:d=16,o=5,b=150:c,e,g,e,c";
const char processing2[] PROGMEM = "processing2:d=16,o=5,b=160:a,c6,a,f,a,c6,a";
const char processing3[] PROGMEM = "processing3:d=16,o=5,b=140:d,f,g,f,d,f,g,f";

// Processing start melodies
const char startProc1[] PROGMEM = "startProc1:d=16,o=5,b=170:c,e,g,c6";
const char startProc2[] PROGMEM = "startProc2:d=16,o=6,b=180:a5,c6,e6,4g6";
const char startProc3[] PROGMEM = "startProc3:d=16,o=5,b=160:f,a,c6,f6,4a6";

// Processing done melodies
const char doneProc1[] PROGMEM = "doneProc1:d=16,o=6,b=170:c6,e6,g6,c7,g6,e6,c6";
const char doneProc2[] PROGMEM = "doneProc2:d=8,o=5,b=160:g5,b5,d6,b5,g5";
const char doneProc3[] PROGMEM = "doneProc3:d=16,o=5,b=180:a5,c6,e6,a6,g6,e6,c6";
const char doneProc4[] PROGMEM = "doneProc4:d=16,o=5,b=150:c6,b5,a5,g5,a5,b5,c6";

// Non-palindromic reversible melodies
// (playing the reverse truly sounds different)
const char bootUp[] PROGMEM = "bootUp:d=16,o=5,b=170:c,e,g,c6,e6,g6,4c7";
const char powerDown[] PROGMEM = "powerDown:d=16,o=5,b=170:c7,g6,e6,c6,g,e,4c";
const char systemStart[] PROGMEM = "systemStart:d=16,o=5,b=160:d,f,g,a,c6,4d6";
const char systemStop[] PROGMEM = "systemStop:d=16,o=5,b=160:d6,c6,a,g,f,4d";
const char activate[] PROGMEM = "activate:d=16,o=6,b=180:a5,c6,e6,g6,4a6";
const char deactivate[] PROGMEM = "deactivate:d=16,o=6,b=180:a6,g6,e6,c6,4a5";
const char launch[] PROGMEM = "launch:d=16,o=5,b=170:g,a,c6,e6,g6,4c7";
const char land[] PROGMEM = "land:d=16,o=5,b=170:c7,g6,e6,c6,a,4g";
const char connect[] PROGMEM = "connect:d=16,o=5,b=160:f,a,c6,e6,4g6";
const char disconnect[] PROGMEM = "disconnect:d=16,o=5,b=160:g6,e6,c6,a,4f";

//project's constants
const char * melodies[] = {
  // Generic RTTTL melodies
  two_short,
  three_short,
  three_short_burst,
  siren,
  scale_up,
  scale_down,
  s_o_s,
  alert,

  // Ready melodies
  ready01,
  ready02,

  // Notification melodies
  notification01,
  notification02,
  notification03,
  notification04,
  notification05,

  // Success melodies
  bright_ping_cascade,
  success01,
  success02,
  success03,
  success04,
  success05,
  success06,
  success07,
  success08,
  success09,
  success10,
  success11,
  success12,
  success13,
  success14,
  success15,
  
  // Failure melodies
  failure,
  low_buzz_drop,
  fail01,
  fail02,
  fail03,
  fail04,

  // Error melodies
  error01,
  error02,
  error03,
  error04,
  error05,
  error06,

  // Turn-off melodies
  turnoff01,
  turnoff02,
  turnoff03,
  turnoff04,
  turnoff05,

  // Processing data melodies
  processing1,
  processing2,
  processing3,

  // Processing start melodies
  startProc1,
  startProc2,
  startProc3,

  // Processing done melodies
  doneProc1,
  doneProc2,
  doneProc3,
  doneProc4,

  // Non-palindromic reversible melodies
  bootUp,
  powerDown,
  systemStart,
  systemStop,
  activate,
  deactivate,
  launch,
  land,
  connect,
  disconnect,
};
const size_t melodies_count = sizeof(melodies) / sizeof(melodies[0]);
int next_melody_index = 0; // index of next melody to play

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  Serial.begin(115200);
  Serial.println();

#ifdef ESP32
  // setup AnyRtttl for ESP32
  anyrtttl::setToneFunction(&esp32::tone);          // tell AnyRtttl to use AnyRtttl's specialized esp32 tone function.
  anyrtttl::setNoToneFunction(&esp32::noTone);      // tell AnyRtttl to use AnyRtttl's specialized esp32 noTone() function.

  // setup pin for rtttl
  esp32::toneSetup(BUZZER_PIN);
#endif

}

void loop() {
  // If we are not playing something 
  if ( !anyrtttl::nonblocking::isPlaying() )
  {
    // Add a delay between each melodies
    delay(1500);

    // Stay silent when we played all melodies
    if (next_melody_index >= melodies_count ) {
      while(true) {delay(1000);}
    }

    // Get next melody buffer
    const char * next_melody = melodies[next_melody_index];

    // Start playing next melody
    anyrtttl::nonblocking::beginProgMem(BUZZER_PIN, next_melody);

    // Move to next melody
    next_melody_index++;
  }
  else
  {
    anyrtttl::nonblocking::play();
  }
}
