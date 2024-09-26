#include <Adafruit_Sensor.h>  // Biblioteca para trabalhar com sensores
#include <DHT.h>              // Biblioteca para o sensor de temperatura e umidade DHT
#include <Wire.h>             // Biblioteca para comunicação I2C
#include <U8glib.h>           // Biblioteca para displays OLED

// Definições dos pinos dos sensores, relés e botão
#define DHTPIN 2
#define DHTTYPE DHT11
#define RAIN_SENSOR_PIN A0
#define SOIL_MOISTURE_SENSOR_PIN A1
#define FAN_RELAY_PIN 3
#define BULB_RELAY_PIN 4
#define PUMP_RELAY_PIN 5
#define BUTTON_PIN 6  // Pino do botão para trocar as telas
#define ALERT_PIN 7   // Pino para alerta (buzzer ou LED)

// Configuração do display OLED
#define OLED_RESET 4
U8GLIB_SSD1306_128X64 display(OLED_RESET);

DHT dht(DHTPIN, DHTTYPE);

// Variáveis para armazenar a temperatura máxima desejada e os dados externos
float temp_max = 25.0;
float temp_min = 20.0;
float externalTemperature = 0.0;
float externalHumidity = 0.0;

// Valores calibrados dos sensores
const int RAIN_SENSOR_MIN = 1023;
const int RAIN_SENSOR_MAX = 350;
const int SOIL_MOISTURE_MIN = 1023;
const int SOIL_MOISTURE_MAX = 500;

int currentScreen = 0;  // Variável para rastrear a tela atual
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

bool relaysInitialized = false; // Variável para verificar se os relés já foram inicializados
unsigned long relayInitTime = 0;
const unsigned long relayDelay = 5000; // Atraso de 5 segundos para inicialização dos relés

// Variáveis para controle de tempo do display
unsigned long lastInteractionTime = 0;
const unsigned long displayTimeout = 30000; // 30 segundos

// Média móvel simples para suavização das leituras de umidade do solo
const int numReadings = 10;
int readings[numReadings];
int readIndex = 0;
int total = 0;
int average = 0;

void setup() {
    Serial.begin(9600);
    pinMode(RAIN_SENSOR_PIN, INPUT);
    pinMode(SOIL_MOISTURE_SENSOR_PIN, INPUT);
    pinMode(FAN_RELAY_PIN, OUTPUT);
    pinMode(BULB_RELAY_PIN, OUTPUT);
    pinMode(PUMP_RELAY_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);  // Configura o botão como entrada com pull-up
    pinMode(ALERT_PIN, OUTPUT);         // Configura o pino de alerta

    dht.begin();
    display.begin();

    // Inicializa as leituras de solo
    for (int i = 0; i < numReadings; i++) {
        readings[i] = 0;
    }

    // Inverter a orientação do display
    display.setRot180();

    // Inicializa os relés desligados (ajuste dependendo se os relés são ativos em nível baixo ou alto)
    digitalWrite(FAN_RELAY_PIN, HIGH);   // Se o relé for ativo em nível baixo, inicie com HIGH
    digitalWrite(BULB_RELAY_PIN, HIGH);  // Se o relé for ativo em nível baixo, inicie com HIGH
    digitalWrite(PUMP_RELAY_PIN, HIGH);  // Se o relé for ativo em nível baixo, inicie com HIGH

    displayInitialization();
    delay(3000);
    relayInitTime = millis(); // Armazena o tempo de início
}

// Função para desenhar informações no display OLED com valor raw
void draw(String label, float value, String unit, int raw = -1) {
    display.firstPage();
    do {
        display.setFont(u8g_font_6x10);
        display.setPrintPos(0, 20);
        display.print(label);

        display.setFont(u8g_font_fub20);
        display.setPrintPos(0, 50);
        display.print(value);

        int xOffset = String(value).length() * 15;
        display.setPrintPos(xOffset, 50);
        display.print(unit);

        if (raw >= 0) { // Se houver um valor raw, exibe-o em uma posição menor
            display.setFont(u8g_font_6x10);
            display.setPrintPos(0, 60); // Posição na linha de baixo
            display.print("Raw: ");
            display.print(raw);
        }
    } while (display.nextPage());
}

// Função para exibir todas as informações na tela inicial
void displayAllData(float internalTemp, float internalHum, float externalTemp, float externalHum, float rainPerc, int rainRaw, float soilPerc, int soilRaw) {
    display.firstPage();
    do {
        display.setFont(u8g_font_6x10);

        display.setPrintPos(0, 10);
        display.print("Temp. Estufa: ");
        display.print(internalTemp);
        display.print(" C");

        display.setPrintPos(0, 20);
        display.print("Hum. Estufa: ");
        display.print(internalHum);
        display.print(" %");

        display.setPrintPos(0, 30);
        display.print("Temp. Ext.: ");
        display.print(externalTemp);
        display.print(" C");

        display.setPrintPos(0, 40);
        display.print("Hum. Ext.: ");
        display.print(externalHum);
        display.print(" %");

        display.setPrintPos(0, 50);
        display.print("Chuva: ");
        display.print(rainPerc);
        display.print(" % (");
        display.print(rainRaw);
        display.print(")");

        display.setPrintPos(0, 60);
        display.print("Solo: ");
        display.print(soilPerc);
        display.print(" % (");
        display.print(soilRaw);
    } while (display.nextPage());
}

