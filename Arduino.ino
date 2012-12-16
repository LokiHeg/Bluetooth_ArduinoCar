const int RIGHT = 5, LEFT = 6, CONTROL_R = 4, CONTROL_L = 7, PIN_DIST = 3, SPEED = 240; // задаем переменные для работы с нужными выводами и значение скорости для pwm 
int flagL = 0, flagR = 0, flag = 0, winCtrl = 0; // переменные для контроля текущего состояния (режима) работы

// левые колеса вперед..........................................................................

void leftForward (char a)
{
  digitalWrite(CONTROL_L, HIGH);
  analogWrite(LEFT, a);
  flagL = 'f';
}

// правые колеса вперед.........................................................................

void rightForward (char a)
{
  digitalWrite(CONTROL_R, LOW);
  analogWrite(RIGHT, a);
  flagR = 'f';
}
  
// левые колеса назад...........................................................................
  
void leftBack (char a)
{
  digitalWrite(CONTROL_L, LOW);
  analogWrite(LEFT, a);
  flagL = 'b';
}

// правые колеса назад...........................................................................

void rightBack (char a)
{
  digitalWrite(CONTROL_R, HIGH);
  analogWrite(RIGHT, a);
  flagR = 'b';  
}

// остановка левых колес.........................................................................

void leftStop()
{
  digitalWrite(LEFT, LOW);
}

// остановка правых колес........................................................................

void rightStop()
{
  digitalWrite(RIGHT, LOW);
}
  
// движение вперед...............................................................................
  
void goForward (char l, char r)
{
  rightForward(r);  
  leftForward(l);
}

// движение назад................................................................................

void goBack(char l, char r)
{
  rightBack(r);
  leftBack(l);  
}

// поворот влево.................................................................................

void turnLeft(char t)
{
  rightForward(t);
  leftBack(t);
}  

// левый поворот при  движении назад.............................................................

void turnBackLeft(char t)
{
  leftForward(t);
  rightBack(t);
}

// поворот вправо................................................................................

void turnRight(char t)
{
  leftForward(t);
  rightBack(t);
}

// правый поворот при движении назад..............................................................

void turnBackRight(char t)
{
  rightForward(t);
  leftBack(t);
}  

// остановка......................................................................................

void stopAll()
{
  rightStop();  
  leftStop();
  digitalWrite(CONTROL_R, LOW);
  digitalWrite(CONTROL_L, HIGH);
}

// считывание данных с датчика расстояния..........................................................

double distance ()
{
  double u = analogRead(PIN_DIST);  
  double dist = (double) u / 4.9;
  return dist;
}

// средние показания данных с датчика расстояния.................................................... 

double distAverage()
{
  double s = 0;
  double dist[10];
  for (int i = 0; i < 10; i++)
    dist[i] = distance(), delayMicroseconds(500);
  for (int i = 0; i < 10; i++)
    s += dist[i];
  return s / 10;  
}
 
// поворот в произвольном направлении................................................................ 
 
void turnRand()
{
  randomSeed(millis());
  int r = random(0, 100);
  while(distAverage() < 8)
  {  
    if(r % 2 == 0)
      turnLeft(SPEED);
    else
      turnRight(SPEED);
  }
}  
  
// авторежим..........................................................................................
  
void Auto()
{
  if (distAverage() < 7)
    turnRand();
  if(distAverage() >= 7)
    goForward(SPEED, SPEED);
}

// функция setup().....................................................................................

void setup()
{
  Serial.begin(9600);
  pinMode (RIGHT, OUTPUT);
  pinMode (LEFT, OUTPUT);
  pinMode (CONTROL_R, OUTPUT);
  pinMode (CONTROL_L, OUTPUT);
  pinMode(PIN_DIST, OUTPUT);
  stopAll();
}

// управление с windows................................................................................

