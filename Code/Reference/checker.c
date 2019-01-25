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
//* UART 관련 매크로                                                   *//
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
// UART0 초기화 함수
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
// UART1 초기화 함수
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
// Baud rate codes for use with the INIT_UART# macro (XTAL=16MHz, U2X=1모드)
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
//* CLCD 관련 함수                                                     *//
//**********************************************************************//
// 아래의 CLCD 관련 설정은 다음의 포트 설정을 기준으로 했으며,
// 각자의 상황에 맞게 적절히 수정하여 사용...
//
// 사용한 CLCD모델: HY-1602H-803 혹은 호환품(16x2)
//
// CLCD RW핀 -> PORTB 핀0
// CLCD CE핀 -> PORTB 핀1
// CLCD RS핀 -> PORTB 핀2
//
// CLCD D0핀 -> PORTE 핀0
// CLCD D1핀 -> PORTE 핀1
// CLCD D2핀 -> PORTE 핀2
// CLCD D3핀 -> PORTE 핀3
// CLCD D4핀 -> PORTE 핀4
// CLCD D5핀 -> PORTE 핀5
// CLCD D6핀 -> PORTE 핀6
// CLCD D7핀 -> PORTE 핀7

#define RW 0
#define CE 1
#define RS 2

void CLCD_CHAR (unsigned char value) // CLCD 한 문자 쓰기 함수
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

void CLCD_CMD (unsigned char value) // CLCD Command 쓰기 함수
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

void CLCD_INIT (void) // CLCD 초기화 함수
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

void CLCD_STRING(char *value) // CLCD 문자열 출력 함수
{
	while(*value != '\0')
	{ 	
		CLCD_CHAR(*value);
		value++;
	}
}

void CLCD_CURSOR(unsigned char x, unsigned char y) // CLCD 커서 위치 지정 함수
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
//* UART 수신 (인터럽트 방식)                                          *//
//**********************************************************************//

volatile unsigned char rpmsg[11], count;

// UART0 수신 인터럽트
ISR(USART0_RX_vect)
{
	rpmsg[count] = UDR0;
	count=count+1;
}

// UART1 수신 인터럽트
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
//* IO포트 및 변수 초기화                                              *//
//**********************************************************************//

	DDRB = BM(CE) | BM(RW) | BM(RS); // CLCD 컨트롤 신호(출력)
	DDRE = 0xFF; // CLCD 데이터 신호(출력)

	PORTB &= ~BM(CE) & ~BM(RW) & ~BM(RS); // CLCD 컨트롤 신호 초기 출력(0)
	PORTE = 0x00; // CLCD 데이터 신호 초기 출력(0)

	CLCD_INIT();

	INIT_UART0(UART_BAUDRATE_115K2,UART_OPT_8_BITS_PER_CHAR); // UART0 115.2Kbps at 16MHz X-tal

	ENABLE_UART0();
	ENABLE_UART0_RX_INT();
	ENABLE_GLOBAL_INT();

	terminal_ID=100; // 카드 단말기 ID, 임의로 설정..
	fare = 15; // 교통카드 1회 사용(통행) 요금 설정(ex, 15원)

	count = 0; // 인터럽트 방식 수신 인덱스 초기화

	// 이전 UID와 UID Valid값 저장 버퍼 초기화
	for(i=0; i<=7;i++)
	{
		old_UID[i] = 0;
		valid_UID[i] = 0;
	}

	valid_UID[0] = 0xE0; // ISO15693 RFID 태그는 최상위 바이트가 항상 0xE0 이다.

	// CLCD 초기화면
	CLCD_CMD(0x01);	//Clear display and place cursor at home in CLCD
	CLCD_STRING("RFID-232 Example");
	CLCD_CMD(0xC0);	// Cursor at 2nd line in CLCD
	CLCD_STRING("Traffic IC Card");

	while(1)
	{
		// 1) Send 'InventoryAll' Command Packet(3 byte)
		unsigned char rqmsg[3] = {0x02, 0x00, 0xf0};
		rpmsg[0] = 0x00; // 초기화

		while(rpmsg[0] != 0x0a)
		{
			count = 0;

			for(i=0; i<3; i++)
			{
				UART0_WAIT_AND_SEND(rqmsg[i]);
			}

			// Receive 'Inventory' Command Response Packet (12 byte)
			_delay_ms(100); // 응답시간 부여
		}

		// 2) If UID detected, temp_UID = UID
		for(i=3; i<=10;i++) // UID 파싱
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
				_delay_ms(100); // 응답시간 부여

 				if(rpmsg[0] != 0x05) // Packet error!
				{
					CLCD_CMD(0x01);	//Clear display and place cursor at home in CLCD
					CLCD_STRING("Packet error!");
				}
				else if(rpmsg[0]==0x05) // Packet normal!
				{
					// 7) Check balance(카드 잔액)
					balance = rpmsg[2]; // rpmsg[2] ~ rpmsg[5] : 태그카드의 Block number 0x00의 4개 바이트 내용, rpmsg[2]만 사용..
					if(balance >= fare) // 카드 잔액이 교통요금보다 많은가?
					{
						balance = balance - fare; // 잔액 갱신

						// 8) Send 'Write single block' Command Packet(16 byte) for updating blance : Block number was selected 0x00 before as example
						unsigned char rqmsg[16]={0x0f,0x22,0x21,temp_UID[7],temp_UID[6],temp_UID[5],temp_UID[4],temp_UID[3],temp_UID[2],temp_UID[1],temp_UID[0],0x00,balance,0,0,0};
						// 주의: 필립스 칩 -> 0x22 / TI칩 -> 0x62

						count = 0; 

						for(i=0; i<16; i++)
						{
							UART0_WAIT_AND_SEND(rqmsg[i]);
						}

						// Receive 'Write single block' Command Response Packet (2 byte)
						_delay_ms(200); // 응답시간 부여

						if((rpmsg[0]==0x01) && (rpmsg[1]==0x00))// Packet normal!
						{
							// 9) Jobs for normal processed
							CLCD_CMD(0x01);	//Clear display and place cursor at home in CLCD
							CLCD_STRING("Paid: 15 Won !!");
							CLCD_CMD(0xC0);	// Cursor at 2nd line in CLCD
							CLCD_STRING("Balance: ");

							unsigned char s[4] = {0,0,0,0}; // 잔금액을 아스키 문자로 저장할 4자리 공간
							utoa(balance, (char *)s, 10); // 숫자를 문자로 변환
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

					else // 카드 잔액 부족
					{
						CLCD_CMD(0x01);	//Clear display and place cursor at home in CLCD
						CLCD_STRING("Not enough Money");
						CLCD_CMD(0xC0);	// Cursor at 2nd line in CLCD
						CLCD_STRING("Balance: ");

						unsigned char s[4] = {0,0,0,0}; // 잔금액을 아스키 문자로 저장할 4자리 공간
						utoa(balance, (char *)s, 10); // 숫자를 문자로 변환
						CLCD_CHAR(s[0]); if(balance>=10) CLCD_CHAR(s[1]); if(balance>=100) CLCD_CHAR(s[2]); CLCD_STRING(" Won"); 

						memcpy(old_UID,"",8); // old_UID clear
					}
				}
			} // else // temp_UID != old_UID
		} // if(temp_UID[0] == valid_UID[0])

	} // while(1) Loop

} // main()
