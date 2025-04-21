#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "./lib/ssd1306.h"
#include "./lib/font.h"
#include "./lib/matrix.h"
#include "hardware/pio.h"
#include "Smart_Switching.pio.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"

//---------------------------------------------------
// DEFINES
//---------------------------------------------------
#define I2C_PORT        i2c1
#define I2C_SDA         14
#define I2C_SCL         15
#define ENDERECO        0x3C
#define LED_RED         13
#define LED_BLUE        12
#define BUTTON_A        5
#define BUTTON_B        6     
#define JOYSTICK_X      27  
#define JOYSTICK_Y      26  
#define JOYSTICK_BT     22 
#define BUZZER          21
#define LED_MATRIX      7
#define TOTAL_LEDS      25
#define MAX_LED_VALUE   60

//---------------------------------------------------
// VARIÁVEIS GLOBAIS
//---------------------------------------------------
// ADC
uint16_t adc_value_x;
uint16_t adc_value_y;  
uint16_t center_value = 2000;  // Valor medido experimentalmente

// Display SSD1306
ssd1306_t ssd; // Display de 128 x 64

// PWM BUZZER
uint slice; 
const uint16_t wrap = 1000;   // Valor de wrap do PWM

//Flags
uint8_t flag_tensao = 0; //Condições aceitáveis
bool flag_display = 0;
bool flag_botao = 1;

//---------------------------------------------------
// PROTOTIPAÇÃO
//---------------------------------------------------
void init_pinos(void);
static void callback_button(uint gpio, uint32_t events);
void control_display(int adc_value_x, int adc_value_y);
void analise_tensao(int adc_y); 
void ihm_dispay(int adc_y, int flag_tensao);
void seleciona_display(bool flag, int adc_x, int adc_y, int flag_tensao);
void exibir_matriz(PIO pio, uint sm, int flag_tensao);
void dispara_buzzer(uint slice);

int main()
{
    stdio_init_all();
    init_pinos();

    // Configura o PWM para o BUZZER 
    pwm_set_enabled(slice, true);
    pwm_set_wrap(slice, wrap);

    // Configura ISR para os botões 
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, callback_button);
    gpio_set_irq_enabled(BUTTON_B, GPIO_IRQ_EDGE_FALL, true);

    // Inicializa o ADC e configura os pinos correspondentes
    adc_init();
    adc_gpio_init(JOYSTICK_X);
    adc_gpio_init(JOYSTICK_Y); 

    // Configura o PIO para controlar a matriz de LEDs
    PIO pio = pio0;
    bool clk = set_sys_clock_khz(128000, false);
    uint offset = pio_add_program(pio, &Smart_Switching_program);
    uint sm_pio = pio_claim_unused_sm(pio, true);
    Smart_Switching_program_init(pio, sm_pio, offset, LED_MATRIX);

    // Inicializa o display via I2C
    i2c_init(I2C_PORT, 400000); // 0.4 MHz
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ENDERECO, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    while (true) {
        // Leitura dos ADCs
        adc_select_input(1); 
        adc_value_x = adc_read();
        adc_select_input(0);
        adc_value_y = adc_read();              
        seleciona_display(flag_display, adc_value_x, adc_value_y, flag_tensao);
        analise_tensao(adc_value_y);
        exibir_matriz(pio, sm_pio, flag_tensao);
        dispara_buzzer(slice);
    }
}

//---------------------------------------------------
// FUNÇÃO DE INICIALIZAÇÃO DOS PINOS
//---------------------------------------------------
void init_pinos(void) {
    // Configura o BUZZER para PWM
    gpio_set_function(BUZZER, GPIO_FUNC_PWM);
    slice = pwm_gpio_to_slice_num(BUZZER);

    // Inicializa LEDs 
    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);

    gpio_init(LED_BLUE);
    gpio_set_dir(LED_BLUE, GPIO_OUT);
  


    // Inicializa os botões com pull-up
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);

    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);

    gpio_init(JOYSTICK_BT);
    gpio_set_dir(JOYSTICK_BT, GPIO_IN);
    gpio_pull_up(JOYSTICK_BT);

    // Configura I2C para o display
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

//---------------------------------------------------
// FUNÇÃO DE CALLBACK DOS BOTÕES
//---------------------------------------------------
static void callback_button(uint gpio, uint32_t events) {
    static absolute_time_t last_time_A = 0;
    static absolute_time_t last_time_B = 0;
    static absolute_time_t last_time_J = 0;
    absolute_time_t now = get_absolute_time();

    if (gpio == BUTTON_A) {
        if (absolute_time_diff_us(last_time_A, now) > 200000) {
            flag_display = !flag_display;
            last_time_A = now;
        }
    } else if (gpio == BUTTON_B) {
        if (absolute_time_diff_us(last_time_B, now) > 200000) {
            flag_botao = !flag_botao;
            last_time_B = now;
        }
}}

