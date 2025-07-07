#ifndef PRIVILEGES_H
#define PRIVILEGES_H

typedef enum Privileges {
    PRIVILEGES_ADMIN         = (1 << 0),  // 0x01
    PRIVILEGES_GUEST         = (1 << 1),  // 0x02
    PRIVILEGES_SUPPORT_AGENT = (1 << 2)   // 0x03
} Privileges;

#endif /* PRIVILEGES_H */