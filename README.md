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
array <+ "test"
array <+ true
array <+ 2000
tmpArray = []
array <+ tmpArray
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
array <+ "test"
array <+ true
array <+ 2000
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
array <+ "test"
array <+ true
array <+ 2000
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
func HelloWorld():string
    return "hello world"

func SumTwoNumber(a:int, b:int):int
    return a + b
```

Multiline decleration
```

func SumAndPrint(a:int, b:int):int
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
func fibonacci(num:int):int
{ 
    if num == 0 then 
        return 0

    if num == 1 then 
        return 1 
        
    left = fibonacci(num - 1)
    right = fibonacci(num - 2) 
    return left + right 
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
iADD    : Sum last 2 int item
iSUB    : Subtract last 2 int item
iMUL    : Multiply the last two int item
iDIV    : Divide the last two int item

dADD    : Sum last 2 double item
dSUB    : Subtract last 2 double item
dMUL    : Multiply the last two double item
dDIV    : Divide the last two double item

//LOGIC
EQ      : Is the last two item equal
LT      : Little control for last two item
LTE     : Little or equal control for last two item
GT      : Greater control for last two item
GTE     : Greater or equal control for last two item
AND     : And operator
OR      : Or operator

DUP     : Dublicate last item
POP     : Remove last item

iPUSH   : Push int
dPUSH   : Push double
bPUSH   : Push bool
sPUSH   : Push string
iPUSH_0 : Push int 0
iPUSH_1 : Push int 1
iPUSH_2 : Push int 2
iPUSH_3 : Push int 3
iPUSH_4 : Push int 4
dPUSH_0 : Push double 0
dPUSH_1 : Push double 1
dPUSH_2 : Push double 2
dPUSH_3 : Push double 3
dPUSH_4 : Push double 4
bPUSH_0 : Push false
bPUSH_1 : Push true

// JUMP AND IF
JMP     : Jump to address
JIF     : If last item true then jump to address
IF_EQ   : If last two items are equal then jump to address
JNIF    : If last item false then jump to address

INC     : Increment last item
DEC     : Decrement last item

//FOR LOCAL VARIABLE 
LOAD    : Load item from memory and push to stack
LOAD_0  : Load item 0 from memory and push to stack
LOAD_1  : Load item 1 from memory and push to stack
LOAD_2  : Load item 2 from memory and push to stack
LOAD_3  : Load item 3 from memory and push to stack
LOAD_4  : Load item 4 from memory and push to stack
STORE   : Save item to memory and pop from stack
STORE_0 : Save item 0 to memory and pop from stack
STORE_1 : Save item 1 to memory and pop from stack
STORE_2 : Save item 2 to memory and pop from stack
STORE_3 : Save item 3 to memory and pop from stack
STORE_4 : Save item 4 to memory and pop from stack

//FOR GLOBAL VARIABLE 
GLOAD   : Load item from global memory and push to stack
GLOAD_0 : Load item 0 from global memory and push to stack
GLOAD_1 : Load item 1 from global memory and push to stack
GLOAD_2 : Load item 2 from global memory and push to stack
GLOAD_3 : Load item 3 from global memory and push to stack
GLOAD_4 : Load item 4 from global memory and push to stack
GSTORE  : Save item to global memory and pop from stack
GSTORE_0: Save item 0 to global memory and pop from stack
GSTORE_1: Save item 1 to global memory and pop from stack
GSTORE_2: Save item 2 to global memory and pop from stack
GSTORE_3: Save item 3 to global memory and pop from stack
GSTORE_4: Save item 4 to global memory and pop from stack

//CONVERT
I2D     : Int to double
D2I     : Double to int
I2B     : Int to bool
B2I     : Bool to int
D2B     : Double do bool
B2D     : Bool to double

CALL    : Create new stack and jump to address
RETURN  : Dispose current stack and jump to last CALL instruction address

PRINT   : Print last item
HALT    : Stop system
```

##### Vm Usage Example (Fibonacci)
```cpp
/*
func fibonacci(num:int):int
{ 
    if num == 0 then 
        return 0

    if num == 1 then 
        return 1 
        
    left = fibonacci(num - 1)
    right = fibonacci(num - 2) 
    return left + right 
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