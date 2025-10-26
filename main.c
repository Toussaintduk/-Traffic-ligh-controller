#define F_CPU 1000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <avr/wdt.h>
#include <util/delay.h>

// LED Definitions
#define NORTH_RED     PINB0
#define NORTH_YELLOW  PINB1
#define NORTH_GREEN   PINB2
#define EAST_RED      PINB3
#define EAST_YELLOW   PINB4
#define EAST_GREEN    PINB5
#define SOUTH_RED     PINC0
#define SOUTH_YELLOW  PINC1
#define SOUTH_GREEN   PINC2
#define WEST_RED      PINC3
#define WEST_YELLOW   PINC4
#define WEST_GREEN    PINC5

// Pedestrian signals
#define PED_NORTH_RED     PIND0
#define PED_NORTH_GREEN   PIND1
#define PED_EAST_RED      PIND2
#define PED_EAST_GREEN    PIND3

// Pedestrian call buttons
#define BUTTON_NORTH    PIND4
#define BUTTON_EAST     PIND5
#define BUTTON_SOUTH    PIND6
#define BUTTON_WEST     PIND7

// Buzzer and display
#define BUZZER        PINA0
#define SEG_A   PA1
#define SEG_B   PA2
#define SEG_C   PA3
#define SEG_D   PA4
#define SEG_E   PA5
#define SEG_F   PA6
#define SEG_G   PA7

// Timing Constants (ms)
#define GREEN_PHASE     20000
#define YELLOW_PHASE    5000
#define ALL_RED_PHASE   2000
#define WALK_TIME       10000
#define WARNING_TIME    3000
#define DEBOUNCE_TIME   50

// System Modes
typedef enum {
	NORMAL_MODE,
	RUSH_HOUR_MODE,
	NIGHT_MODE,
	EMERGENCY_MODE
} SystemMode;

// Traffic Light States
typedef enum {
	NS_GREEN_EW_RED,
	NS_YELLOW_EW_RED,
	ALL_RED_1,
	NE_GREEN_SW_RED,
	NE_YELLOW_SW_RED,
	ALL_RED_2,
	NW_GREEN_SE_RED,
	NW_YELLOW_SE_RED,
	ALL_RED_3,
	SE_GREEN_NW_RED,
	SE_YELLOW_NW_RED,
	ALL_RED_4,
	SW_GREEN_NE_RED,
	SW_YELLOW_NE_RED,
	ALL_RED_5,
	EW_GREEN_NS_RED,
	EW_YELLOW_NS_RED,
	ALL_RED_6,
	PEDESTRIAN_PHASE_N,
	PEDESTRIAN_PHASE_E,
	PEDESTRIAN_PHASE_S,
	PEDESTRIAN_PHASE_W
} TrafficState;

// Direction enumeration
typedef enum {
	NORTH,
	EAST,
	SOUTH,
	WEST,
	NORTH_EAST,
	SOUTH_WEST,
	NORTH_WEST,
	SOUTH_EAST
} Direction;

// Global Variables
volatile uint32_t system_ticks = 0;
volatile SystemMode current_mode = NORMAL_MODE;
volatile bool pedestrian_request[4] = {false};
volatile uint16_t countdown_timer = 0;
volatile TrafficState current_state = NS_GREEN_EW_RED;
volatile uint16_t state_timer = 0;
volatile uint8_t displayed_number = 0;

// 7-segment patterns (common anode) for digits 0-9
const uint8_t segment_patterns[10] = {
	0xC0, // 0 - segments a,b,c,d,e,f
	0xF9, // 1 - segments b,c
	0xA4, // 2 - segments a,b,g,e,d
	0xB0, // 3 - segments a,b,g,c,d
	0x99, // 4 - segments f,g,b,c
	0x92, // 5 - segments a,f,g,c,d
	0x82, // 6 - segments a,f,g,e,c,d
	0xF8, // 7 - segments a,b,c
	0x80, // 8 - all segments
	0x90  // 9 - segments a,b,c,d,f,g
};

// Function Prototypes
void initialize_ports(void);
void initialize_interrupts(void);
void update_display(uint8_t value);
void advance_state(void);
void check_pedestrian_requests(void);
void start_pedestrian_phase(Direction direction);
void set_lights(TrafficState new_state);
void buzzer_on(void);
void buzzer_off(void);
void test_display(void);

int main(void) {
	initialize_ports();
	initialize_interrupts();
	
	// Uncomment to test display during startup
	// test_display();
	
	set_lights(NS_GREEN_EW_RED);
	state_timer = GREEN_PHASE;
	
	sei();

	while(1) {
		// Main processing handled in interrupts
	}
}

