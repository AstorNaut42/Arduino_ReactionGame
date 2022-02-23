#include <Button.h>

Button SW1(3); // Connect your button between pin 3 and GND
Button SW2(4); // Connect your button between pin 4 and GND

int redLED = 10;     //RGB-rød
int greenLED = 9;    //RGB-grønn
int blueLED = 8;     //RGB-blå
int buzzerPin = 11;

int LED1 = 6;  //Spiller 1 - LED
int LED2 = 7; //Spiller 2 - LED

int incomingByte;
int champion;
int championLED;
int game_points = 0;

int winner = 0;         //Initialiserer vinnerindikatoren
int winnerBeep = 750;   //Buzzer-pitchen for vinnerfanfaren
int fault = 0;          //Initialiserer feilindikatoren
int faultBeep = 200;    //Buzzer-pitchen for feillyden

unsigned long wait = 0;
unsigned long now = 0;
unsigned long responseTime = 0;

int timePassedRestart = 0;
long prevNow = 0;
long newNow = 0;
int nowRestart = 0;
int millis_restart = 0;
unsigned long millisMomentan = 0;   

int gameState = 0;    // Variabel som styrer hele gangen i spillet
int gamePoints[2];
int blueLight = 0;

#define WAITMODE 0    
#define GAMEON 1
#define WINNER 2
#define LOSER 3
# define FINAL 4


float randomNumbFunc(){
  wait = random(3000,6000);
} // end randomFunc

int randomBlue(){
  blueLight = random(0,4);
  }


void setup(){

  SW1.begin();
  SW2.begin();

  // vet ikke hva denne gjør, lagt til i lag med knappebibliotek
  while (!Serial) { }; // for Leos

  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(buzzerPin, OUTPUT); 
  
  randomSeed(analogRead(A0));
  Serial.begin(9600);

  randomNumbFunc();
  randomBlue();
  Serial.print("    ======||====== WAR OF LIGHTS ======||======");
  Serial.println(" ");
  Serial.println(" Skriv 'S' i konsollen for å starte spillet ");
}




int winSequence(){
    randomBlue();
    int timer = 100;
    timePassedRestart = 0;

    // Legger til et poeng til den som vant runden
    int points = pointSystem();
    gamePoints[winner] += points; 

  for(int i = 0; i < 6; i++){
    digitalWrite(greenLED, HIGH);
    tone(buzzerPin, winnerBeep +i*100);
    delay(timer);
    digitalWrite(greenLED, LOW);
    noTone(buzzerPin);
    delay(timer);
    }
  digitalWrite(greenLED, LOW);
  digitalWrite(redLED, LOW);  
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);

    // Gjør sånn at tiden kan resettes når spillfunksjonen starter
  millis_restart = 0;
  nowRestart = 0;
  Serial.println((String)"Blue Player: "+gamePoints[0]+" || Yellow Player: "+gamePoints[1]); 

   // Går videre til spillfunksjonen GAMEON
  gameState = 1;
  
  
  }


void resetLEDs(){
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, LOW);
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(blueLED, LOW);
  }


int pointSystem(){
  if(responseTime <= 200){
    game_points = 10;
    } 
  if(responseTime > 200 and responseTime < 400){
    game_points = 5;
    }
  if(responseTime > 400){
    game_points = 1;
    }
    
  return game_points;
  }


void loseSequence(){
    randomBlue();
    timePassedRestart = 0;
    int timer = 100;

    // tar et poeng fra den som trykket feil
    gamePoints[fault] -= 5;

    for(int x = 1; x < 15; x++){
      analogWrite(redLED, x*10);
      tone(buzzerPin, faultBeep);
      delay(timer);
      }
    noTone(buzzerPin);
    resetLEDs();

    // Gjør sånn at tiden kan resettes når spillfunksjonen starter
    millis_restart = 0;
    nowRestart = 0;
    Serial.println((String)"Blue Player: "+gamePoints[0]+" || Yellow Player: "+gamePoints[1]); 

    // Går videre til spillfunksjonen GAMEON
    gameState = 1;
   
  
  } 


