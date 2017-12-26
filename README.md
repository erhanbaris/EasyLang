# EasyLang
It is a programming language that has begun to be developed for testing purposes.

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
This operation works for string and array types.
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
io::print (data1, data2, data3, data4, data5)
```

Also single parameter can call without parentheses
```
io::print data1
```

You can call function with '_' for parameterless function.

### If statement
```
if data1 > 1 then
    io::print "bigger"
else
    io::print "opssss"

if data1 == 1 then
    io::print "equal"
else
    io::print "not equal"
```

### Loop
```
for i in 0 to 10 then
    io::print i + data5

for i in data1 to data5 then
{
    io::print i
    io::print (i, data1)
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
