
#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>

volatile uint8_t cont = 0;
volatile bool conv = false;

ISR(TIMER0_OVF_vect)
{
  cont++;
  if(cont == 16){ // Habilita a Conversão a uma frenquencia 1000hz
    conv = true;
    cont = 0;
  }
}

int main(){

    uint16_t ADCSensor = 0;
    
    // valores iniciais dos sinais PWM
    OCR1A = 0; // FAN
    
    // configuracao dos PWMs
    bool Inicia = true;
    //  FAST PWM 10 bits (Modo 7) sem invers�o
    
    // PWM do FAN
    // Scale 1/256 CSx2 CSx1 62,5kHz
    TCCR1A = _BV(COM1A1) | _BV(WGM10) | _BV(WGM11);
    TCCR1B = _BV(CS12) |_BV(CS11) | _BV(WGM12);

    // CONTA COM O CONTADOR 0
    TCCR0A |= 0x00;//_BV(WGM01)|_BV(WGM00);
	TCCR0B |= _BV(CS02) | _BV(CS00);;  // 1/1024 prescale
	TIMSK0 |= _BV(TOIE0);   // Enable timer0 overflow interrupt(TOIE0)
    sei();//Habilita interrupção z

    // PB1/OC1A como saida
    DDRB |= _BV(PORTB1);
    
    //Configuracao do AD
    ADMUX  |= _BV(REFS0); //Utiliza VCC como referência
    ADCSRA |= _BV(ADEN)|_BV(ADPS2)|_BV(ADPS1)|_BV(ADPS0);  //Habilita o AD e utiliza prescale selection de 128
    
    // VAI USAR ADC0 para TempIN, ADC2 para TempOUT e ADC4 para Umidade

    //Configuracao de botao
    DDRB &= 0b11111110; // PINO PB0 como entrada
    PORTB |= 0x00; // HiZ

    //Configuracao de LEDS
    DDRD |= _BV(PORTD3); //PD3
    PORTD &= 0x00; // Mantem tudo apagado
    PORTD |= _BV(PORTD3);
    
    while(1){
        // Quando habilitado, faz as conversoes, depois que acabar desabilita e espera
        // o contador para poder fazer as conversoes novamente.
        if (conv == true)
        {
            ADCSRA |= _BV(ADSC); 
            while(!(ADCSRA & 0x10));
            ADCSensor = ADC;
            conv = false;
        }        
        if(Inicia){
            // quando habilitado o botao
            OCR1A = ADCSensor;
            PORTD |=0x00;                
        }else{
            OCR1A = 0;
            PORTD |= _BV(PORTD3)
        }		
        if(PINB & 0x01){
            if(Inicia == true){
                Inicia = false;
            }else{
                Inicia = true;
            }
        }    
    }
}