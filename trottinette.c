// ===================================================================
// Trottinette
// - PIC16F877A  - simulation
// ===================================================================

unsigned int accel_value, distance_value, speed_percent, current_speed;
unsigned short timer_100ms, braking;
char lcd_text[16];

unsigned short security = 1;
unsigned short rb0_last = 1;
unsigned short last_rb4_state = 1;
unsigned short charge_mode = 0;
unsigned short charge_done = 0;
unsigned short charge_timer_active = 0;
unsigned short rb4_press_count = 0;
unsigned int rb4_press_timeout = 0;
unsigned short show_history = 0;


unsigned short default_delay_active = 0;
unsigned int default_delay_counter = 0;

volatile unsigned short rb4_pressed = 0;
volatile unsigned short rb5_pressed = 0;
volatile unsigned int timer0_count = 0;
volatile unsigned short charge_complete = 0;

unsigned short dummy;

//
sbit LCD_RS at RC0_bit;
sbit LCD_EN at RC1_bit;
sbit LCD_D4 at RC4_bit;
sbit LCD_D5 at RC5_bit;
sbit LCD_D6 at RC6_bit;
sbit LCD_D7 at RC7_bit;

sbit LCD_RS_Direction at TRISC0_bit;
sbit LCD_EN_Direction at TRISC1_bit;
sbit LCD_D4_Direction at TRISC4_bit;
sbit LCD_D5_Direction at TRISC5_bit;
sbit LCD_D6_Direction at TRISC6_bit;
sbit LCD_D7_Direction at TRISC7_bit;

// ================================================================
// INTERRUPT
// ================================================================
void interrupt() {

    if (TMR0IF_bit) {
        TMR0 = 6;
        timer0_count++;
        if (timer0_count >= 40) {
            charge_complete = 1;
            TMR0IE_bit = 0;
        }
        TMR0IF_bit = 0;
    }

    if (RBIF_bit) {
        dummy = PORTB;

        // RB4 - Charge button
        if (RB4_bit && !last_rb4_state) {
            rb4_pressed = 1;
        }
        last_rb4_state = RB4_bit;

        // RB5 - History button
        if (RB5_bit) {
            rb5_pressed = 1;
        }

        RBIF_bit = 0;
    }
}

// ================================================================
// TIMER
// ================================================================
void Timer0_Init() {
    OPTION_REG = 0b10000111;
    TMR0 = 6;
    TMR0IE_bit = 0;
}

void Timer0_Start() {
    timer0_count = 0;
    charge_complete = 0;
    TMR0 = 6;
    TMR0IE_bit = 1;
}

// ================================================================
// EMERGENCY STOP (FREINAGE)
// ================================================================
void Emergency_Stop() {

    braking = 1;
    RB7_bit = 1;  // Red LED ON
    RB1_bit = 1;  // Turn on RB1 during braking

    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Out(1, 1, "OBSTACLE !");
    Lcd_Out(2, 1, "FREINAGE !");
    Delay_ms(50);

    while (current_speed > 0) {
        current_speed = (current_speed >= 20) ? current_speed - 20 : 0;

        RD0_bit = 0;
        RD1_bit = 0;
        PWM1_Set_Duty(current_speed * 255 / 100);

        Lcd_Cmd(_LCD_CLEAR);
        Lcd_Out(1,1,"FREIN");
        Lcd_Out(2,1,"V : ");

        IntToStr(current_speed, lcd_text);
        Ltrim(lcd_text);
        Lcd_Out_CP(lcd_text);
        Lcd_Out_CP("%");

        Delay_ms(30);
    }

    PWM1_Set_Duty(0);
    RD0_bit = 0;
    RD1_bit = 0;

    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Out(1,1,"ARRET");


    while (security) {
        distance_value = ADC_Read(1);
        if (distance_value >= 300) break;
        Delay_ms(100);
    }

    Lcd_Cmd(_LCD_CLEAR);
    RB7_bit = 0;  // Red LED OFF
    RB1_bit = 0;  // Turn off RB1 after braking
    braking = 0;
}