void test_display() {
	// Count from 0 to 9
	for(uint8_t i = 0; i < 10; i++) {
		update_display(i);
		_delay_ms(500);
	}
	// Show 8 (all segments) for 2 seconds
	update_display(8);
	_delay_ms(2000);
	// Clear display
	PORTA &= 0x01;
}

void initialize_ports(void) {
	// Set all traffic lights as outputs
	DDRB |= (1<<NORTH_RED)|(1<<NORTH_YELLOW)|(1<<NORTH_GREEN)|
	(1<<EAST_RED)|(1<<EAST_YELLOW)|(1<<EAST_GREEN);
	DDRC |= (1<<SOUTH_RED)|(1<<SOUTH_YELLOW)|(1<<SOUTH_GREEN)|
	(1<<WEST_RED)|(1<<WEST_YELLOW)|(1<<WEST_GREEN);

	// Pedestrian signals and buttons
	DDRD |= (1<<PED_NORTH_RED)|(1<<PED_NORTH_GREEN)|(1<<PED_EAST_RED)|(1<<PED_EAST_GREEN);
	DDRD &= ~((1<<BUTTON_NORTH)|(1<<BUTTON_EAST)|(1<<BUTTON_SOUTH)|(1<<BUTTON_WEST));
	PORTD |= (1<<BUTTON_NORTH)|(1<<BUTTON_EAST)|(1<<BUTTON_SOUTH)|(1<<BUTTON_WEST);

	// Buzzer and display
	DDRA |= (1<<BUZZER)|0xFE;  // PA0 for buzzer, PA1-PA7 for display
	PORTA &= ~(0xFE);          // Turn off all segments initially

	// Initialize pedestrian signals to "Don't Walk"
	PORTD |= (1<<PED_NORTH_RED)|(1<<PED_EAST_RED);
	PORTD &= ~((1<<PED_NORTH_GREEN)|(1<<PED_EAST_GREEN));
}

void initialize_interrupts(void) {
	// Button interrupts
	PCICR |= (1<<PCIE3);
	PCMSK3 |= (1<<PCINT28)|(1<<PCINT29)|(1<<PCINT30)|(1<<PCINT31);

	// Timer1 for 1ms interrupts (CTC mode)
	TCCR1A = 0;
	TCCR1B = (1<<WGM12)|(1<<CS11)|(1<<CS10); // CTC mode, prescaler 64
	OCR1A = 15; // For 1ms at 1MHz with prescaler 64: (1MHz/64)/1000Hz - 1 = 15.625 ? 15
	TIMSK1 = (1<<OCIE1A); // Enable Timer1 compare match A interrupt
}

ISR(PCINT3_vect) {
	static uint32_t last_press_time = 0;
	static uint8_t button_state = 0xF0;
	uint8_t current_buttons = PIND & 0xF0;

	if (system_ticks - last_press_time > DEBOUNCE_TIME) {
		if (!(current_buttons & (1<<BUTTON_NORTH)) && (button_state & (1<<BUTTON_NORTH))) {
			pedestrian_request[NORTH] = true;
		}
		if (!(current_buttons & (1<<BUTTON_EAST)) && (button_state & (1<<BUTTON_EAST))) {
			pedestrian_request[EAST] = true;
		}
		if (!(current_buttons & (1<<BUTTON_SOUTH)) && (button_state & (1<<BUTTON_SOUTH))) {
			pedestrian_request[SOUTH] = true;
		}
		if (!(current_buttons & (1<<BUTTON_WEST)) && (button_state & (1<<BUTTON_WEST))) {
			pedestrian_request[WEST] = true;
		}

		last_press_time = system_ticks;
		button_state = current_buttons;
	}
}

ISR(TIMER1_COMPA_vect) {
	system_ticks++;

	// Update countdown timer and display
	if (countdown_timer > 0) {
		countdown_timer--;
		
		// Update display during pedestrian phase
		if (current_state >= PEDESTRIAN_PHASE_N && current_state <= PEDESTRIAN_PHASE_W) {
			displayed_number = (countdown_timer / 1000) + 1; // Show countdown from 10 to 1
			if (displayed_number > 9) displayed_number = 9; // Cap at 9 if needed
		}
	}

	// Buzzer handling
	if (current_state >= PEDESTRIAN_PHASE_N && current_state <= PEDESTRIAN_PHASE_W) {
		if (countdown_timer > WARNING_TIME) {
			// Regular beep (500ms on/off)
			if ((system_ticks % 1000) < 500) buzzer_on();
			else buzzer_off();
			} else {
			// Fast beep for last 3 seconds (200ms on/off)
			if ((system_ticks % 400) < 200) buzzer_on();
			else buzzer_off();
		}
		} else {
		buzzer_off();
	}

	update_display(displayed_number);

	if (state_timer > 0) {
		state_timer--;
		if (state_timer == 0) {
			advance_state();
		}
	}
	
	// Check pedestrian requests
	if (pedestrian_request[NORTH] || pedestrian_request[EAST] ||
	pedestrian_request[SOUTH] || pedestrian_request[WEST]) {
		check_pedestrian_requests();
	}
}

