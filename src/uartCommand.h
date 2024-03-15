/* 
 * File:   uartCommand.h
 * 
 * 2024.03.15
 * 
 */

#ifndef UART_COMMAND_H
#define	UART_COMMAND_H

void uartCommand_Init(void);
void    uartComandCheck(void);
void    debuggerComand(uint8_t*);
void    tamamoniCommandCheck(uint8_t*); 


#endif //UART_COMMAND_H