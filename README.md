# EasyLang
It is a programming language that has begun to be developed for testing purposes. It is still in development and it is not stable at this time. It contains tiny stack based virtual machine for fast code execution.

![EasyLang](EasyLang.gif)

### Primative Types
1. Integer
2. Double
3. String
4. Boolean
5. Array
6. Dictionary (soon)


### Assignment operations
```
data1 = 1
data2 = true
data3 = .555
data4 = "hello world"
data5 = 1024
data6 = []
```

### Append operations
This operation works for string and array types.
```
array = []
array += "test"
array += true
array += 2000
tmpArray = []
array += tmpArray
```

Result
```
(ARRAY) Size: 3
0. (STRING) test
1. (BOOL) true
2. (INTEGER) 2000
```

### Indexer operations
Fetch value from array or string via index.
This operation works for string and array types.
##### Get item from index
```
array = []
array += "test"
array += true
array += 2000
array!0
```

Result
```
(STRING) test
```

```
text = "test"
text!1
```

Result
```
(STRING) e
```

##### Get items with range
```
array = []
array += "test"
array += true
array += 2000
array!1:2
```

Result
```
(ARRAY) Size: 2
0. (BOOL) true
1. (INTEGER) 2000
```

```
text = "test 123"
text!5:3
```

Result
```
(STRING) 123
```

### Function decleration
##### Function decleration
Singline decleration
```
func HelloWorld()
    return "hello world"

func SumTwoNumber(a, b)
    return a + b
```

Multiline decleration
```

func SumAndPrint(a, b)
{
    sum = a + b
    io::print (sum)
    return sum
}

```

### Function call
Parameterless function call
```
HelloWorld _
HelloWorld()
```

With parameter
```
SumAndPrint(10, 20)
io::print (data1)
```

You can call function with '_' for parameterless function.

### If statement
```
if data1 > 1 then
    io::print ("bigger")
else
    io::print ("opssss")

if data1 == 1 then
    io::print ("equal")
else
    io::print ("not equal")
```

### Loop
```
for i in 0 to 10 then
    io::print (i + data5)

for i in data1 to data5 then
{
    io::print (i)
    io::print (i + data1)
}
```

Fibonacci Code Example.
```
func fibonacci(num)
{
    if num == 1 || num == 2 then
        return 1
    return fibonacci(num - 1) + fibonacci(num - 2)
}

fibonacci(10)
```

Result
```
(INTEGER) 55
```

### Internal packages
1. core
2. array
3. io

### Internal functions
```

#Type Operations
bool core::isEmpty(item)
bool core::isBool(item)
bool core::isInteger(item)
bool core::isDouble(item)
bool core::isBool(item)
bool core::isString(item)
bool core::isArray(item)
bool core::isDictionary(item)

bool core::dumpopcode() // Show all executed opcodes on virtual machine

bool core::toBool(item)
integer core::toInteger(item)
double core::toDouble(item)
bool toBool(core::item)
string core::toString(item)
integer core::length(item)

#Array Operations
void array::addTo(array, item)
void array::addTo(array, index, item)

#IO Operations
void io::print(item ...)
string io::readline()
```


### Virtual Machine
Easy Language source code is interpreting but not converting opcode (yet). I am planning to convert codes to opcode. But it will take a time for developing VM.