void update_display(uint8_t value) {
	// Ensure value is in range (0-9)
	if (value > 9) value = 9;
	
	// Clear display bits (PA1-PA7) while preserving buzzer (PA0)
	PORTA &= 0x01;
	
	// Apply the new pattern (shifted left by 1 since we start at PA1)
	PORTA |= (segment_patterns[value] << 1);
}

void advance_state(void) {
	switch (current_state) {
		case NS_GREEN_EW_RED:
		set_lights(NS_YELLOW_EW_RED);
		state_timer = YELLOW_PHASE;
		displayed_number = 5;
		break;

		case NS_YELLOW_EW_RED:
		set_lights(ALL_RED_1);
		state_timer = ALL_RED_PHASE;
		displayed_number = 2;
		break;

		case ALL_RED_1:
		set_lights(NE_GREEN_SW_RED);
		state_timer = GREEN_PHASE;
		displayed_number = 5;
		break;

		case NE_GREEN_SW_RED:
		set_lights(NE_YELLOW_SW_RED);
		state_timer = YELLOW_PHASE;
		displayed_number = 5;
		break;

		case NE_YELLOW_SW_RED:
		set_lights(ALL_RED_2);
		state_timer = ALL_RED_PHASE;
		displayed_number = 2;
		break;

		case ALL_RED_2:
		set_lights(NW_GREEN_SE_RED);
		state_timer = GREEN_PHASE;
		displayed_number = 5;
		break;

		case NW_GREEN_SE_RED:
		set_lights(NW_YELLOW_SE_RED);
		state_timer = YELLOW_PHASE;
		displayed_number = 5;
		break;

		case NW_YELLOW_SE_RED:
		set_lights(ALL_RED_3);
		state_timer = ALL_RED_PHASE;
		displayed_number = 2;
		break;

		case ALL_RED_3:
		set_lights(SE_GREEN_NW_RED);
		state_timer = GREEN_PHASE;
		displayed_number = 5;
		break;

		case SE_GREEN_NW_RED:
		set_lights(SE_YELLOW_NW_RED);
		state_timer = YELLOW_PHASE;
		displayed_number = 5;
		break;

		case SE_YELLOW_NW_RED:
		set_lights(ALL_RED_4);
		state_timer = ALL_RED_PHASE;
		displayed_number = 2;
		break;

		case ALL_RED_4:
		set_lights(SW_GREEN_NE_RED);
		state_timer = GREEN_PHASE;
		displayed_number = 5;
		break;

		case SW_GREEN_NE_RED:
		set_lights(SW_YELLOW_NE_RED);
		state_timer = YELLOW_PHASE;
		displayed_number = 5;
		break;

		case SW_YELLOW_NE_RED:
		set_lights(ALL_RED_5);
		state_timer = ALL_RED_PHASE;
		displayed_number = 2;
		break;

		case ALL_RED_5:
		set_lights(EW_GREEN_NS_RED);
		state_timer = GREEN_PHASE;
		displayed_number = 5;
		break;

		case EW_GREEN_NS_RED:
		set_lights(EW_YELLOW_NS_RED);
		state_timer = YELLOW_PHASE;
		displayed_number = 5;
		break;

		case EW_YELLOW_NS_RED:
		set_lights(ALL_RED_6);
		state_timer = ALL_RED_PHASE;
		displayed_number = 2;
		break;

		case ALL_RED_6:
		set_lights(NS_GREEN_EW_RED);
		state_timer = GREEN_PHASE;
		displayed_number = 5;
		break;

		case PEDESTRIAN_PHASE_N:
		case PEDESTRIAN_PHASE_E:
		case PEDESTRIAN_PHASE_S:
		case PEDESTRIAN_PHASE_W:
		set_lights(ALL_RED_1);
		state_timer = ALL_RED_PHASE;
		displayed_number = 2;
		break;
	}
}