// Função para exibir a inicialização no display OLED
void displayInitialization() {
    display.firstPage();
    do {
        display.setFont(u8g_font_fub20);
        display.setPrintPos(0, 20);
        display.print("C.E.S.A");

        display.setFont(u8g_font_6x10);
        display.setPrintPos(0, 40);
        display.print("Agrinho 2024 Robotica");
    } while (display.nextPage());
}

// Função para processar os dados recebidos do ESP01
void processSerialInput(String input) {
    int tempIndex = input.indexOf("TEMP:");
    int humIndex = input.indexOf("HUM:");

    if (tempIndex != -1 && humIndex != -1) {
        externalTemperature = input.substring(tempIndex + 5, input.indexOf(";", tempIndex)).toFloat();
        externalHumidity = input.substring(humIndex + 4, input.indexOf(";", humIndex)).toFloat();
    } else {
        Serial.println("Erro ao processar dados do ESP01");
    }
}

// Função para verificar o estado do botão e alternar entre as telas
void checkButton() {
    int buttonState = digitalRead(BUTTON_PIN);

    if (buttonState == LOW) {
        unsigned long currentTime = millis();
        if (currentTime - lastDebounceTime > debounceDelay) {
            currentScreen++;
            if (currentScreen > 6) { // Se passar do número de telas, reinicia para a tela inicial
                currentScreen = 0;
            }
            lastDebounceTime = currentTime;
            lastInteractionTime = millis(); // Registra a última interação com o botão
        }
    }
}

// Função para verificar se o display deve ser desligado por inatividade
void checkDisplayTimeout() {
    if (millis() - lastInteractionTime > displayTimeout) {
        display.sleepOn();
    } else {
        display.sleepOff();
    }
}

void loop() {
    float internalTemperature = dht.readTemperature();
    float internalHumidity = dht.readHumidity();

    if (isnan(internalTemperature) || isnan(internalHumidity)) {
        Serial.println("Erro ao ler o sensor DHT11");
        return;  // Sai da função loop se os dados forem inválidos
    }

    int rainSensorValue = analogRead(RAIN_SENSOR_PIN);

    // Média móvel simples para suavizar leituras de umidade do solo
    total = total - readings[readIndex];  // Subtrai a leitura mais antiga
    readings[readIndex] = analogRead(SOIL_MOISTURE_SENSOR_PIN);  // Lê o novo valor
    total = total + readings[readIndex];  // Adiciona a nova leitura
    readIndex = (readIndex + 1) % numReadings;  // Avança para o próximo índice
    average = total / numReadings;  // Calcula a média

    // Inicialização dos relés após o tempo definido
    if (!relaysInitialized && millis() - relayInitTime > relayDelay) {
        relaysInitialized = true;
    }

    if (relaysInitialized) {
        // Controle dos relés com histerese
        if (internalTemperature > temp_max) {
            digitalWrite(FAN_RELAY_PIN, LOW);  // Liga ventilador
        } else if (internalTemperature < temp_min) {
            digitalWrite(FAN_RELAY_PIN, HIGH);  // Desliga ventilador
        }

        // Controle da lâmpada com base na umidade
        if (internalHumidity < 50.0) {
            digitalWrite(BULB_RELAY_PIN, LOW);  // Liga lâmpada
        } else {
            digitalWrite(BULB_RELAY_PIN, HIGH);  // Desliga lâmpada
        }
    }

    // Cálculo dos percentuais com base nas leituras
    float rainPercentage = map(rainSensorValue, RAIN_SENSOR_MIN, RAIN_SENSOR_MAX, 0, 100);
    float soilMoisturePercentage = map(average, SOIL_MOISTURE_MIN, SOIL_MOISTURE_MAX, 0, 100);

    // Controle de alerta
    if (internalTemperature > temp_max || soilMoisturePercentage < 30) {
        digitalWrite(ALERT_PIN, HIGH);  // Liga alerta
    } else {
        digitalWrite(ALERT_PIN, LOW);   // Desliga alerta
    }

    // Processamento dos dados recebidos via serial
    if (Serial.available() > 0) {
        String input = Serial.readString();
        processSerialInput(input);
    }

    // Verifica se o botão foi pressionado
    checkButton();

    // Exibe as telas correspondentes
    switch (currentScreen) {
        case 0:
            displayAllData(internalTemperature, internalHumidity, externalTemperature, externalHumidity, rainPercentage, rainSensorValue, soilMoisturePercentage, average);
            break;
        case 1:
            draw("Temp. Estufa", internalTemperature, "C");
            break;
        case 2:
            draw("Hum. Estufa", internalHumidity, "%");
            break;
        case 3:
            draw("Temp. Ext.", externalTemperature, "C");
            break;
        case 4:
            draw("Hum. Ext.", externalHumidity, "%");
            break;
        case 5:
            draw("Chuva", rainPercentage, "%", rainSensorValue);
            break;
        case 6:
            draw("Solo", soilMoisturePercentage, "%", average);
            break;
    }

    // Verifica o timeout do display
    checkDisplayTimeout();
}