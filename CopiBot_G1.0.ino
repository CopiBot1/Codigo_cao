#include <HCSR04.h>

//Sensor 1 (frontal)
#define Trig1_PIN 4
#define Echo1_PIN 3

//Sensor 2 (esquerda)
#define Trig2_PIN 9
#define Echo2_PIN 8

//Sensor 3 (direita)
#define Trig3_PIN A5
#define Echo3_PIN A4

//Motor 1 (roda esquerda)
#define Vel_Motor1 6
#define Mla1 12
#define Mlb1 13

//Motor 2 (roda direita)
#define Vel_Motor2 5
#define Mla2 11
#define Mlb2 10

//Declaração de variáveis e constantes
unsigned long agora = 0, antes = 0;
int vel = 100, vel2 = 100;
float distancia[3] = {0, 0, 0};

#define Dis_Sensor_Frontal 0 //Desconto (mm) da distância do sensor 1
#define Dis_Sensor_Esq 0     //Desconto (mm) da distância do sensor 2
#define Dis_Sensor_Dir 0     //Desconto (mm) da distância do sensor 3
#define Tolerancia 200       //Tolerância (mm) de distância mínima permitida 

//Declaração de funções
int verificar(float x, float y, float z);
void ligaMotor(int v1, int a1, int b1, int v2, int a2, int b2);

//Definição dos sensores
HCSR04 sensor_1(Trig1_PIN, Echo1_PIN);
HCSR04 sensor_2(Trig2_PIN, Echo2_PIN);
HCSR04 sensor_3(Trig3_PIN, Echo3_PIN);

void setup()
{
  //Inicialização dos sensores
  pinMode(Trig1_PIN, OUTPUT);
  pinMode(Echo1_PIN, INPUT);
  pinMode(Trig2_PIN, OUTPUT);
  pinMode(Echo2_PIN, INPUT);
  pinMode(Trig3_PIN, OUTPUT);
  pinMode(Echo3_PIN, INPUT);

  //Inicialização dos motor 1 (roda esquerda) 
  pinMode(Vel_Motor1, OUTPUT);
  pinMode(Mla1, OUTPUT);
  pinMode(Mlb1, OUTPUT);
  digitalWrite(Mlb1, HIGH);
  digitalWrite(Mla1, LOW);
  analogWrite(Vel_Motor1, vel);

  //Inicialização dos motor 2 (roda direita) 
  pinMode(Vel_Motor2, OUTPUT);
  pinMode(Mla2, OUTPUT);
  pinMode(Mlb2, OUTPUT);
  digitalWrite(Mla2, HIGH);
  digitalWrite(Mlb2, LOW);
  analogWrite(Vel_Motor2, vel2);

  Serial.begin(9600); 
}

void loop()
{
  agora = millis();
  
  //Liga e desliga o sensor 1
  digitalWrite(Trig1_PIN, HIGH);
  if((agora - antes) == 1)
  {
    digitalWrite(Trig1_PIN, LOW);
    antes = agora;
  }
  //Aquisição da distância frontal em mm
  distancia[0] = (sensor_1.dist() * 10);

  //Liga e desliga o sensor 2
  digitalWrite(Trig2_PIN, HIGH);
  if((agora - antes) == 1)
  {
    digitalWrite(Trig2_PIN, LOW);
    antes = agora;
  }
  //Aquisição da distância esquerda em mm
  distancia[1] = (sensor_2.dist() * 10);

  //Liga e desliga o sensor 3
  digitalWrite(Trig3_PIN, HIGH);
  if((agora - antes) == 1)
  {
    digitalWrite(Trig3_PIN, LOW);
    antes = agora;
  }
  //Aquisição da distância direita em mm
  distancia[2] = (sensor_3.dist() * 10);
  
  if((agora - antes) > 100)
  {
      Serial.println(agora - antes);

      //Liga os dois motores no sentido horário
      ligaMotor(100, 0, 1, 100, 1, 0);

      antes = agora;
  }

  //Movimentação lateral
  switch(verificar(distancia[1], distancia[2], distancia[0]))
  {
    //Ir para trás
    case 0:
    //Ligar os dois motores no sentido anti-horário
    ligaMotor(150, 1, 0, 150, 0, 1);

    //Tempo para o desligamento
    if((agora - antes) > 100)
    {
      Serial.println(agora - antes);

      //Liga os dois motores no sentido horário
      ligaMotor(100, 0, 1, 100, 1, 0);

      antes = agora;
    }
    break;

    //Ir para direita
    case 1:
    //Liga o motor esquerdo no sentido horário e o direito no sentido anti-horário
    ligaMotor(200, 0, 1, 100, 0, 1);

    //Tempo para o desligamento
    if((agora - antes) > 100)
    {
      Serial.println(agora - antes);

      //Liga os dois motores no sentido horário
      ligaMotor(100, 0, 1, 100, 1, 0);

      antes = agora;
    }
    break;

    //Ir para esquerda
    case 2:
    //Liga o motor esquerdo no sentido anti-horário e o direito no sentido horário
    ligaMotor(100, 1, 0, 200, 1, 0);
   
    //Tempo para o desligamento
    if((agora - antes) > 100)
    {
      Serial.println(agora - antes);

      //Liga os dois motores no sentido horário
      ligaMotor(100, 0, 1, 100, 1, 0);

      antes = agora;
    }
    break;

    default:
    Serial.println("Nada");
  }
}


//----------FUNÇÕES----------//
//Função que verifica as distâncias laterais e frontal
int verificar(float x, float y, float z)
{
  float dist_esq = x - Dis_Sensor_Esq, 
        dist_dir = y - Dis_Sensor_Dir;

  float dist_frente = z;

  //Ir para trás
  if(dist_frente < Tolerancia && dist_frente > 0 || (dist_frente < Tolerancia && dist_frente > 0 && dist_esq < Tolerancia && dist_esq > 0 && dist_dir < Tolerancia && dist_dir > 0))
  {
    Serial.println("Pra trás!!");
    Serial.println(dist_frente);
    return 0;
  }
  //Verifica o lado esquerdo (Ir para direita)
  else if(dist_esq < Tolerancia && dist_esq > 0)
  {
    Serial.println("Vá pra direita");
    Serial.println(dist_esq);
    return 1;
  }
  //Verifica o lado direito (Ir para esquerda)
  else if(dist_dir < Tolerancia && dist_dir > 0)
  {
    Serial.println("Vá pra esquerda");    Serial.println(dist_dir);
    return 2;
  }
  //Não fazer nada
  else
  {
    return 3;
  }
}

//Função que controla os motores
void ligaMotor(int v1, int a1, int b1, int v2, int a2, int b2)
{
  analogWrite(Vel_Motor1, v1);
  digitalWrite(Mla1, a1);
  digitalWrite(Mlb1, b1);  	
  analogWrite(Vel_Motor2, v2);
  digitalWrite(Mla2, a2);
  digitalWrite(Mlb2, b2);
}
