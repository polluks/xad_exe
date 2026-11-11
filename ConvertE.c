/* endian conversion macros */

#define EndGetM32(a)  (((((const unsigned char *) a)[0]) << 24) |  \
                       ((((const unsigned char *) a)[1]) << 16) |  \
                       ((((const unsigned char *) a)[2]) <<  8) |  \
                       ((((const unsigned char *) a)[3])))
#define EndGetM16(a)  (((((const unsigned char *) a)[0]) <<  8) |  \
                       ((((const unsigned char *) a)[1])))
#define EndGetI32(a)  (((((const unsigned char *) a)[3]) << 24) |  \
                       ((((const unsigned char *) a)[2]) << 16) |  \
                       ((((const unsigned char *) a)[1]) <<  8) |  \
                       ((((const unsigned char *) a)[0])))
#define EndGetI16(a)  (((((const unsigned char *) a)[1]) <<  8) |  \
                       ((((const unsigned char *) a)[0])))
