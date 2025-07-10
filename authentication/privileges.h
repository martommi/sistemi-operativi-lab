#ifndef PRIVILEGES_H
#define PRIVILEGES_H

#define BIT(n) (1 << (n))

typedef enum Privileges {
    PRIVILEGES_ADMIN         = BIT(0),  
    PRIVILEGES_GUEST         = BIT(1),  
    PRIVILEGES_SUPPORT_AGENT = BIT(2)   
} Privileges;

#endif /* PRIVILEGES_H */