##### VM Instruction Set
```
DELETE            : Delete item
HALT              : Stop system

ADD               : Sum last 2 item
SUB               : Subtract last 2 item
MUL               : Multiply the last two item
DIV               : Divide the last two item

EQ                : Pop last 2 item and check for equal then push bool status
NOT_EQ            : Pop last 2 item and check for not equal then push bool status
LT                : Pop last 2 item and check for lower then push bool status
LTE               : Pop last 2 item and check for lower or equal then push bool status
GT                : Pop last 2 item and check for greater then push bool status
GTE               : Pop last 2 item and check for greater or equal then push bool status
AND               : And logic for last 2 item and push bool status
OR                : Or logic for last 2 item and push bool status

CONST_STR         : Push string value to stack
CONST_INT         : Push int value to stack
CONST_INT_0       : Push 0 to stack
CONST_INT_1       : Push 1 to stack
CONST_BOOL_TRUE   : Push true to stack
CONST_BOOL_FALSE  : Push false to stack
CONST_DOUBLE      : Push double value to stack
CONST_DOUBLE_0    : Push 0.0 to stack
CONST_DOUBLE_1    : Push 1.0 to stack

JMP               : Jump to address
JIF               : If last item true then jump to address
IF_EQ             : If last two items are equal then jump to address
JNIF              : If last item false then jump to address

INC               : Increment last item
DEC               : Decrement last item
NEG               : Make negative last item

LOAD              : Load item from memory and push to stack
LOAD_0            : Load item 0 from memory and push to stack
LOAD_1            : Load item 1 from memory and push to stack
LOAD_2            : Load item 2 from memory and push to stack
LOAD_3            : Load item 3 from memory and push to stack
LOAD_4            : Load item 4 from memory and push to stack
LOAD_5            : Load item 5 from memory and push to stack
LOAD_6            : Load item 6 from memory and push to stack
LOAD_7            : Load item 7 from memory and push to stack
LOAD_8            : Load item 8 from memory and push to stack
LOAD_9            : Load item 9 from memory and push to stack
LOAD_10           : Load item 10 from memory and push to stack
LOAD_11           : Load item 11 from memory and push to stack
LOAD_12           : Load item 12 from memory and push to stack
LOAD_13           : Load item 13 from memory and push to stack
LOAD_14           : Load item 14 from memory and push to stack
LOAD_15           : Load item 15 from memory and push to stack
LOAD_16           : Load item 16 from memory and push to stack

STORE             : Save item to memory and pop from stack
STORE_0           : Save item 0 to memory and pop from stack
STORE_1           : Save item 1 to memory and pop from stack
STORE_2           : Save item 2 to memory and pop from stack
STORE_3           : Save item 3 to memory and pop from stack
STORE_4           : Save item 4 to memory and pop from stack
STORE_5           : Save item 5 to memory and pop from stack
STORE_6           : Save item 6 to memory and pop from stack
STORE_7           : Save item 7 to memory and pop from stack
STORE_8           : Save item 8 to memory and pop from stack
STORE_9           : Save item 9 to memory and pop from stack
STORE_10          : Save item 10 to memory and pop from stack
STORE_11          : Save item 11 to memory and pop from stack
STORE_12          : Save item 12 to memory and pop from stack
STORE_13          : Save item 13 to memory and pop from stack
STORE_14          : Save item 14 to memory and pop from stack
STORE_15          : Save item 15 to memory and pop from stack
STORE_16          : Save item 16 to memory and pop from stack

GLOAD             : Load item from global memory and push to stack
GLOAD_0           : Load item 0 from global memory and push to stack
GLOAD_1           : Load item 1 from global memory and push to stack
GLOAD_2           : Load item 2 from global memory and push to stack
GLOAD_3           : Load item 3 from global memory and push to stack
GLOAD_4           : Load item 4 from global memory and push to stack
GLOAD_5           : Load item 5 from global memory and push to stack
GLOAD_6           : Load item 6 from global memory and push to stack
GLOAD_7           : Load item 7 from global memory and push to stack
GLOAD_8           : Load item 8 from global memory and push to stack
GLOAD_9           : Load item 9 from global memory and push to stack
GLOAD_10          : Load item 10 from global memory and push to stack
GLOAD_11          : Load item 11 from global memory and push to stack
GLOAD_12          : Load item 12 from global memory and push to stack
GLOAD_13          : Load item 13 from global memory and push to stack
GLOAD_14          : Load item 14 from global memory and push to stack
GLOAD_15          : Load item 15 from global memory and push to stack
GLOAD_16          : Load item 16 from global memory and push to stack

GSTORE            : Save item to global memory and pop from stack
GSTORE_0          : Save item 0 to global memory and pop from stack
GSTORE_1          : Save item 1 to global memory and pop from stack
GSTORE_2          : Save item 2 to global memory and pop from stack
GSTORE_3          : Save item 3 to global memory and pop from stack
GSTORE_4          : Save item 4 to global memory and pop from stack
GSTORE_5          : Save item 5 to global memory and pop from stack
GSTORE_6          : Save item 6 to global memory and pop from stack
GSTORE_7          : Save item 7 to global memory and pop from stack
GSTORE_8          : Save item 8 to global memory and pop from stack
GSTORE_9          : Save item 9 to global memory and pop from stack
GSTORE_10         : Save item 10 to global memory and pop from stack
GSTORE_11         : Save item 11 to global memory and pop from stack
GSTORE_12         : Save item 12 to global memory and pop from stack
GSTORE_13         : Save item 13 to global memory and pop from stack
GSTORE_14         : Save item 14 to global memory and pop from stack
GSTORE_15         : Save item 15 to global memory and pop from stack
GSTORE_16         : Save item 16 to global memory and pop from stack

DUP               : Dublicate last item and push
POP               : Pop last item from stack
PUSH              : Push item to stack
PRINT             : Print last item

CALL              : Create new stack and jump to address
RETURN            : Dispose current stack and jump to last CALL instruction address

CALL_NATIVE       : Call native(c/c++) method
METHOD_DEF        : Method definition
INITARRAY         : Initialize array with items
INITDICT          : Initialize dictionary with items
INDEX             : Get item from item
INITEMPTYARRAY    : Initialize empty array
APPEND	          : Append item to item

```

##### Vm Usage Example (Fibonacci)
```cpp
/*
func fibonacci(num)
{
    if num == 1 || num == 2 then
        return 1
    return fibonacci(num - 1) + fibonacci(num - 2)
}
*/

vm_system vm;
std::vector<size_t> codes{
    iPUSH, 10, // number
    iCALL, 5,  // jump and create new stack
    iHALT,

    // Init variables
    iSTORE_0, // save number

    /*
    if num = 0 then 
        return 0 
    */
    iLOAD_0,  // get num
    iPUSH, 0,
    iEQ,
    iJIF, 15,
    iPUSH, 0,
    iRETURN,  // return 0

    /*
    if num = 1 then 
        return 1 
    */
    iLOAD, 0, // get num
    iPUSH, 1,
    iEQ,
    iJIF, 25,
    iPUSH, 1,
    iRETURN,  // return 1

    /* 
    left = fibonacci(num - 1)
    */
    iLOAD_0,  // get num
    iPUSH, 1,
    iSUB,     // num - 1
    iCALL, 5, // fibonacci(num - 1)
    
    /*
    right = fibonacci(num - 2) 
    */
    iLOAD_0,  // get num
    iPUSH, 2,
    iSUB,     // num - 2
    iCALL, 5, // fibonacci(num - 1)
    
    /*
    return left + right 
    */
    iADD,     
    iRETURN
};
vm.execute(&codes[0], codes.size());
size_t result = vm.getUInt(); // 55
```