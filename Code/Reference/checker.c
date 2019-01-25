/* ==================================================================== */
/*             Example Public Traffic IC Card System program            */
/* ==================================================================== */

// Standard GCC include files for AVR
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <util/delay.h>
#include <stdlib.h>

// Useful stuff
#define BM(n) (1 << (n))


//**********************************************************************//
//* UART ���� ��ũ��                                                   *//
//**********************************************************************//

//-------------------------------------------------------------------------------------------------------
// Gloval interrupt control
//-------------------------------------------------------------------------------------------------------
#define ENABLE_GLOBAL_INT()         do { asm ("sei\n\t" ::); } while (0)
#define DISABLE_GLOBAL_INT()        do { asm ("cli\n\t" ::); } while (0)
//-------------------------------------------------------------------------------------------------------
// UART0 interrupts
//-------------------------------------------------------------------------------------------------------
#define ENABLE_UART0_INT()          do { UCSR0B |= (BM(UDRIE0) | BM(RXCIE0)); } while (0)
#define DISABLE_UART0_INT()         do { UCSR0B &= ~(BM(UDRIE0) | BM(RXCIE0)); } while (0) 

#define ENABLE_UART0_TX_INT()       do { UCSR0B |= BM(UDRIE0); } while (0)
#define DISABLE_UART0_TX_INT()      do { UCSR0B &= ~BM(UDRIE0); } while (0) 
#define CLEAR_UART0_TX_INT()        do { UCSR0A &= ~BM(UDRE0); } while (0)
#define SET_UART0_TX_INT()          do { UCSR0A |= BM(UDRE0); } while (0)

#define ENABLE_UART0_RX_INT()       do { UCSR0B |= BM(RXCIE0); } while (0)
#define DISABLE_UART0_RX_INT()      do { UCSR0B &= ~BM(RXCIE0); } while (0) 
#define CLEAR_UART0_RX_INT()        do { UCSR0A &= ~BM(RXC0); } while (0)
//-------------------------------------------------------------------------------------------------------
// UART1 interrupts
//-------------------------------------------------------------------------------------------------------
#define ENABLE_UART1_INT()          do { UCSR1B |= (BM(UDRIE1) | BM(RXCIE1)); } while (0)
#define DISABLE_UART1_INT()         do { UCSR1B &= ~(BM(UDRIE1) | BM(RXCIE1)); } while (0) 

#define ENABLE_UART1_TX_INT()       do { UCSR1B |= BM(UDRIE1); } while (0)
#define DISABLE_UART1_TX_INT()      do { UCSR1B &= ~BM(UDRIE1); } while (0) 
#define CLEAR_UART1_TX_INT()        do { UCSR1A &= ~BM(UDRE1); } while (0)
#define SET_UART1_TX_INT()          do { UCSR1A |= BM(UDRE1); } while (0)

#define ENABLE_UART1_RX_INT()       do { UCSR1B |= BM(RXCIE1); } while (0)
#define DISABLE_UART1_RX_INT()      do { UCSR1B &= ~BM(RXCIE1); } while (0) 
#define CLEAR_UART1_RX_INT()        do { UCSR1A &= ~BM(RXC1); } while (0)
//-------------------------------------------------------------------------------------------------------
// UART0 �ʱ�ȭ �Լ�
//-------------------------------------------------------------------------------------------------------
#define INIT_UART0(baudRate,options) \
    do { \
        UBRR0H = (baudRate) >> 8; \
        UBRR0L = (baudRate); \
        UCSR0C = (unsigned char) options; \
        if (options > 0xFF) { \
            UCSR0B |= 0x04; \
        } else { \
            UCSR0B &= ~0x04; \
        } \
		UCSR0A |= BM(U2X1); \
    } while (0)
