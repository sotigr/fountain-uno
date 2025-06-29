
#define VALVE_RELAY_PIN 4
#define FLUSH_VALVE_RELAY_PIN 2
#define MOTION_SENSOR_PIN 7

#define MINUTE 60000 // milliseconds in a minute
#define VALVE_OPEN_TIME_MS MINUTE // 1 minute
#define HOLD_TIME_MS MINUTE * 10 // 10 minutes
#define FLUSH_TIME_MS MINUTE * 2 // 2 minutes
#define FLUSH_EVERY_MS MINUTE * 60 * 12 // 12 hours 

// Operation Modes =========================================
#define OM_STANDBY 0
#define OM_ACTIVE 1
#define OM_HOLD 2
#define OM_FLUSH 3


int activeOperationMode = OM_STANDBY;

void setMode(int mode)
{
  switch (mode)
  {
    case OM_STANDBY:  
      //Serial.print("setting mode to OM_STANDBY");
      initStandByMode();
      break;
    case OM_ACTIVE:
      //Serial.print("setting mode to OM_ACTIVE");
      initActiveMode();
      break;
    case OM_HOLD:
      //Serial.print("setting mode to OM_HOLD");
      initHoldMode();
      break;
    case OM_FLUSH:
      //Serial.print("setting mode to OM_FLUSH");
      initFlushMode();
      break;
  }
  activeOperationMode = mode;
}

// Standby Mode =========================================
struct StandByState
{
  unsigned long flushStartTime; 
};

StandByState standbyState;
void initStandByMode()
{ 
}

void runStandByMode()
{
  unsigned long currentTime = millis();
  if (currentTime - standbyState.flushStartTime > FLUSH_EVERY_MS) { 
    setMode(OM_FLUSH);
    return;
  }

  int sensorValue = digitalRead(MOTION_SENSOR_PIN);
  if (sensorValue == HIGH)
  {
     setMode(OM_ACTIVE);
  } 
}

// Active Mode =========================================
struct ActiveState
{
  unsigned long startTime; 
};
ActiveState activeState;

void initActiveMode()
{
  activeState = {
    startTime: millis()
  };
  digitalWrite(VALVE_RELAY_PIN, HIGH);
}

void runActiveMode()
{
  unsigned long currentTime = millis();
  if (currentTime - activeState.startTime > VALVE_OPEN_TIME_MS) {
    digitalWrite(VALVE_RELAY_PIN, LOW);
    setMode(OM_HOLD);
  } 
}

// Hold Mode =========================================
struct HoldState
{
  unsigned long startTime; 
};
HoldState holdState;

void initHoldMode()
{
  holdState = {
    startTime: millis()
  }; 
}

void runHoldMode()
{
  unsigned long currentTime = millis();
  if (currentTime - holdState.startTime > HOLD_TIME_MS) { 
    setMode(OM_STANDBY);
  } 
}

// Flush Mode =========================================
struct FlushState
{
  unsigned long startTime; 
};
FlushState flushState;
void initFlushMode()
{
  flushState = {
    startTime: millis()
  }; 
  digitalWrite(FLUSH_VALVE_RELAY_PIN, HIGH);
}
void runFlushMode()
{
  unsigned long currentTime = millis(); 
  if (currentTime - flushState.startTime > FLUSH_TIME_MS) { 
    digitalWrite(FLUSH_VALVE_RELAY_PIN, LOW);
    standbyState.flushStartTime = millis();
    setMode(OM_STANDBY); 
  } 
}

// Setup System =========================================
void setup()
{  
  //Serial.begin(9600); // open the serial port at 9600 bps:
  pinMode(MOTION_SENSOR_PIN, INPUT);
  pinMode(VALVE_RELAY_PIN, OUTPUT);
  pinMode(FLUSH_VALVE_RELAY_PIN, OUTPUT);

  digitalWrite(VALVE_RELAY_PIN, LOW);
  digitalWrite(FLUSH_VALVE_RELAY_PIN, LOW);
}

void loop()
{
  switch (activeOperationMode)
  {
  case OM_ACTIVE:
    runActiveMode();
    break;
  case OM_STANDBY:
    runStandByMode();
    break;
  case OM_HOLD:
    runHoldMode();
    break;
  case OM_FLUSH:
    runFlushMode(); 
    break;
  default:
    runStandByMode();
    break;
  }
}
