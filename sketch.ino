
#define VALVE_RELAY_PIN 1
#define MOTION_SENSOR_PIN 0 

#define VALVE_OPEN_TIME_MS 1000 * 60 // 1 minute
#define HOLD_TIME_MS 1000 * 60 * 10 // 10 minutes

// Operation Modes =========================================
#define OM_STANDBY 0
#define OM_ACTIVE 1
#define OM_HOLD 2

int activeOperationMode = OM_STANDBY;

void setMode(int mode)
{
  switch (mode)
  {
    case OM_STANDBY:
      initStandByMode();
      break;
    case OM_ACTIVE:
      initActiveMode();
      break;
    case OM_HOLD:
      initHoldMode();
      break;
  }
  activeOperationMode = mode;
}

// Standby Mode =========================================
void initStandByMode()
{
}

void runStandByMode()
{
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

// Setup System =========================================
void setup()
{  
  pinMode(MOTION_SENSOR_PIN, INPUT);
  pinMode(VALVE_RELAY_PIN, OUTPUT);
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
  default:
    runStandByMode();
  }
}