//-------------------------------------------------------------------------------------------------------
// UART1 �ʱ�ȭ �Լ�
//-------------------------------------------------------------------------------------------------------
#define INIT_UART1(baudRate,options) \
    do { \
        UBRR1H = (baudRate) >> 8; \
        UBRR1L = (baudRate); \
        UCSR1C = (unsigned char) options; \
        if (options > 0xFF) { \
            UCSR1B |= 0x04; \
        } else { \
            UCSR1B &= ~0x04; \
        } \
		UCSR1A |= BM(U2X1); \
    } while (0)

//-------------------------------------------------------------------------------------------------------
// Baud rate codes for use with the INIT_UART# macro (XTAL=16MHz, U2X=1���)
//-------------------------------------------------------------------------------------------------------
#define UART_BAUDRATE_2K4           832
#define UART_BAUDRATE_4K8           416
#define UART_BAUDRATE_9K6           207
#define UART_BAUDRATE_14K4          138
#define UART_BAUDRATE_19K2          103
#define UART_BAUDRATE_28K8          68
#define UART_BAUDRATE_38K4          51
#define UART_BAUDRATE_57K6          34
#define UART_BAUDRATE_76K8          25
#define UART_BAUDRATE_115K2         16
#define UART_BAUDRATE_230K4         8
#define UART_BAUDRATE_250K          7
#define UART_BAUDRATE_500K          3
#define UART_BAUDRATE_1M            1
//-------------------------------------------------------------------------------------------------------
// Options for use with the INIT_UART1 macro
//-------------------------------------------------------------------------------------------------------
#define UART_OPT_ONE_STOP_BIT       0
#define UART_OPT_TWO_STOP_BITS      0x08
#define UART_OPT_NO_PARITY          0
#define UART_OPT_EVEN_PARITY        0x20
#define UART_OPT_ODD_PARITY         0x30
#define UART_OPT_5_BITS_PER_CHAR    0
#define UART_OPT_6_BITS_PER_CHAR    0x02
#define UART_OPT_7_BITS_PER_CHAR    0x04
#define UART_OPT_8_BITS_PER_CHAR    0x06
#define UART_OPT_9_BITS_PER_CHAR    0x0406
//-------------------------------------------------------------------------------------------------------
// Enable/disable UART
//-------------------------------------------------------------------------------------------------------
#define ENABLE_UART0()              (UCSR0B |= (BM(RXEN0) | BM(TXEN0))) 
#define DISABLE_UART0()             (UCSR0B &= ~(BM(RXEN0) | BM(TXEN0)))
#define ENABLE_UART1()              (UCSR1B |= (BM(RXEN1) | BM(TXEN1))) 
#define DISABLE_UART1()             (UCSR1B &= ~(BM(RXEN1) | BM(TXEN1)))
//-------------------------------------------------------------------------------------------------------
// Macros which are helpful when transmitting and receiving data over the serial interface.
//-------------------------------------------------------------------------------------------------------
#define UART0_WAIT()                do { while (!(UCSR0A & BM(UDRE0))); CLEAR_UART0_TX_INT(); } while (0)
#define UART0_RXWAIT()              do { while (!(UCSR0A & BM(RXC0))); CLEAR_UART0_RX_INT(); } while (0)
#define UART0_SEND(x)               do { UDR0 = (x); } while (0)
#define UART0_WAIT_AND_SEND(x)      do { UART0_WAIT(); UART0_SEND(x); } while (0)
#define UART0_RECEIVE(x)            do { (x) = UDR0; } while (0)
#define UART0_WAIT_AND_RECEIVE(x)   do { UART0_RXWAIT(); UART0_RECEIVE(x); } while (0)
#define UART0_LINEFEED()            do { UART0_WAIT_AND_SEND(13); UART0_WAIT_AND_SEND(10); } while (0)