void check_pedestrian_requests(void) {
	// Check if current state allows interruption
	bool can_interrupt = false;
	
	switch (current_state) {
		case NS_GREEN_EW_RED:
		case NS_YELLOW_EW_RED:
		can_interrupt = pedestrian_request[NORTH] || pedestrian_request[SOUTH];
		break;
		
		case NE_GREEN_SW_RED:
		case NE_YELLOW_SW_RED:
		can_interrupt = pedestrian_request[NORTH] || pedestrian_request[EAST];
		break;
		
		case NW_GREEN_SE_RED:
		case NW_YELLOW_SE_RED:
		can_interrupt = pedestrian_request[NORTH] || pedestrian_request[WEST];
		break;
		
		case SE_GREEN_NW_RED:
		case SE_YELLOW_NW_RED:
		can_interrupt = pedestrian_request[SOUTH] || pedestrian_request[EAST];
		break;
		
		case SW_GREEN_NE_RED:
		case SW_YELLOW_NE_RED:
		can_interrupt = pedestrian_request[SOUTH] || pedestrian_request[WEST];
		break;
		
		case EW_GREEN_NS_RED:
		case EW_YELLOW_NS_RED:
		can_interrupt = pedestrian_request[EAST] || pedestrian_request[WEST];
		break;
		
		default:
		can_interrupt = false;
		break;
	}
	
	if (can_interrupt) {
		if (pedestrian_request[NORTH]) {
			start_pedestrian_phase(NORTH);
			return;
		}
		if (pedestrian_request[EAST]) {
			start_pedestrian_phase(EAST);
			return;
		}
		if (pedestrian_request[SOUTH]) {
			start_pedestrian_phase(SOUTH);
			return;
		}
		if (pedestrian_request[WEST]) {
			start_pedestrian_phase(WEST);
			return;
		}
	}
}

void start_pedestrian_phase(Direction direction) {
	// First set all traffic lights to red
	PORTB |= (1<<NORTH_RED)|(1<<EAST_RED);
	PORTC |= (1<<SOUTH_RED)|(1<<WEST_RED);
	PORTB &= ~((1<<NORTH_GREEN)|(1<<NORTH_YELLOW)|(1<<EAST_GREEN)|(1<<EAST_YELLOW));
	PORTC &= ~((1<<SOUTH_GREEN)|(1<<SOUTH_YELLOW)|(1<<WEST_GREEN)|(1<<WEST_YELLOW));

	// Activate appropriate traffic flow opposite to pedestrian direction
	switch (direction) {
		case NORTH:
		// When North pedestrian is crossing, allow South-East flow
		PORTB |= (1<<EAST_GREEN);
		PORTC |= (1<<SOUTH_GREEN);
		break;
		
		case EAST:
		// When East pedestrian is crossing, allow North-West flow
		PORTB |= (1<<NORTH_GREEN);
		PORTC |= (1<<WEST_GREEN);
		break;
		
		case SOUTH:
		// When South pedestrian is crossing, allow North-East flow
		PORTB |= (1<<NORTH_GREEN)|(1<<EAST_GREEN);
		break;
		
		case WEST:
		// When West pedestrian is crossing, allow South-East flow
		PORTC |= (1<<SOUTH_GREEN)|(1<<WEST_GREEN);
		break;
	}

	// Activate pedestrian signal
	if (direction == NORTH || direction == SOUTH) {
		PORTD |= (1<<PED_NORTH_GREEN);
		PORTD &= ~(1<<PED_NORTH_RED);
		current_state = (direction == NORTH) ? PEDESTRIAN_PHASE_N : PEDESTRIAN_PHASE_S;
		} else {
		PORTD |= (1<<PED_EAST_GREEN);
		PORTD &= ~(1<<PED_EAST_RED);
		current_state = (direction == EAST) ? PEDESTRIAN_PHASE_E : PEDESTRIAN_PHASE_W;
	}

	pedestrian_request[direction] = false;
	countdown_timer = WALK_TIME;
	state_timer = WALK_TIME;
	displayed_number = 10; // Initialize countdown at 10 (will show 9 first)
}

