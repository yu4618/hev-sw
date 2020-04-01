#include <Arduino.h>
//#include <LiquidCrystal.h>

#if defined(ARDUINO_FEATHER_ESP32)
#define BOARD "HUZZAH32"
#include <huzzah32_pinout.h>
#elif defined(ARDUINO_NodeMCU_32S)
#define BOARD "ESP32"
#include <nodemcu_32s_pinout.h>
#elif defined(ARDUINO_AVR_UNO)
#define BOARD "UNO"
#include <Arduino_uno_pinout.h>
#elif defined(ARDUINO_SAMD_MKRVIDOR4000)
#define BOARD "VIDOR"
#include <Arduino_MKR_4000_Vidor_pinout.h>
#elif defined(ARDUINO_SAMD_MKRWIFI1010)
#define BOARD "MKR1010"
#include <Arduino_MKR_1010_WIFI_pinout.h>
#endif

//LiquidCrystal lcd(pin_lcd_rs, pin_lcd_en, pin_lcd_d4, pin_lcd_d5, pin_lcd_d6, pin_lcd_d7);
// input params
enum hev_modes : byte
{
    HEV_MODE_PS,
    HEV_MODE_CPAP,
    HEV_MODE_PRVC,
    HEV_MODE_TEST
};

enum valve_states : bool
{
    V_OPEN = LOW,
    V_CLOSED = HIGH
};

enum lab_cycle_modes : byte
{
    LAB_MODE_BREATHE = 0,
    LAB_MODE_PURGE = 1,
    LAB_MODE_FLUSH = 2
};
int ventilation_mode = HEV_MODE_PS;
float working_pressure = 1;             //?
float inspiratory_minute_volume = 6000; // ml/min
float respiratory_rate = 15;            //  10-40 +-1 ;aka breaths_per_min
float inspiratory_pressure = 10;        // 10-80 cmH2O +-1
//float tidal_volume = 200; // calc 200-1500ml +- 100
float inspiratory_time = 1.0; // 0.4-1.5s +-0.1
float pause_time = 1.0;       // range?
//float expiratory_time ; // calc
float expiratory_minute_volume; // calc?? same as inspiratory_minute_volume?
float trigger_sensitivity;

// states
enum BS_STATES : byte
{
    BS_IDLE,
    BS_BUFF_PREFILL,
    BS_BUFF_FILL,
    BS_BUFF_LOADED,
    BS_BUFF_PRE_INHALE,
    BS_INHALE,
    BS_WAIT,
    BS_EXHALE_FILL,
    BS_EXHALE,
    BS_BUFF_PURGE,
    BS_BUFF_FLUSH
};

int bs_state = BS_IDLE;

byte lab_cycle_mode = 0;
bool start = LOW;

// calculations
float calc_tidal_volume()
{
    return inspiratory_minute_volume / respiratory_rate;
}

float calc_expiration_time()
{
    float total_respiratory_time = 60.0 / respiratory_rate;
    // total = expire + inspire + pause
    return (total_respiratory_time - inspiratory_time - pause_time);
}

float calc_expiratory_minute_volume()
{
    // probably need to calculate this from readings
    return 0;
}

// FROM OVERLEAF DOC
// Working pressure, corresponding to the manually set and monitored input pressure to the unit
// Inspiratory Minute Volume setting
// Breaths per minute setting
// Tidal volume display, based on previous two parameters
// Inspiration time setting
// Pause time setting
// expiration time display based on previous two parameters
// expired minute volume
// Airway pressure display based on the reading of $P_2$ ??? not a setting
// PEEP setting ??? external
// Trigger sensitivity to patient initiated breath