#define UART1_WAIT()                do { while (!(UCSR1A & BM(UDRE1))); CLEAR_UART1_TX_INT(); } while (0)
#define UART1_RXWAIT()              do { while (!(UCSR1A & BM(RXC1))); CLEAR_UART1_RX_INT(); } while (0)
#define UART1_SEND(x)               do { UDR1 = (x); } while (0)
#define UART1_WAIT_AND_SEND(x)      do { UART1_WAIT(); UART1_SEND(x); } while (0)
#define UART1_RECEIVE(x)            do { (x) = UDR1; } while (0)
#define UART1_WAIT_AND_RECEIVE(x)   do { UART1_RXWAIT(); UART1_RECEIVE(x); } while (0)
#define UART1_LINEFEED()            do { UART1_WAIT_AND_SEND(13); UART1_WAIT_AND_SEND(10); } while (0)
//-------------------------------------------------------------------------------------------------------


//**********************************************************************//
//* CLCD ���� �Լ�                                                     *//
//**********************************************************************//
// �Ʒ��� CLCD ���� ������ ������ ��Ʈ ������ �������� ������,
// ������ ��Ȳ�� �°� ������ �����Ͽ� ���...
//
// ����� CLCD��: HY-1602H-803 Ȥ�� ȣȯǰ(16x2)
//
// CLCD RW�� -> PORTB ��0
// CLCD CE�� -> PORTB ��1
// CLCD RS�� -> PORTB ��2
//
// CLCD D0�� -> PORTE ��0
// CLCD D1�� -> PORTE ��1
// CLCD D2�� -> PORTE ��2
// CLCD D3�� -> PORTE ��3
// CLCD D4�� -> PORTE ��4
// CLCD D5�� -> PORTE ��5
// CLCD D6�� -> PORTE ��6
// CLCD D7�� -> PORTE ��7

#define RW 0
#define CE 1
#define RS 2

void CLCD_CHAR (unsigned char value) // CLCD �� ���� ���� �Լ�
{
	PORTE = value;
	PORTB |= BM(RS);
	PORTB &= ~BM(RW);
	_delay_ms(1);
	PORTB |= BM(CE);
	_delay_ms(1);
	PORTB &= ~BM(CE);
	PORTB &= ~BM(RS);
	PORTE = 0;
}

void CLCD_CMD (unsigned char value) // CLCD Command ���� �Լ�
{
	PORTE = value;
	PORTB &= ~BM(RS);
	PORTB &= ~BM(RW);
	_delay_ms(1);
	PORTB |= BM(CE);
	_delay_ms(1);
	PORTB &= ~BM(CE);
	PORTE = 0;
}

void CLCD_INIT (void) // CLCD �ʱ�ȭ �Լ�
{
	CLCD_CMD(0x38);	// Function Set=> (0:0:1:DL:N:F:*:*) 8bit data lines, 2-line 5x7 dot character
	_delay_ms(1);  // for LCD response time

	CLCD_CMD(0x38);	// repeat 2nd of 3 times
	_delay_ms(1);

	CLCD_CMD(0x38);	// repeat 3rd of 3 times
	_delay_ms(1);

	CLCD_CMD(0x06);	// Entry Mode Set=> (0:0:0:0:0:1:I/D:S) cursor direction, shift display
	_delay_ms(1);

	CLCD_CMD(0x0E);	// Display On/Off Control=> (0:0:0:0:1:D:C:B) Display On, Cursor On, Cursor blink Off
	_delay_ms(1);

	CLCD_CMD(0x01);	// Clear Display=> clear display and move cursor to home position
}

void CLCD_STRING(char *value) // CLCD ���ڿ� ��� �Լ�
{
	while(*value != '\0')
	{ 	
		CLCD_CHAR(*value);
		value++;
	}
}

void CLCD_CURSOR(unsigned char x, unsigned char y) // CLCD Ŀ�� ��ġ ���� �Լ�
{
	if(y == 0)
	{
		CLCD_CMD(0x80 + x);	
	}
	else if(y == 1)
	{
		CLCD_CMD(0xC0 + x);	
	}
}


//**********************************************************************//
//* UART ���� (���ͷ�Ʈ ���)                                          *//
//**********************************************************************//

volatile unsigned char rpmsg[11], count;

