// test_grammar.c - Comprehensive C89 Grammar Test

// 1. Basic Types and Declarations
typedef int MyInt;
typedef struct Point Point;
extern const volatile int global_var;
static unsigned long array[10];

// 2. Storage Classes
auto int auto_var;
register int reg_var;
static int static_var;
extern int extern_var;

// 3. Struct/Union Definitions
struct Point {
    int x, y;
    const double z;
};

union Data {
    int i;
    float f;
    char str[20];
};

// 4. Enums
enum Colors {
    RED,
    GREEN = 20,
    BLUE
};

// 5. Complex Declarators
int *(*fp)(char*, void*);
int (*array_ptr[10])(int);
char (*(*x[3])())[5];

// 6. Function Definitions with Various Parameter Types
void simple_func(void) {
    return;
}

int complex_func(int a, char *b, void (*callback)(void)) {
    return a;
}

// 7. Nested Block Statements
void nested_blocks(void) {
    int a = 1;
    {
        int b = 2;
        {
            int c = 3;
        }
    }
}

// 8. Control Structures
void control_structures(int x) {
    if (x > 0) {
        x++;
    } else if (x < 0) {
        x--;
    } else {
        x = 0;
    }

    switch (x) {
        case 1:
            break;
        case 2:
            return;
        default:
            break;
    }

    while (x > 0) {
        x--;
    }

    do {
        x++;
    } while (x < 10);

    for (int i = 0; i < 10; i++) {
        if (i == 5) continue;
        if (i == 8) break;
    }
}

// 9. Complex Expressions
void expressions(void) {
    int a, b, c;
    
    // Assignment expressions
    a = b = c = 0;
    
    // Conditional expressions
    a = b ? c : d;
    
    // Logical expressions
    a = b && c || d;
    
    // Bitwise expressions
    a = b & c | d ^ e;
    
    // Relational expressions
    a = b < c && c <= d;
    
    // Shift expressions
    a = b << 2 >> 1;
    
    // Additive/multiplicative
    a = b * c / d + e - f;
    
    // Unary expressions
    a = -b + ++c - --d;
    
    // Sizeof expressions
    a = sizeof(int) + sizeof b;
    
    // Cast expressions
    a = (int)(float)(char)b;
    
    // Array/struct/pointer expressions
    a = array[0].x->y[1];
}

// 10. Initializers
int init_array[] = {1, 2, 3, {4, 5}, 6};
struct Point points[] = {{.x=1, .y=2}, {.x=3, .y=4}};

// 11. Typedef and Complex Types
typedef int (*FuncPtr)(void);
typedef struct Node {
    struct Node* next;
    int data;
} Node;

// 12. Function Pointers and Callbacks
void higher_order(void (*callback)(int), int arg) {
    callback(arg);
}

int main(void) {
    return 0;
}