/*
 * VAG_CDC.c
 *
 * Created: 23.06.2013 20:00:51
 *  Author: Dennis Schuett, dev.shyd.de
 */

#define F_CPU 8000000UL

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "uart.h"

#define UART_BAUD_RATE 9600
#define LED_PWR PA0
#define RADIO_OUT PD2
#define FT_CBUS1 PD3
#define RADIO_OUT_IS_HIGH (PIND & (1<<RADIO_OUT))

#define CDC_PREFIX1 0x53
#define CDC_PREFIX2 0x2C

#define CDC_END_CMD 0x14
#define CDC_PLAY 0xE4
#define CDC_STOP 0x10
#define CDC_NEXT 0xF8
#define CDC_PREV 0x78
#define CDC_SEEK_FWD 0xD8
#define CDC_SEEK_RWD 0x58
#define CDC_CD1 0x0C
#define CDC_CD2 0x8C
#define CDC_CD3 0x4C
#define CDC_CD4 0xCC
#define CDC_CD5 0x2C
#define CDC_CD6 0xAC
#define CDC_SCAN 0xA0
#define CDC_SFL 0x60
#define CDC_PLAY_NORMAL 0x08

#define MODE_PLAY 0xFF
#define MODE_SHFFL 0x55
#define MODE_SCAN 0x00

uint16_t captimehi = 0;
uint16_t captimelo = 0;
uint8_t capturingstart = 0;
uint8_t capturingbytes = 0;
uint32_t cmd = 0;
uint8_t cmdbit = 0;
uint8_t newcmd = 0;
uint8_t shutdownpending = 0;

uint8_t getCommand(uint32_t cmd);
void shutdown();
void softreset();

volatile uint8_t cd;
volatile uint8_t tr;
volatile uint8_t mode;

ISR(INT0_vect) //remote signals
{
	if(RADIO_OUT_IS_HIGH)
	{
		if (capturingstart || capturingbytes)
		{
			captimelo = TCNT1;
		}
		else
			capturingstart = 1;
		TCNT1 = 0;

		//eval times
		if (captimehi > 8300 && captimelo > 3500)
		{
			capturingstart = 0;
			capturingbytes = 1;
			//uart_puts("startseq found\r\n");
		}
		else if(capturingbytes && captimelo > 1500)
		{
			//uart_puts("bit 1\r\n");
			cmd = (cmd<<1) | 0x00000001;
			cmdbit++;
		}
		else if (capturingbytes && captimelo > 500)
		{
			//uart_puts("bit 0\r\n");
			cmd = (cmd<<1);
			cmdbit++;
		}
		else
		{
			//uart_puts("nothing found\r\n");
		}
		if(cmdbit == 32)
		{
			//uart_puts("new cmd\r\n");
			newcmd = 1;
			cmdbit = 0;
			capturingbytes = 0;
		}
	}
	else
	{
		captimehi = TCNT1;
		TCNT1 = 0;
	}
}

ISR(INT1_vect) //ft230x cbus1
{
	if (PIND & (1<<FT_CBUS1)) //reset radio display to 'PLAY' CD01 TR00
	{
		//PORTA &= ~(1<<LED_PWR);
		cd = 0xBE;
		tr = 0xFF;
		mode = 0xFF;
	}
	else //usb connect
	{
		PORTA |= (1<<LED_PWR);
		_delay_ms(50);
		PORTA &= ~(1<<LED_PWR);
		cd = 0xB0;
		tr = 0x00;
	}
}

uint8_t spi_xmit(uint8_t val) {
	SPDR = val;
	while(!(SPSR & (1<<SPIF)));
	return SPDR;
}

void send_package(uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5, uint8_t c6, uint8_t c7)
{
	spi_xmit(c0);
	_delay_us(874);
	spi_xmit(c1);
	_delay_us(874);
	spi_xmit(c2);
	_delay_us(874);
	spi_xmit(c3);
	_delay_us(874);
	spi_xmit(c4);
	_delay_us(874);
	spi_xmit(c5);
	_delay_us(874);
	spi_xmit(c6);
	_delay_us(874);
	spi_xmit(c7);
}

int main(void)
{
	cd = 0xBE;
	tr = 0xFF;
	mode = 0xFF;
	//LEDs
	DDRA |= (1<<LED_PWR);

	//pullup
	PORTD |= (1<<FT_CBUS1);

	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));

	//init SPI
	DDRB |= (1<<PB5) | (1<<PB7)| (1<<DDB4);

	// SPI Type: Master
	// SPI Clock Rate: 62,500 kHz
	// SPI Clock Phase: Cycle Start
	// SPI Clock Polarity: Low
	// SPI Data Order: MSB First
	SPCR=0x57;//at 8MHz
	//SPCR=0x56;//at 4MHz
	SPSR=0x00;

	//beta commands -> cdc
	TCCR1B |= (1<<CS11);    // no prescaler 8 -> 1 timer clock tick is 1us long
	GICR |= (1<<INT0) | (1<<INT1);
	MCUCR |= (1<<ISC00) | (1<<ISC10); //any change on INT0 and INT1
	sei();

	//init led on
	PORTA |= (1<<LED_PWR);
	_delay_ms(500);
	//uart_puts("VAG_CDC ready...\r\n");
	uart_putc(0xAA);
	uart_putc(0x55);
	PORTA &= ~(1<<LED_PWR);

	send_package(0x74,0xBE,0xFE,0xFF,0xFF,0xFF,0x8F,0x7C); //idle
	_delay_ms(10);
	send_package(0x34,0xFF,0xFE,0xFE,0xFE,0xFF,0xFA,0x3C); //load disc
	_delay_ms(100);
	send_package(0x74,0xBE,0xFE,0xFF,0xFF,0xFF,0x8F,0x7C); //idle
	_delay_ms(10);

    while(1)
    {
		int r = uart_getc();
		//r has new data
		if(r <= 0xFF)
		{
			//send inverted CD No.
			if((r & 0xC0) == 0xC0)
			{
				if (r == 0xCA)
					mode = MODE_SCAN;
				else if (r == 0xCB)
					mode = MODE_SHFFL;
				else if (r == 0xCC)
					mode = MODE_PLAY;
				else
					cd = 0xFF^(r & 0x0F);
			}
			//send inverted TR No.
			else
				tr = 0xFF^r;
		}
		//                disc  trk  min  sec
		//send_package(0x34,cd,tr,0xFF,0xFF,0xFF,0xCF,0x3C);
		send_package(0x34,cd,tr,0xFF,0xFF,mode,0xCF,0x3C);
        _delay_ms(41);
		if (newcmd)
		{
			newcmd = 0;
			uint8_t c = getCommand(cmd);
			if (c)
			{
				uart_putc(c);
				/*if (c == CDC_STOP)
				{
					shutdownpending = 1;
				}
				else if (shutdownpending && c == CDC_END_CMD)
				{
					shutdownpending = 0;
					PORTA &= ~(1<<LED_PWR);
					_delay_ms(100);
					shutdown();
					PORTA |= (1<<LED_PWR);
				}*/
			}
		}
    }
}

uint8_t getCommand(uint32_t cmd)
{
	if (((cmd>>24) & 0xFF) == CDC_PREFIX1 && ((cmd>>16) & 0xFF) == CDC_PREFIX2)
		if (((cmd>>8) & 0xFF) == (0xFF^((cmd) & 0xFF)))
			return (cmd>>8) & 0xFF;
	return 0;
}