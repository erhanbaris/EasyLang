# EasyLang
It is a programming language that has begun to be developed for testing purposes.

![EasyLang](EasyLang.gif)

###Primative Types
1. Integer
2. Double
3. Text
4. Boolean
5. Array
6. Dictionary (soon)


###Assignment operations
```
data1 = 1
data2 = true
data3 = .555
data4 = "hello world"
data5 = 1024
data6 = []
```

###Function descleration
```
func HelloWorld()
    return "hello world"

func SumTwoNumber(a, b)
    return a + b

func SumAndPrint(a, b)
{
    sum = a + b
    print (sum)
    return sum
}
```

###Function call
```
HelloWorld()
HelloWorld _
SumAndPrint(10, 20)

io::print data1
io::print (data1, data2, data3, data4, data5)
```
You can call function with '_' for parameterless function.

###If statement
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

###Loop
```
i -> 0..100 
    io::print i + data5

i -> data1..data5 
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

###Internal packages
1. core
2. array
3. io

###Internal functions
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

#Array Operations
void array::addTo(array, item)
void array::addTo(array, index, item)

#IO Operations
void io::print(item ...)
string io::readline()
```
