#include <KNoTThing.h>

// Definitions
#define WATER_FLOW_SENSOR_NAME "YF-S201"
#define WATER_FLOW_SENSOR_ID   1
#define WATER_FLOW_SENSOR_PIN  3

// Constants
const float MILILITER_PER_PULSE = 2.222;
const unsigned short PRECISION = 1000;

// Global Variables
static volatile long _pulses = 0;
static KNoTThing _thing;

static long getPulses()
{
    return _pulses;
}

static float getTotalFlowInMilliliter()
{
    return (getPulses() * MILILITER_PER_PULSE);
}

static float getTotalFlowInLiter()
{
    return getTotalFlowInMilliliter() / PRECISION;
}

static int waterFlowRead(int32_t* val_int, uint32_t* val_dec, int32_t* multiplier)
{
    static int32_t previous_val_int = 0, previous_val_dec = 0;
    double value = getTotalFlowInLiter();
    double fractpart, intpart;

    fractpart = modf(value, &intpart);
    *val_int = (int32_t)  (intpart);
    *val_dec = (uint32_t) (fractpart * PRECISION);
    *multiplier = 1;

    // Debug
    if (previous_val_int != *val_int || previous_val_dec != *val_dec)
    {
        previous_val_int = *val_int;
        previous_val_dec = *val_dec;
        
        Serial.print("=> [");
        Serial.print(previous_val_int, DEC);
        Serial.print(".");
        Serial.print(previous_val_dec, DEC);
        Serial.println("l]");
    }

    return (0);
}

static int waterFlowWrite(int32_t* val_int, uint32_t* val_dec, int32_t* multiplier)
{
    // NOP
    return (0);
}

void increasePulse()
{
    _pulses += 1;
}

void setup()
{
    pinMode(WATER_FLOW_SENSOR_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(WATER_FLOW_SENSOR_PIN), increasePulse, RISING);

    Serial.begin(9600);
    _thing.init("WaterFlowSensor");
    _thing.registerFloatData(WATER_FLOW_SENSOR_NAME, WATER_FLOW_SENSOR_ID, KNOT_TYPE_ID_VOLUMEFLOW, KNOT_UNIT_VOLUME_L, waterFlowRead, waterFlowWrite);
}

void loop ()
{
    _thing.run();
}

