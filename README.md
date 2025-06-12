<!DOCTYPE html>
<html lang="pt-BR">
<head>
<meta charset="UTF-8" />
<meta name="viewport" content="width=device-width, initial-scale=1" />

</head>
<body>
  <h1><span class="emoji">🌱</span>Projeto de Monitoramento de Estufa com Arduino</h1>

  <section>
    <h2>Componentes Necessários <span class="emoji">🛠️</span></h2>
    <ul>
      <li>Arduino (Uno, Nano, etc.)</li>
      <li>Sensor de Temperatura e Umidade DHT11</li>
      <li>Sensor de Umidade do Solo</li>
      <li>Sensor de Chuva</li>
      <li>Relés (para controlar ventilador, lâmpada e bomba)</li>
      <li>Display OLED (128x64)</li>
      <li>Fios de conexão</li>
      <li>Protoboard (opcional)</li>
      <li>Buzzer ou LED (para alertas)</li>
    </ul>
  </section>

  <section>
    <h2>Bibliotecas Utilizadas <span class="emoji">📚</span></h2>
    <ul>
      <li><code>Adafruit_Sensor.h</code>: Biblioteca para trabalhar com sensores.</li>
      <li><code>DHT.h</code>: Biblioteca para o sensor DHT11.</li>
      <li><code>Wire.h</code>: Biblioteca para comunicação I2C.</li>
      <li><code>U8glib.h</code>: Biblioteca para displays OLED.</li>
    </ul>
  </section>

  <section>
    <h2>Conexões <span class="emoji">🔌</span></h2>
    <ol>
      <li><strong>DHT11:</strong><br/>
        Pino de dados no pino 2 do Arduino.<br/>
        VCC no 5V do Arduino.<br/>
        GND no GND do Arduino.
      </li>
      <li><strong>Sensores:</strong><br/>
        Sensor de chuva no pino A0.<br/>
        Sensor de umidade do solo no pino A1.
      </li>
      <li><strong>Relés:</strong><br/>
        Ventilador no pino 3.<br/>
        Lâmpada no pino 4.<br/>
        Bomba no pino 5.
      </li>
      <li><strong>Botão:</strong><br/>
        No pino 6, com resistor pull-up.
      </li>
      <li><strong>Alerta:</strong><br/>
        Buzzer ou LED no pino 7.
      </li>
      <li><strong>Display OLED:</strong><br/>
        Conecte SDA e SCL do display aos pinos correspondentes do Arduino (geralmente A4 e A5 no Uno).
      </li>
    </ol>
  </section>

  <section>
    <h2>Configuração do Código <span class="emoji">💻</span></h2>
    <ol>
      <li><strong>Instalação das Bibliotecas:</strong><br/>
      Instale as bibliotecas necessárias via Gerenciador de Bibliotecas da Arduino IDE.</li>
      <li><strong>Código:</strong><br/>
      Copie e cole o código principal no arquivo <code>main.ino</code>.</li>
      <li><strong>Configurações:</strong><br/>
      Ajuste os valores <code>temp_max</code> e <code>temp_min</code> para definir os limites de temperatura desejados.</li>
    </ol>
  </section>

  <section>
    <h2>Funcionamento do Projeto <span class="emoji">🚀</span></h2>
    <p>O sistema lê temperatura e umidade interna da estufa, além de umidade do solo e presença de chuva. Ele controla automaticamente os relés para ventilador e lâmpada conforme essas leituras.</p>
    <p>As informações ficam visíveis no display OLED, e você pode navegar entre telas usando um botão. Um alerta sonoro ou visual é acionado se a temperatura ultrapassar limites ou a umidade do solo estiver crítica.</p>
  </section>

  <section>
    <h2>Uso do Projeto <span class="emoji">🏡</span></h2>
    <ol>
      <li><strong>Inicialização:</strong> Após carregar o código, o display exibirá uma tela de inicialização com o nome do projeto.</li>
      <li><strong>Navegação:</strong> Pressione o botão para alternar entre as telas com diferentes informações em tempo real.</li>
      <li><strong>Monitoramento:</strong> Observe as leituras exibidas e os alertas. O sistema ajusta automaticamente os dispositivos conforme necessário.</li>
    </ol>
  </section>

  <section>
    <h2>Contribuições <span class="emoji">🤝</span></h2>
    <p>Sinta-se à vontade para contribuir com melhorias, correções de bugs ou novas funcionalidades. Faça um fork do repositório, modifique o código e envie um pull request!</p>
  </section>

  <section>
    <h2>Licença <span class="emoji">📄</span></h2>
    <p>Este projeto é open source e pode ser utilizado e modificado livremente. Por favor, mantenha a atribuição ao autor original.</p>
  </section>

  <footer>
    <p>Esperamos que este projeto ajude a manter suas plantas saudáveis e felizes! <span class="emoji">🌿😊</span></p>
  </footer>
</body>
</html>

