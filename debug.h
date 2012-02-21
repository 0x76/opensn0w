/*
 * debugsup.h - sort of from ReactOS/Wine.
 */

#ifndef _DEBUG_H_
#define _DEBUG_H_

extern int opensn0w_debug_level;
void debug_printf(int dbglevel, char *fmt, ...);

#define DBGFLTR_MISC		0x8
#define DBGFLTR_TRACE		0x7
#define DBGFLTR_INFO		0x6
#define DBGFLTR_DPRINT		0x5
#define DBGFLTR_WARN		0x4
#define DBGFLTR_ERR		0x3
#define DBGFLTR_FATAL		0x2
#define DBGFLTR_RELEASE		0x1

#define UNIMPLEMENTED    debug_printf(DBGFLTR_MISC, "WARNING:  %s at %s:%d is UNIMPLEMENTED!\n",__FUNCTION__,__FILE__,__LINE__);
#define FATAL(fmt, ...)  debug_printf(DBGFLTR_RELEASE, "(%s:%d) FATAL ERROR (Aborting): " fmt, __FILE__, __LINE__, ##__VA_ARGS__), exit(-1);
#define ERR(fmt, ...)    debug_printf(DBGFLTR_ERR, "(%s:%d) ERROR: " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define WARN(fmt, ...)   debug_printf(DBGFLTR_WARN, "(%s:%d) WARNING: " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define TRACE(fmt, ...)  debug_printf(DBGFLTR_TRACE, "(%s:%d) TRACE: " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define INFO(fmt, ...)   debug_printf(DBGFLTR_INFO, "(%s:%d) INFO: " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define DPRINT(fmt, ...) debug_printf(DBGFLTR_DPRINT, "(%s:%d) " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define STATUS(fmt, ...) debug_printf(DBGFLTR_RELEASE, fmt,  ##__VA_ARGS__)
//#define STATUS(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define ERROR	ERR
#endif