// Dette er selve spillfunksjonen, den som kjører det meste av spillet
unsigned long gameStart(unsigned long millis_passed, int wait){



  // Sjekker om noen har vunnet sammenlagt. Dvs fått 5 poeng. Går da videre til case 4 (FINAL)
  if (gamePoints[winner] > 29){
    gameState = 4;
    return;
    }

 // Grusom workaround for å kompensere for at det er gått noen millisekunder innen denne funksjonen skal kjøres
  if (millis_restart == 0){
    millis_restart = 1;
    millisMomentan = millis_passed;
    }   
   now = millis_passed - millisMomentan;   



    // Sjekker om det ikke er gått like lang tid som gitt av det tilfeldig genererte tallet
   if(now <= wait){
    
    digitalWrite(redLED, HIGH);
    digitalWrite(greenLED, LOW); 

      // Sett med if setninger som sjekker om noen trykker på knappen før tiden er gått
      if(SW1.pressed()){
        digitalWrite(LED1, HIGH); 
        digitalWrite(redLED, HIGH);

        // Setter at spiller 1 ( 0 indeksert) er den som har feil (fault), og går videre til gamestate 3 som er LOSER casen 
        fault = 0;
        gameState = 3;
        } 
      
      if(SW2.pressed()){
        digitalWrite(LED2, HIGH); 
        digitalWrite(redLED, HIGH);

        // Setter at spiller 2 ( 0 indeksert) er den som har feil (fault), og går videre til gamestate 3 som er LOSER casen 
        fault = 1;
        gameState = 3;
        } 
    
    } // End if lyset fortsatt er rødt


    // Sjekker om lyset skal endres fra rødt
    if (now >= wait){

      // Starter en ny telling for når lyset endres fra rødt
       if (nowRestart == 0){
            newNow = now;
            nowRestart = 1;
            }


       // Sjekker om det skal lyse blått 
      if (blueLight == 3){
        digitalWrite(redLED, LOW);
        digitalWrite(blueLED, HIGH);

          // Sett med if setninger som sjekker om noen trykker på knappen når lyset er blått
          if(SW1.pressed()){
            digitalWrite(LED1, HIGH); 
            digitalWrite(redLED, HIGH);
    
            // Setter at spiller 1 ( 0 indeksert) er den som har feil (fault), og går videre til gamestate 3 som er LOSER casen 
            fault = 0;
            gamePoints[0] -= 5; 
            gameState = 3;
            
            } 
          
          if(SW2.pressed()){
            digitalWrite(LED2, HIGH); 
            digitalWrite(redLED, HIGH);
    
            // Setter at spiller 2 ( 0 indeksert) er den som har feil (fault), og går videre til gamestate 3 som er LOSER casen 
            fault = 1;
            gamePoints[1] -= 5; 
            gameState = 3;
            } 

            // Venter i 2 sekunder, oppdaterer da det tilfeldige tallet som gir blått lys, og går mest sannsynlig over til grønt lys igjen
            if(now - newNow > 2000){
              timePassedRestart = 0;
              randomBlue();
              }
  
        }// End if lyset skal lyse blått



      
      else if (blueLight != 3){
     
         if (timePassedRestart == 0){
              prevNow = now;
              timePassedRestart = 1;
              }

        digitalWrite(redLED, LOW);
        digitalWrite(greenLED, HIGH);

          // Sett med if setninger som sjekker hvem som trykker først etter lampen er blitt grønn
        if (SW1.pressed()){
          responseTime = (now - prevNow);
          Serial.println(" ");
          Serial.println((String)"Blå Spiller brukte "+responseTime+" ms.");
          digitalWrite(LED1, HIGH); 
          // Setter at spiller 1 ( 0 indeksert) er den som vant (winner), og går videre til gamestate 2 som er WINNER casen 
          winner = 0;
          gameState = 2;
          }

         // Setter at spiller 2 ( 0 indeksert) er den som vant (winner), og går videre til gamestate 2 som er WINNER casen 
        if (SW2.pressed()){
          responseTime = (now - prevNow);
          Serial.println(" ");
          Serial.println((String)"Gul Spiller brukte "+responseTime+" ms.");
          digitalWrite(LED2, HIGH);
          winner = 1;
          gameState = 2;
          }
        } // End if ikke blått lys
        
     }// End if now >= wait 


}


// Lys og lydsekvens for den som vant sammenlagt
void finalWin(){

  resetLEDs();

  if( gamePoints[0] > gamePoints[1]){
     Serial.println(" === BLÅ SPILLER VANT! === ");
     championLED = LED1;
     champion = 0;
    }

  else if ( gamePoints[0] < gamePoints[1]){
    Serial.println(" === GUL SPILLER VANT! === ");
    championLED = LED2;
    champion = 1;
    }

  else if ( gamePoints[0] == gamePoints[1]){
    Serial.println(" === Ingen vinner! === ");
    championLED = greenLED;
    }

  for(int i = 1; i < 4; i++){
    for (int j = 1; j < 4; j++){
      tone(buzzerPin, faultBeep + j*100);
      analogWrite(greenLED, j*25);
      digitalWrite(championLED, HIGH);
      delay(100);
      }
    for (int k = 1; k < 4; k++){
      tone(buzzerPin, winnerBeep +k*100);
      analogWrite(redLED, k*25);
      digitalWrite(championLED, LOW);
      delay(100);
    }
    noTone(buzzerPin);
    resetLEDs();
  }
}


void waitFunc(){

  resetLEDs();

  // Sjekker om bokstaven s er blitt skrevet i Serial monitor
  if (incomingByte == 'S' || incomingByte == 's' ){
    gameState = 1;
    }

    // Tilbakestiller poengsum
    gamePoints[0]=0;
    gamePoints[1]=0;
  }




void loop(){

   if (incomingByte == 'Q' || incomingByte == 'q'){
      gameState = 4;
      }

   if (incomingByte == 'R' || incomingByte == 'r'){
      gameState = 0;
      }
      

   if(Serial.available() > 0){
      incomingByte = Serial.read(); 
    }
 
  // Holder telling på millisekunder
  float currentMillis = millis();

  // Switch case som kjører hele gangen i spillet. Begynner på WAITMODE som kun er ventemodus
  switch (gameState) {
    case WAITMODE:
    waitFunc();
    break;

    
    case GAMEON:
    // Starter spillet
    gameStart(currentMillis, wait);
    break;

    
    case WINNER:
    // Kjører vinnersekvensen, og oppdaterer det tilfeldige tallet
    winSequence();
    randomNumbFunc();
    break;

    
    case LOSER:
    // Kjører tapersekvensen og oppdaterer det tilfeldige tallet
    loseSequence();
    randomNumbFunc();    
    break;

    case FINAL:
    // Kjører en seierssekvens når en spiller har nådd 5 poeng. Reesetter så til ventemodus
    finalWin();
    gameState = 0;
    break;
  
  
  } // End switch case



}
