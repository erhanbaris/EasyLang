# EasyLang
It is a programming language that has begun to be developed for testing purposes. It is still in development and it is not stable at this time. It contains tiny stack based virtual machine for fast code execution.

![EasyLang](EasyLang.gif)

### Primative Types
1. Integer
2. Double
3. Text
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
    if num <= 1 then 
        return 1 
        
    left = fibonacci(num - 1)
    right = fibonacci(num - 2) 
    return left + right 
}

fibonacci(10)
```

Result
```
(INTEGER) 89
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
iADD    : Sum last 2 item
iSUB    : Subtract last 2 item
iMUL    : Multiply the last two item
iDIV    : Divide the last two item
iEQ     : Is the last two item equal
iLT     : Little control for last two item
iLTE    : Little or equal control for last two item
iGT     : Greater control for last two item
iGTE    : Greater or equal control for last two item
iAND    : And operator
iOR     : Or operator
iDUP    : Dublicate last item
iPOP    : Remove last item
iJMP    : Jump to address
iJIF    : If last item true then jump to address
iJNIF   : If last item false then jump to address
iINC    : Increment last item
iDEC    : Decrement last item
iLOAD   : Load item from memory and push to stack
iSTORE  : Save item to memory and pop from stack
iCALL   : Create new stack and jump to address
iRETURN : Dispose current stack and jump to last CALL instruction address
iPUSH   : Push to stack
iPRINT  : Print last item
iHALT   : Stop system
```

##### Vm Usage Example
```cpp
vm_system vm;

/*
//Destination code
size_t test(size_t total)
{
    size_t i = 0;
    for (; i < total; ++i) { }
    return i;
}
size_t result = test(100);
*/

vm.execute({
    vm_inst::iPUSH, 100, // total
    vm_inst::iCALL, 5, // jump and create new stack
    vm_inst::iHALT,
    
    // Init variables
    vm_inst::iSTORE, 0, // save total
    vm_inst::iLOAD, 0, // get total to stack
    vm_inst::iPUSH, 0, // i = 0
    vm_inst::iSTORE, 1, // save i
    vm_inst::iLOAD, 1,
    vm_inst::iGT,
    vm_inst::iJIF, 29,
    vm_inst::iLOAD, 0,
    vm_inst::iLOAD, 1,
    vm_inst::iINC, // ++i
    vm_inst::iSTORE, 1,
    vm_inst::iLOAD, 1,
    vm_inst::iJMP, 15, // loop
    vm_inst::iLOAD, 1, // return i value
    vm_inst::iRETURN // clear stack
});

size_t i = vm.getUInt(); // 100
```