// UART0 ���� ���ͷ�Ʈ
ISR(USART0_RX_vect)
{
	rpmsg[count] = UDR0;
	count=count+1;
}

// UART1 ���� ���ͷ�Ʈ
ISR(USART1_RX_vect)
{
	rpmsg[count] = UDR1;
	count++;
}


//**********************************************************************//
//* main()                                                             *//
//**********************************************************************//

int main (void) 
{
	unsigned char temp_UID[8],old_UID[8],valid_UID[8],balance,fare,database[100][10],index=0,terminal_ID;
	unsigned int i;

//**********************************************************************//
//* IO��Ʈ �� ���� �ʱ�ȭ                                              *//
//**********************************************************************//

	DDRB = BM(CE) | BM(RW) | BM(RS); // CLCD ��Ʈ�� ��ȣ(���)
	DDRE = 0xFF; // CLCD ������ ��ȣ(���)

	PORTB &= ~BM(CE) & ~BM(RW) & ~BM(RS); // CLCD ��Ʈ�� ��ȣ �ʱ� ���(0)
	PORTE = 0x00; // CLCD ������ ��ȣ �ʱ� ���(0)

	CLCD_INIT();

	INIT_UART0(UART_BAUDRATE_115K2,UART_OPT_8_BITS_PER_CHAR); // UART0 115.2Kbps at 16MHz X-tal

	ENABLE_UART0();
	ENABLE_UART0_RX_INT();
	ENABLE_GLOBAL_INT();

	terminal_ID=100; // ī�� �ܸ��� ID, ���Ƿ� ����..
	fare = 15; // ����ī�� 1ȸ ���(����) ��� ����(ex, 15��)

	count = 0; // ���ͷ�Ʈ ��� ���� �ε��� �ʱ�ȭ

	// ���� UID�� UID Valid�� ���� ���� �ʱ�ȭ
	for(i=0; i<=7;i++)
	{
		old_UID[i] = 0;
		valid_UID[i] = 0;
	}

	valid_UID[0] = 0xE0; // ISO15693 RFID �±״� �ֻ��� ����Ʈ�� �׻� 0xE0 �̴�.

	// CLCD �ʱ�ȭ��
	CLCD_CMD(0x01);	//Clear display and place cursor at home in CLCD
	CLCD_STRING("RFID-232 Example");
	CLCD_CMD(0xC0);	// Cursor at 2nd line in CLCD
	CLCD_STRING("Traffic IC Card");

	while(1)
	{
		// 1) Send 'InventoryAll' Command Packet(3 byte)
		unsigned char rqmsg[3] = {0x02, 0x00, 0xf0};
		rpmsg[0] = 0x00; // �ʱ�ȭ

		while(rpmsg[0] != 0x0a)
		{
			count = 0;

			for(i=0; i<3; i++)
			{
				UART0_WAIT_AND_SEND(rqmsg[i]);
			}

			// Receive 'Inventory' Command Response Packet (12 byte)
			_delay_ms(100); // ����ð� �ο�
		}

		// 2) If UID detected, temp_UID = UID
		for(i=3; i<=10;i++) // UID �Ľ�
		{
			temp_UID[10-i] = rpmsg[i];
		}

		// 3) Valid UID?
		if(temp_UID[0] == valid_UID[0])
		{
			// 4) temp_UID == old_UID?
			if(!memcmp(temp_UID,old_UID,8)) // temp_UID == old_UID
			{
				// 5) Display "Paid Already!"
				CLCD_CMD(0x01);	//Clear display and place cursor at home in CLCD
				CLCD_STRING("Paid Already!");
			}
			else // temp_UID != old_UID
			{
				// 6) Send 'Read single block' Command Packet(12 byte) for checking blance : Block number is selected 0x00 as example
				unsigned char rqmsg[12]={0x0b,0x22,0x20,rpmsg[3],rpmsg[4],rpmsg[5],rpmsg[6],rpmsg[7],rpmsg[8],rpmsg[9],rpmsg[10],0x00};

				count = 0;

				for(i=0; i<12; i++)
				{
					UART0_WAIT_AND_SEND(rqmsg[i]);
				}

				// Receive 'Read single block' Command Response Packet (6 byte)
				_delay_ms(100); // ����ð� �ο�

 				if(rpmsg[0] != 0x05) // Packet error!
				{
					CLCD_CMD(0x01);	//Clear display and place cursor at home in CLCD
					CLCD_STRING("Packet error!");
				}
				else if(rpmsg[0]==0x05) // Packet normal!
				{
					// 7) Check balance(ī�� �ܾ�)
					balance = rpmsg[2]; // rpmsg[2] ~ rpmsg[5] : �±�ī���� Block number 0x00�� 4�� ����Ʈ ����, rpmsg[2]�� ���..
					if(balance >= fare) // ī�� �ܾ��� �����ݺ��� ������?
					{
						balance = balance - fare; // �ܾ� ����

						// 8) Send 'Write single block' Command Packet(16 byte) for updating blance : Block number was selected 0x00 before as example
						unsigned char rqmsg[16]={0x0f,0x22,0x21,temp_UID[7],temp_UID[6],temp_UID[5],temp_UID[4],temp_UID[3],temp_UID[2],temp_UID[1],temp_UID[0],0x00,balance,0,0,0};
						// ����: �ʸ��� Ĩ -> 0x22 / TIĨ -> 0x62

						count = 0; 

						for(i=0; i<16; i++)
						{
							UART0_WAIT_AND_SEND(rqmsg[i]);
						}

						// Receive 'Write single block' Command Response Packet (2 byte)
						_delay_ms(200); // ����ð� �ο�

						if((rpmsg[0]==0x01) && (rpmsg[1]==0x00))// Packet normal!
						{
							// 9) Jobs for normal processed
							CLCD_CMD(0x01);	//Clear display and place cursor at home in CLCD
							CLCD_STRING("Paid: 15 Won !!");
							CLCD_CMD(0xC0);	// Cursor at 2nd line in CLCD
							CLCD_STRING("Balance: ");

							unsigned char s[4] = {0,0,0,0}; // �ܱݾ��� �ƽ�Ű ���ڷ� ������ 4�ڸ� ����
							utoa(balance, (char *)s, 10); // ���ڸ� ���ڷ� ��ȯ
							CLCD_CHAR(s[0]); if(balance>=10) CLCD_CHAR(s[1]); if(balance>=100) CLCD_CHAR(s[2]); CLCD_STRING(" Won"); 

							memcpy(old_UID,temp_UID,8); // old_UID = temp_UID

							// DB update
							{
								index += index;

								for(i=0;i<8;i++)
								{
									database[index][i] = temp_UID[i];
								}
								database[index][8] = fare;
								database[index][9] = terminal_ID;
							}
						}
 						else // Packet error!
						{
							CLCD_CMD(0x01);	//Clear display and place cursor at home in CLCD
							CLCD_STRING("Packet error!");									
						}
					}

					else // ī�� �ܾ� ����
					{
						CLCD_CMD(0x01);	//Clear display and place cursor at home in CLCD
						CLCD_STRING("Not enough Money");
						CLCD_CMD(0xC0);	// Cursor at 2nd line in CLCD
						CLCD_STRING("Balance: ");

						unsigned char s[4] = {0,0,0,0}; // �ܱݾ��� �ƽ�Ű ���ڷ� ������ 4�ڸ� ����
						utoa(balance, (char *)s, 10); // ���ڸ� ���ڷ� ��ȯ
						CLCD_CHAR(s[0]); if(balance>=10) CLCD_CHAR(s[1]); if(balance>=100) CLCD_CHAR(s[2]); CLCD_STRING(" Won"); 

						memcpy(old_UID,"",8); // old_UID clear
					}
				}
			} // else // temp_UID != old_UID
		} // if(temp_UID[0] == valid_UID[0])

	} // while(1) Loop

} // main()
