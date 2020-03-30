// Multi-loops skeleton
// Description: Shows 3 loops operating in "parallel" reading/writing a global variable
// Karol Hennessy
// 2020-03-29

#include <VariableTimedAction.h>

// global variables
static int globalcnt = 0;



class SafetyAlarmLoop : public VariableTimedAction
{
  private:
    int cnt = 0; 
    unsigned long run()
    {
      cnt++;
      Serial.println("SA "+ String(globalcnt));
      return 0;  
    }
  public:
    int getCnt()
    {
      return cnt;  
    }
} safetyalarmloop ; 

class BreathingLoop : public VariableTimedAction
{
  private:
    int cnt = 0; 
    unsigned long run()
    {
      cnt++;
      Serial.println("BR "+ String(cnt));
      return 0;  
    }
  public:
    int getCnt()
    {
      return cnt;  
    }
    
} breathingloop; 

class UILoop : public VariableTimedAction
{ 
  private:
    //int cnt = 0; 
    unsigned long run()
    {
      globalcnt++;
      Serial.println("UI "+ String(globalcnt));
      return 0;  
    }
  public:
   /* int getCnt()
    {
      return cnt;  
    }*/
  
} uiloop;


void setup() {
  Serial.begin(9600);
  // note - order is not guaranteed if they all run on the same "cycle"
  safetyalarmloop.start(1000);
  //breathingloop.start(500);
  uiloop.start(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  VariableTimedAction::updateActions();
}