int windows()
{
 if(Serial.available() > 0) // если есть доступные данные из последовательного порта
 {
    // если доступный байт не равен 'w' - выходим
    if (Serial.peek() != 'w')
      return -1;
  winCtrl = 1; // управление ведется с winows - устанавливаем флаг winCtrl в 1    
  while(1) // входим в бесконечный цикл
  {  
    int value = Serial.read();
    stopAll();
    // остановка перед препятствиями при управлении в ручном режиме  
    if (distAverage() < 7)
      stopAll();
    // поворот вправо пока передается нужный сигнал
    while (value == 'r')
    {
      turnRight(SPEED);
      // если передается сигнал break ('m') - остановка, выход из цикла
      if (Serial.read() == 'm')
      {
        stopAll();
        break;
      }
    }
    // поворот влево пока передается нужный сигнал
    while (value == 'l')
    {
      turnLeft(SPEED);
      // если передается сигнал break ('m') - остановка, выход из цикла
      if (Serial.read() == 'm')
        {
          stopAll();
          break;
        } 
    }
    // поворот вправо пока передается нужный сигнал
    while (value == 'p')
    {
      turnBackRight(SPEED);
      // если передается сигнал break ('m') - остановка, выход из цикла
      if (Serial.read() == 'm')
      {
        stopAll();
        break;
      }
    }
    // поворот влево пока передается нужный сигнал
    while (value == 'o')
    {
      turnBackLeft(SPEED);
      // если передается сигнал break ('m') - остановка, выход из цикла
      if (Serial.read() == 'm')
      {
        stopAll();
        break;
      }
    }
    // движение вперед пока передается нужный сигнал
    while (value == 'f')
    {
      int val = Serial.read();
      goForward(SPEED, SPEED);
      // если передается сигналы break ('m') или 'l' (влево), или 'r' (вправо), либо при прибдижении к препятствию - остановка, выход из цикла "вперед" 
      if(val == 'm' || val == 'l' || val == 'r' || distAverage() < 7)
      {
        stopAll();
        break;
      }
    }
    // движение назад пока передается нужный сигнал
    while (value == 'b')
    {
      int val = Serial.read();
      goBack(SPEED, SPEED);
      // если передается сигналы break ('m') или 'o' (влево), или 'p' (вправо)  - остановка, выход из цикла "назад" 
      if(val == 'm' || val == 'o' || val == 'p')
      {
        stopAll();
        break;
      }
    }
    // включение авторежима, если получен нужный сигнал ('s')
    while (value == 's')
    {
      Auto();
      // если получен сигнал для выхода из авторежима - остановка, выход из цикла авторежима
      if (Serial.available() > 0 && Serial.read() == 'm')
        {
          stopAll();
          break;
        }
    }
    stopAll();
    // выход, если получен сигнал выхода ('e')
    if (value == 'e')
      return 0;
  }
  }
  else 
    stopAll();
}

// управление с android................................................................................

void android()
{
  // если флаг управления с windows равен 1 - выходим
  if (winCtrl == 1)
    return;
  // входим в бесконечный цикл  
  while(1)
  {
  if(Serial.available() > 0)
  {  
    int value = Serial.read();
    // аворежим - аналогично windows
    while (value == 's')
    {
      Auto();
      if (Serial.available() > 0 && Serial.read() == 's')
        {
          stopAll();
          break;
        }
    }
    // выходим из функции, если получен сигнал 'e'
    if (value == 'e')
      {
        stopAll();
        Serial.end();
        return;
      }
    // остановка перед препятствиями при управлении в ручном режиме  
    if (distAverage() < 7)
      stopAll();
    // входим в цикл движения вперед, если получен сигнал 'f'  
    while (value == 'f')
    { 
       goForward(SPEED, SPEED);  
       int val = Serial.read();
       // входим в цикл поворота влево, если получен сигнал 'l'  
       while (val == 'l')
       {
         turnLeft(SPEED), delay(5);
         // выход из цикла поворота влево, если повторно получен сигнал 'l'  
         if (Serial.available() && Serial.read() == 'l')
           break;
       } 
       // входим в цикл поворота вправо, если получен сигнал 'r'  
       while (val == 'r')
       {
         turnRight(SPEED), delay(5);  
         // выход из цикла поворота вправо, если повторно получен сигнал 'r'  
         if (Serial.available() && Serial.read() == 'r')
           break;
       }
       // выход из цикла "вперед", если получены соответсвующие сигналы или при приближении к препятствию
       if (val == 'f' || val == 'b' || distAverage() < 7 || val == 'e' || val == 's')
         break;
       // входим в цикл авторежима, если пришел сигнал 's'  
       while (val == 's')
       {
         Auto();
         // выход из цикла авторежима, если повторно получен сигнал 's'
         if (Serial.available() > 0 && Serial.read() == 's')
           {
             stopAll();
             break;
           }
       }  
    }
    // входим в цикл движения назад, если получен сигнал 'b' (всё аналогично движению вперед)  
    while (value == 'b')
    { 
       goBack(SPEED, SPEED);    
       int val = Serial.read();
       while (val == 'l')
       {
         turnLeft(SPEED), delay(5);
         if (Serial.available() && Serial.read() == 'l')
           break;
       } 
      while (val == 'r')
       {
         turnRight(SPEED), delay(5);  
         if (Serial.available() && Serial.read() == 'r')
           break;
       }
       if (val == 'f' || val == 'b' || val == 'e' || val == 's')
         break;
       while (val == 's')
       {
         Auto();
         if (Serial.available() > 0 && Serial.read() == 's')
           {
             stopAll();
             break;
           }
       }  
    }
  }
  else
    stopAll();
  }
}

// функция loop()....................................................................................

void loop()
{
  // вызываем функцию android(), если не ведется управление с windows
  if (windows() == -1)
    android();
}
  