//---------------------------------------------------
// FUNÇÃO DE ANALISE DE TENSÃO
//---------------------------------------------------
void analise_tensao(int adc_y) {
    float margem_max = 1.20; //20% 
    float margem_mim = 0.70; //30%
    // Elimina ruídos: se a diferença for menor que a tolerância, usa o valor central
    uint16_t tolerancia = 100;
    uint16_t leitura_y = (abs(adc_y - center_value) > tolerancia) ? adc_y : center_value;
    // Flag:
    // 0 -> Condicao Normal
    // 1 -> Sobretensão
    // 2 -> Subtensão
    // 3 -> Queda de Energia
    if (leitura_y < tolerancia){
        printf("Queda de Energia Detctada!\n");
        flag_tensao = 3;
    }else if (leitura_y < margem_mim * center_value ){
        printf("Subtensão Detctada!\n");
        flag_tensao = 2;        
    }else if (leitura_y > margem_max * center_value ){
        printf("Sobretensão Detctada!\n");
        flag_tensao = 1;      
    }else{
        printf("Condições Normais! \n");
        flag_tensao = 0;          
    }
    if ((flag_tensao > 0) && flag_botao ){
        gpio_put(LED_RED,1);
        gpio_put(LED_BLUE,0);
    }else {
        gpio_put(LED_BLUE,1);
        gpio_put(LED_RED,0);
    }

}
//---------------------------------------------------
// FUNÇÃO DE CONTROLE DISPLAY
//---------------------------------------------------

void control_display(int adc_value_x, int adc_value_y){

    uint8_t top = (64 - 8) - (adc_value_y * 0.0137);
    uint8_t  left = adc_value_x * 0.0294;
    ssd1306_fill(&ssd, false); //Apaga todos pixels

    //Borda grossa
    ssd1306_rect(&ssd, 1, 1, 126, 62, 1, 1); // Desenha um retângulo
    ssd1306_rect(&ssd, 4, 4, 120, 56, 0, 1); // Desenha um retângulo

    ssd1306_rect(&ssd, top, left, 8, 8, 1, 1); // Desenha o quadrado
    ssd1306_send_data(&ssd); // Atualiza o display
 }

 void ihm_dispay(int adc_y, int flag_tensao){
    uint16_t tolerancia = 100;
    uint16_t leitura_y = (abs(adc_y - center_value) > tolerancia) ? adc_y : center_value;
    char exibir_tensao[4];
    uint16_t tensao = (uint16_t)leitura_y * 100 / center_value;
    sprintf(exibir_tensao, "%d", tensao);

    char msg[20];
    switch (flag_tensao)
    {
    case 1:
        strcpy(msg, "SOBRETENSAO");
        break;
    case 2:
        strcpy(msg, "SUBTENSAO");
        break;

    case 3:
        strcpy(msg, "QUEDA");
        break;
    
    default:
        strcpy(msg, "REDE ADEQUADA");
        break;
    }
    ssd1306_fill(&ssd, false);
    //Borda grossa
    ssd1306_rect(&ssd, 1, 1, 126, 62, 1, 1); // Desenha um retângulo
    ssd1306_rect(&ssd, 4, 4, 120, 56, 0, 1); // Desenha um retângulo
    ssd1306_draw_string(&ssd, "TENSAO = ", 4, 14);
    ssd1306_draw_string(&ssd, exibir_tensao, 88, 14);
    ssd1306_draw_char(&ssd, '%', 112, 14);
    ssd1306_draw_string(&ssd, msg, 6, 30);
    if (flag_tensao > 0 && flag_botao){
        ssd1306_draw_string(&ssd, "BATERIAS ON", 6, 45);
    }else{
        ssd1306_draw_string(&ssd, "BATERIAS OFF", 6, 45);
    }
    ssd1306_send_data(&ssd);
 }
void seleciona_display(bool flag, int adc_x, int adc_y, int flag_tensao){
    if (flag){
        ihm_dispay(adc_y, flag_tensao);        
    }
    else{
        control_display(adc_x, adc_y);
    }
}
void exibir_matriz(PIO pio, uint sm, int flag_tensao){
        for (int i = 0; i < 25; i++)
        {
            pio_sm_put_blocking(pio, sm, icones[flag_tensao][i]);
        }
}

void dispara_buzzer(uint slice) {
    // Se o volume estiver nos limites de segurança, emite alarme:
    if ((flag_tensao > 0) && flag_botao) {
        // Alarme de emergência: tom mais agudo
        uint16_t level = 500;   // Duty cycle 50% (em wrap 1000)
        float div = 100.0f;     // Divisor para frequência mais alta
        pwm_set_gpio_level(BUZZER, level);
        pwm_set_clkdiv(slice, div);
    } else if (flag_tensao > 0) {
        // Alarme de baixa emergência: tom mais grave
        uint16_t level = 500;
        float div = 250.0f;     // Divisor para frequência mais baixa
        pwm_set_gpio_level(BUZZER, level);
        pwm_set_clkdiv(slice, div);
    } else {
        // Sem alarme: buzzer desligado
        pwm_set_gpio_level(BUZZER, 0);
    }
}