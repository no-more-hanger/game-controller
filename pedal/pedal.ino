#define SENSOR 2    // 센서핀 번에 연결

volatile int count, old_count;
unsigned long old_time = 0;

unsigned long ignore_milli = 50;
unsigned long old_time_temp = 0;
bool flag = false;

/* ------------ rpm constant ----------------*/
float coefficient = 60; // 회전수에 대한 시간 기준 (ex. 초당, 분당 등등) (단위: s)
float interval = 5000; // rpm 계산하는 간격 (단위: ms)

void setup()
{
  count = 0;
  old_count = 0;
  old_time = millis();
  old_time_temp = millis();
  
  pinMode(SENSOR, INPUT);

  Serial.begin(9600); 
  // Serial.println("start");
}

void counting() 
{
  if (millis() - old_time_temp > ignore_milli) 
  {
    old_time_temp = millis();
    return;
  }
  if (digitalRead(SENSOR) == LOW && !flag)
  {
    flag = true;
    count++;
  } 
  else if (digitalRead(SENSOR) == HIGH) 
  {
    flag = false;
  }
}

void loop()
{
  // Serial.print("value:");
  // Serial.println(digitalRead(SENSOR));
  // Serial.print("\t");
  // Serial.println(count);
  counting();

  if (old_count != count)
  {
    Serial.println(count);
    old_count = count;
  }
  unsigned long time = millis(); // 현재 시간
  if (time - old_time >= interval) {

    float rpm = coefficient * 1000 / (time - old_time) * count;
    count = 0;
    old_time = millis();
    Serial.println((String) "속도: " + rpm);
  }
}