void setup()
{

    pinMode(pin_valve_in, OUTPUT);
    pinMode(pin_valve_out, OUTPUT);
    pinMode(pin_valve_scavenge, OUTPUT);
    pinMode(pin_valve_purge, OUTPUT);

    pinMode(pin_p_supply, INPUT);
    pinMode(pin_p_regulated, INPUT);
    pinMode(pin_p_buffer, INPUT);
    pinMode(pin_p_inhale, INPUT);
    pinMode(pin_p_patient, INPUT);
    pinMode(pin_temp, INPUT);

    pinMode(pin_led_0, OUTPUT);
    pinMode(pin_led_1, OUTPUT);
    pinMode(pin_led_2, OUTPUT);

    pinMode(pin_buzzer, OUTPUT);
    pinMode(pin_button, INPUT);

    Serial.begin(9600);

    // lcd.begin(16, 2);  // Declare number of columns and rows
}

void setValves(bool vin, bool vout, bool vscav, bool vpurge)
{

    digitalWrite(pin_valve_in, vin);
    digitalWrite(pin_valve_out, vout);
    digitalWrite(pin_valve_scavenge, vscav);
    digitalWrite(pin_valve_purge, vpurge);
}

void breath_cycle()
{
    // basic cycle for testing hardware
    int next_state;
    switch (bs_state)
    {
    case BS_IDLE:
        start = digitalRead(pin_button);
        if (start == HIGH)
        {
            next_state = BS_BUFF_PREFILL;
        }
        else
        {
            delay(1000);
            next_state = BS_IDLE;
        }
        setValves(V_CLOSED, V_OPEN, V_OPEN, V_CLOSED);
        break;
    case BS_BUFF_PREFILL:
        setValves(V_CLOSED, V_CLOSED, V_OPEN, V_CLOSED);
        delay(1000);
        next_state = BS_BUFF_FILL;
        break;
    case BS_BUFF_FILL:
        setValves(V_OPEN, V_CLOSED, V_OPEN, V_CLOSED);
        delay(500);
        next_state = BS_BUFF_LOADED;
        break;
    case BS_BUFF_LOADED:
        setValves(V_CLOSED, V_CLOSED, V_OPEN, V_CLOSED);
        switch (lab_cycle_mode)
        {
        case LAB_MODE_FLUSH:
            delay(500);
            next_state = BS_BUFF_FLUSH;
            break;
        case LAB_MODE_PURGE:
            delay(500);
            next_state = BS_BUFF_PURGE;
            break;
        default:
            delay(1500);
            next_state = BS_BUFF_PRE_INHALE;
        }
        break;
    case BS_BUFF_PRE_INHALE:
        setValves(V_CLOSED, V_CLOSED, V_CLOSED, V_CLOSED);
        delay(100);
        next_state = BS_INHALE;
        break;
    case BS_INHALE:
        setValves(V_CLOSED, V_OPEN, V_CLOSED, V_CLOSED);
        delay(100);
        next_state = BS_WAIT;
        break;
    case BS_WAIT:
        setValves(V_CLOSED, V_CLOSED, V_CLOSED, V_CLOSED);
        delay(1000);
        next_state = BS_EXHALE_FILL;
        break;
    case BS_EXHALE_FILL:
        setValves(V_OPEN, V_CLOSED, V_OPEN, V_CLOSED);
        delay(1000);
        next_state = BS_EXHALE;
        break;
    case BS_EXHALE:
        setValves(V_CLOSED, V_CLOSED, V_OPEN, V_CLOSED);
        delay(10);
        next_state = BS_BUFF_LOADED;
        break;
    case BS_BUFF_PURGE:
        setValves(V_CLOSED, V_CLOSED, V_OPEN, V_OPEN);
        delay(1000);
        next_state = BS_BUFF_PREFILL;
        break;
    case BS_BUFF_FLUSH:
        setValves(V_CLOSED, V_OPEN, V_OPEN, V_CLOSED);
        delay(1000);
        next_state = BS_IDLE;
        break;
    default:
        next_state = bs_state;
    }
    bs_state = next_state;
}


void loop()
{
    // put your main code here, to run repeatedly:
    //Serial.println(BOARD);
    // delay(1000);

    // buzzer
    // tone(pin, freq (Hz), duration);
    breath_cycle();
    Serial.println("state: " + String(bs_state));
}
