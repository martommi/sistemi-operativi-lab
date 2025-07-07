#ifndef PRIVILEGES_H
#define PRIVILEGES_H

#define BIT(n) (1 << (n))

typedef enum Privileges {
    PRIVILEGES_ADMIN         = BIT(0),  // 0x01
    PRIVILEGES_GUEST         = BIT(1),  // 0x02
    PRIVILEGES_SUPPORT_AGENT = BIT(2)   // 0x03
} Privileges;

#endif /* PRIVILEGES_H */