// ================================================================
// MAIN PROGRAM
// ================================================================
void main() {

    ADCON0 = 0x81;
    ADCON1 = 0xC4;
    TRISA = 0x03;
    TRISB = 0x71;
    PORTB = 0;
    TRISC = 0x00;
    PORTC = 0;
    TRISD = 0x00;
    PORTD = 0;
    NOT_RBPU_bit = 0;

    PWM1_Init(20000);
    PWM1_Start();
    PWM1_Set_Duty(0);

    Lcd_Init();
    Delay_ms(20);
    Lcd_Cmd(_LCD_CLEAR);

    Timer0_Init();

    rb0_last = RB0_bit;
    last_rb4_state = RB4_bit;

    dummy = PORTB;
    RBIF_bit = 0;

    GIE_bit = 1;
    PEIE_bit = 1;
    RBIE_bit = 1;

    rb4_pressed = 0;
    rb5_pressed = 0;

    Lcd_Out(1, 1, "SYSTEME PRET");
    RB3_bit = 1;
    Delay_ms(50);
    RB3_bit = 0;
    Delay_ms(30);

    Lcd_Cmd(_LCD_CLEAR);

    while(1) {

        // SECURITY BUTTON
        if (RB0_bit && !rb0_last) {
            rb0_last = 1;

            security = !security;

            if (security) {
                Lcd_Cmd(_LCD_CLEAR);
                Lcd_Out(1, 1, "SEC ON");
            } else {
                Lcd_Cmd(_LCD_CLEAR);
                Lcd_Out(1, 1, "SEC OFF");
            }

            Delay_ms(100);
            Lcd_Cmd(_LCD_CLEAR);
        }
        if (!RB0_bit) rb0_last = 0;

        // CHARGE MODE
        if (rb4_pressed) {
            rb4_pressed = 0;

            if (!charge_mode) {
                // First press - Enter charge mode with default delay
                charge_mode = 1;
                charge_done = 0;
                charge_timer_active = 0;
                default_delay_active = 1;
                default_delay_counter = 0;
                rb4_press_count = 0;
                rb4_press_timeout = 0;

                Lcd_Cmd(_LCD_CLEAR);
                Lcd_Out(1, 1, "CHARGING");
                Delay_ms(100);
                Lcd_Cmd(_LCD_CLEAR);

            } else if (default_delay_active) {
                // Pressed during default delay - count presses
                rb4_press_count++;

                if (rb4_press_count >= 4) {
                    // 4 presses total - skip default delay and start timer
                    default_delay_active = 0;
                    charge_timer_active = 1;
                    Timer0_Start();

                    Lcd_Cmd(_LCD_CLEAR);
                    Lcd_Out(1, 1, "TIMER");
                    Delay_ms(100);
                    Lcd_Cmd(_LCD_CLEAR);
                }
            }
        }

        // HISTORY
        if (rb5_pressed) {
            rb5_pressed = 0;
            Lcd_Cmd(_LCD_CLEAR);
            Lcd_Out(1, 1, "HISTORIQUE");
            Delay_ms(100);
            Lcd_Cmd(_LCD_CLEAR);
        }

        // CHARGE MODE ACTIVE
        if (charge_mode) {
            if (!charge_done) {

                // DEFAULT DELAY MODE (500ms )
                if (default_delay_active) {
                    // Flicker both LEDs together
                    RB3_bit = ~RB3_bit;
                    RB7_bit = ~RB7_bit;
                    Delay_ms(50);

                    default_delay_counter += 50;

                    // Check if 500ms passed
                    if (default_delay_counter >= 800) {
                        // Default delay finished
                        default_delay_active = 0;
                        charge_done = 1;
                        charge_mode = 0;

                        // Turn off red, turn on green
                        RB7_bit = 0;
                        RB3_bit = 1;

                        Lcd_Cmd(_LCD_CLEAR);
                        Lcd_Out(1, 1, "CHARGED");
                        Delay_ms(150);

                        // Turn off green LED
                        RB3_bit = 0;
                        Lcd_Cmd(_LCD_CLEAR);
                    }
                }

                // TIMER MODE
                else if (charge_timer_active) {
                    if (charge_complete) {
                        // Timer finished
                        charge_done = 1;
                        charge_mode = 0;

                        // Turn off red, turn on green
                        RB7_bit = 0;
                        RB3_bit = 1;

                        Lcd_Cmd(_LCD_CLEAR);
                        Lcd_Out(1, 1, "CHARGED");
                        Delay_ms(150);

                        // Turn off green LED
                        RB3_bit = 0;
                        Lcd_Cmd(_LCD_CLEAR);
                    } else {
                        // Flicker both LEDs together during timer
                        RB3_bit = ~RB3_bit;
                        RB7_bit = ~RB7_bit;
                        Delay_ms(50);
                    }
                }
            }
            continue;
        }

        // NORMAL SPEED MODE
        accel_value = ADC_Read(0);
        speed_percent = ((unsigned long)accel_value * 100) / 1023;

        if (!braking) {
            unsigned short target_speed;
            unsigned short speed_change;

            // Motor direction and target speed calculation with deadzone
            if (speed_percent < 48) {
                // BACKWARD - speed increases
                RD0_bit = 0;
                RD1_bit = 1;
                target_speed = 100;
            }
            else if (speed_percent > 52) {
                // FORWARD - speed increases
                RD0_bit = 1;
                RD1_bit = 0;
                target_speed = 100;
            }
            else {
                // STOP - speed decreases to 0%
                RD0_bit = 0;
                RD1_bit = 0;
                target_speed = 0;
            }


            // Determine speed change based on current speed level
            if (current_speed == 50) {
                speed_change = 10;  // At exactly 50%, reduce by 10%
            }
            else if (current_speed > 90) {
                speed_change = 5;   // Over 90%: change by 5%
            }
            else if (current_speed > 70) {
                speed_change = 3;   // 70-90%: change by 3%
            }
            else if (current_speed > 50) {
                speed_change = 1;   // 50-70%: change by 1%
            }
            else if (current_speed > 30) {
                speed_change = 1;   // 30-50%: change by 1%
            }
            else if (current_speed > 10) {
                speed_change = 3;   // 10-30%: change by 3%
            }
            else {
                speed_change = 5;   // 0-10%: change by 5%
            }

            // Apply speed change
            if (current_speed < target_speed) {
                // Accelerating
                if (target_speed - current_speed >= speed_change)
                    current_speed += speed_change;
                else
                    current_speed = target_speed;
            }
            else if (current_speed > target_speed) {
                // Decelerating
                if (current_speed - target_speed >= speed_change)
                    current_speed -= speed_change;
                else
                    current_speed = target_speed;
            }

            PWM1_Set_Duty(current_speed * 255 / 100);

            Lcd_Cmd(_LCD_CLEAR);

            IntToStr(current_speed, lcd_text);
            Ltrim(lcd_text);
            Lcd_Out_CP(lcd_text);
            Lcd_Out_CP("%");
        }

        // DISTANCE CHECK
        timer_100ms++;
        if (timer_100ms >= 5) {
            timer_100ms = 0;
            distance_value = ADC_Read(1);
            if (security && distance_value < 300 && !braking)
                Emergency_Stop();
        }

        Delay_ms(50);
    }
}