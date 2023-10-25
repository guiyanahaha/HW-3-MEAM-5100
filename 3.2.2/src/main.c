/* Name: main.c
 * Author: <Yining Guo>
 * Penn ID: <80047379>
 * Lab 3 Waldo code part 3.2.3
 */

#include "MEAM_general.h"
#include "m_usb.h"
 
// define ADC ID
#define ADC0D 0
#define ADC1D 1
#define ADC4D 2
#define ADC5D 3
#define ADC6D 4
#define ADC7D 5
#define ADC8D 6
#define ADC9D 7
#define ADC10D 8
#define ADC11D 9
#define ADC12D 10
#define ADC13D 11

#define Servo_pin1 PB6
#define Servo_pin2 PB7
#define Servo_pin3 PB5

int current_idx = 1;

// The custom string_copy below is suggested by ChatGPT:
// Custom string copy function
void string_copy(char *dest, const char *src) {
    while ((*dest++ = *src++)) {}
}

// Function to set up the ADC for a specified port
void ADC_setup(int ADC_index) {
    // preallocate variables and constants for ADC pins
    const int PORT_id[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    const char ADC_name[12][10] = {"ADC0D", "ADC1D", "ADC4D", "ADC5D", "ADC6D", "ADC7D", "ADC8D", "ADC9D", "ADC10D", "ADC11D", "ADC12D", "ADC13D"};
    char PORT[10] = "ADC0D";
    int i = 0;

    //set the system clock speed to 16Mhz
    _clockdivide(0);

    // Set voltage reference
    clear(ADMUX,REFS1);	
    set(ADMUX,REFS0);

    // Set ADC Prescaler to /128
    set(ADCSRA,ADPS2);	
    set(ADCSRA,ADPS1);	
    set(ADCSRA,ADPS0);

    // Clear previous ADC port bits
    clear(ADCSRB,MUX5);	
    clear(ADMUX,MUX2);	
    clear(ADMUX,MUX1);	
    clear(ADMUX,MUX0);

    // setup ADC pins according to user input
    for (i=0; i< 12; i++) {
        if (ADC_index == PORT_id[i]){
            string_copy(PORT, ADC_name[i]);    //assign ports to ADC
            if (i < 6){
                clear(DIDR0, atoi(PORT));
                if (i >= 2){
                    set(ADMUX,MUX2);
                }
                if (i >= 4){
                    set(ADMUX,MUX1);
                }
                if (i == 1 || i == 3 || i == 5){
                    set(ADMUX,MUX0);
                }
            } else if (i >= 6 && i != 10){

                clear(DIDR0, atoi(PORT));
                set(ADCSRB,MUX5);	
                if (i >= 10){
                    set(ADMUX,MUX2);
                }
                if (i == 8 || i == 9 || i == 12){
                    set(ADMUX,MUX1);
                }
                if (i == 7 || i == 9 || i == 11 || i == 12){
                    set(ADMUX,MUX0);
                }
            } else if (i >= 6 && i == 10){
                set(DIDR0, atoi(PORT));
                set(ADCSRB,MUX5);	
                set(ADMUX,MUX2);
            }
        }
    }
}

// Function to read the ADC value for the configured port
void readADC() {

    // Enable ADC
    set(ADCSRA, ADEN);
    // Begin conversion
    set(ADCSRA, ADSC);

    // Wait for the conversion to complete
    while (1) {
        if (bit_is_set(ADCSRA,ADIF)){ // if ADC conversion is complete
            set(ADCSRA, ADIF);  // reset the flag

            m_usb_tx_string("potentiometer ");
            m_usb_tx_uint(current_idx);
            m_usb_tx_string(" angle:");
            m_usb_tx_uint(ADC);  //print angle values of 280 degree potentiometer 

            m_usb_tx_string("\n");
            if (current_idx == 1) {
                OCR1B = 100.0 +ADC* 1250.0/1024.0; // set duty cycle as a larger range to allow deviation
            } else if (current_idx == 2){
                OCR1C = 100.0 +ADC* 1250.0/1024.0; // set duty cycle as a larger range to allow deviation
            } else if (current_idx == 3){
                OCR1A = 100.0 +ADC* 1250.0/1024.0;
            }
            current_idx = current_idx % 3 + 1;

            set(ADCSRA, ADSC); // start conversion again
            break;
        }
    }
}

void setup_pwm(servo_pin) {
    // Set Servo pin as output
    set(DDRB, servo_pin);

    if (servo_pin == PB6){
        // Set Timer Mode: Fast PWM, non-inverting mode
        set(TCCR1A, COM1B1); // clear at OCR1B, set at rollover
        clear(TCCR1A, COM1B0);
    } else if (servo_pin == PB7) {
        // Set Timer Mode: Fast PWM, non-inverting mode
        set(TCCR1A, COM1C1); // clear at OCR1C, set at rollover
        clear(TCCR1A, COM1C0);
    } else if (servo_pin == PB5) {
        // Set Timer Mode: Fast PWM, non-inverting mode
        set(TCCR1A, COM1A1); // clear at OCR1C, set at rollover
        clear(TCCR1A, COM1A0);
    }

    set(TCCR1B, CS10);
    set(TCCR1B, CS11); // prescale /64

    clear(TCCR1A, WGM10);
    set(TCCR1A, WGM11);
    set(TCCR1B, WGM12);
    set(TCCR1B, WGM13); // set timer to mode 14

    ICR1 = 5000;    //set the frequency of PWM at 50Hz
}


int main(void)
{
    // Initialize USB
    m_usb_init();
    // Call subroutines
    while(1){
        setup_pwm(Servo_pin1);
        ADC_setup(ADC0D); //reading from potentiometer 1
        readADC();

        setup_pwm(Servo_pin2);
        ADC_setup(ADC1D); //reading from potentiometer 2
        readADC();

        setup_pwm(Servo_pin3);
        ADC_setup(ADC4D); //reading from potentiometer 2
        readADC();
    }
    return 0;
}