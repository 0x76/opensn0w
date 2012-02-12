/*
 * debugsup.h - sort of from ReactOS/Wine.
 */

#ifndef _DEBUG_H_
#define _DEBUG_H_
#define UNIMPLEMENTED    printf("WARNING:  %s at %s:%d is UNIMPLEMENTED!\n",__FUNCTION__,__FILE__,__LINE__);
#define FATAL(fmt, ...)  printf("(%s:%d) FATAL ERROR (Aborting): " fmt, __FILE__, __LINE__, ##__VA_ARGS__), exit(-1);
#define ERR(fmt, ...)    printf("(%s:%d) ERROR: " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define WARN(fmt, ...)   printf("(%s:%d) WARNING: " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define TRACE(fmt, ...)  printf("(%s:%d) TRACE: " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define INFO(fmt, ...)   printf("(%s:%d) INFO: " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define DPRINT(fmt, ...) printf("(%s:%d) " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#endif