void set_lights(TrafficState new_state) {
	// Turn off all traffic lights first
	PORTB &= ~((1<<NORTH_RED)|(1<<NORTH_YELLOW)|(1<<NORTH_GREEN)|
	(1<<EAST_RED)|(1<<EAST_YELLOW)|(1<<EAST_GREEN));
	PORTC &= ~((1<<SOUTH_RED)|(1<<SOUTH_YELLOW)|(1<<SOUTH_GREEN)|
	(1<<WEST_RED)|(1<<WEST_YELLOW)|(1<<WEST_GREEN));

	// Turn off pedestrian signals
	PORTD &= ~((1<<PED_NORTH_GREEN)|(1<<PED_EAST_GREEN));
	PORTD |= (1<<PED_NORTH_RED)|(1<<PED_EAST_RED);

	// Set new traffic light state
	switch (new_state) {
		case NS_GREEN_EW_RED:
		PORTB |= (1<<NORTH_GREEN)|(1<<EAST_RED);
		PORTC |= (1<<SOUTH_GREEN)|(1<<WEST_RED);
		break;

		case NS_YELLOW_EW_RED:
		PORTB |= (1<<NORTH_YELLOW)|(1<<EAST_RED);
		PORTC |= (1<<SOUTH_YELLOW)|(1<<WEST_RED);
		break;

		case NE_GREEN_SW_RED:
		// North-East (North+East) green, South-West red
		PORTB |= (1<<NORTH_GREEN)|(1<<EAST_GREEN);
		PORTC |= (1<<SOUTH_RED)|(1<<WEST_RED);
		break;

		case NE_YELLOW_SW_RED:
		PORTB |= (1<<NORTH_YELLOW)|(1<<EAST_YELLOW);
		PORTC |= (1<<SOUTH_RED)|(1<<WEST_RED);
		break;

		case NW_GREEN_SE_RED:
		// North-West (North+West) green, South-East red
		PORTB |= (1<<NORTH_GREEN)|(1<<EAST_RED);
		PORTC |= (1<<SOUTH_RED)|(1<<WEST_GREEN);
		break;

		case NW_YELLOW_SE_RED:
		PORTB |= (1<<NORTH_YELLOW)|(1<<EAST_RED);
		PORTC |= (1<<SOUTH_RED)|(1<<WEST_YELLOW);
		break;

		case SE_GREEN_NW_RED:
		// South-East (South+East) green, North-West red
		PORTB |= (1<<NORTH_RED)|(1<<EAST_GREEN);
		PORTC |= (1<<SOUTH_GREEN)|(1<<WEST_RED);
		break;

		case SE_YELLOW_NW_RED:
		PORTB |= (1<<NORTH_RED)|(1<<EAST_YELLOW);
		PORTC |= (1<<SOUTH_YELLOW)|(1<<WEST_RED);
		break;

		case SW_GREEN_NE_RED:
		// South-West (South+West) green, North-East red
		PORTB |= (1<<NORTH_RED)|(1<<EAST_RED);
		PORTC |= (1<<SOUTH_GREEN)|(1<<WEST_GREEN);
		break;

		case SW_YELLOW_NE_RED:
		PORTB |= (1<<NORTH_RED)|(1<<EAST_RED);
		PORTC |= (1<<SOUTH_YELLOW)|(1<<WEST_YELLOW);
		break;

		case EW_GREEN_NS_RED:
		PORTB |= (1<<NORTH_RED)|(1<<EAST_GREEN);
		PORTC |= (1<<SOUTH_RED)|(1<<WEST_GREEN);
		break;

		case EW_YELLOW_NS_RED:
		PORTB |= (1<<NORTH_RED)|(1<<EAST_YELLOW);
		PORTC |= (1<<SOUTH_RED)|(1<<WEST_YELLOW);
		break;

		case ALL_RED_1:
		case ALL_RED_2:
		case ALL_RED_3:
		case ALL_RED_4:
		case ALL_RED_5:
		case ALL_RED_6:
		PORTB |= (1<<NORTH_RED)|(1<<EAST_RED);
		PORTC |= (1<<SOUTH_RED)|(1<<WEST_RED);
		break;

		case PEDESTRIAN_PHASE_N:
		// North pedestrian crossing with South-East flow
		PORTB |= (1<<EAST_GREEN);
		PORTC |= (1<<SOUTH_GREEN);
		break;

		case PEDESTRIAN_PHASE_E:
		// East pedestrian crossing with North-West flow
		PORTB |= (1<<NORTH_GREEN);
		PORTC |= (1<<WEST_GREEN);
		break;

		case PEDESTRIAN_PHASE_S:
		// South pedestrian crossing with North-East flow
		PORTB |= (1<<NORTH_GREEN)|(1<<EAST_GREEN);
		break;

		case PEDESTRIAN_PHASE_W:
		// West pedestrian crossing with South-East flow
		PORTC |= (1<<SOUTH_GREEN)|(1<<WEST_GREEN);
		break;
	}

	current_state = new_state;
}

void buzzer_on(void) {
	PORTA |= (1<<BUZZER);
}

void buzzer_off(void) {
	PORTA &= ~(1<<BUZZER);
}