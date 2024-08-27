// Please modify accordingly
const int PULSE_SENSOR_PIN = 5;  // Analog PIN where the PulseSensor is connected
int const LED_BLUE = 8;
int const LED_GREEN = 9;
int const LED_YELLOW = 10; 
int const LED_RED = 11;        

int const BUTTON_PIN_1 = 0;
int const BUTTON_PIN_2 = 1;
int const BUTTON_PIN_3 = 2;     

int const READ_THRESHOLD = 40;
int const LOW_THRESHOLD = 50;       // Blue
int const MED_THRESHOLD = 60 ;      // Green
int const HIGH_THRESHOLD = 70;      // Yellow 
int const VERY_HIGH_THRESHOLD = 80; // Red

int const RX_FOR_MP3 = 13;
int const TX_FOR_MP3 = 12;
int const MP3_VOLUME = 5; // 0-30


// Do not modify below
int currentBPM = 0; // No need to worry

int currentBranch = 0;

bool startStoryDone = false;
bool midStoryDone = false;
bool endStoryDone = false;

