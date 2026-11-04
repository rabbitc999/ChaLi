# Chapter 18 Generics

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../18-泛型.md)
<!-- END LANGUAGE NAVIGATION -->

An algorithm depending on a small set of operations can parameterize the varying types. Generics retain static checking while concrete types, constants and constraints determine valid instantiations.

Distinguish type parameters, integer constants, associated types and generic interfaces, then place control expressions inside generic functions. Explain each parameter's role and its effect on instantiated fields, methods and results.

## 18.1 Parameterize a repeated calculation

<!-- verify-book -->
```chali
T maxOf<T>(T a, T b)
{
    if (a > b)
    {
        return a;
    }
    return b;
}

void main()
{
    println("{}", maxOf(3, 9));
    println("{}", maxOf("apple", "pear"));
}
```

```text
9
pear
```

maxOf returns a when a>b and b otherwise. Calls infer int64 and String respectively. Each concrete type must support the comparison and return paths, yielding 9 and pear. A type parameter alone does not guarantee that every type supports >; constraints make requirements explicit.

## 18.2 Type and constant parameters

A fixed-capacity buffer varies in both element type and slot count. Writing a structure for every combination would repeat its fields and methods, so parameterize those varying parts.

Buffer uses T for its element type and an int64 constant N for its dimension. Both become concrete at use, but they have different roles. Observe them through the data shape and len result below.

<!-- verify-book -->
```chali
struct Buffer<T, N: int64>
{
    T data[N];
    int64 len()
    {
        return N;
    }
};
void main()
{
    Buffer<int64, 4> buffer;
    buffer.data[0] = 7;
    println("{} {}", buffer.len(), buffer.data[0]);
}
```

```text
4 7
```

Buffer's T is an element type parameter and N is an int64 constant parameter. N determines the shape of data[N], rather than being a freely mutable length field. Len returns that type-level capacity.

`Buffer<int64, 4>` supplies integer elements and four slots. Writing slot zero gives the output four and seven. Valid indices are zero through three; the returned length four is not the final index. Different N values affect the instantiated type and layout rather than acting like ordinary runtime assignments.


## 18.3 Express required capabilities

Generic code cannot assume every T supports addition, hashing or a particular field. State required behavior as constraints. Associated types describe dependent types such as a container element type.

<!-- verify-book -->
```chali
interface Container
{
    type Item;
    Item get(int64 index);
};
struct Numbers: Container
{
    type Item = int64;
    Item get(int64 index)
    {
        return index + 10;
    }
};
C.Item first<C: Container>(C value)
{
    return value.get(0);
}
void main()
{
    Numbers numbers;
    println("{}", first<Numbers>(numbers));
}
```

```text
10
```

Container declares an associated type Item, which each implementation defines. Numbers sets it to int64, and get returns index+10, making index zero produce ten.

First's `C: Container` constraint permits calling get. Its `C.Item` return type ties the result to that same implementation's associated type. Main selects Numbers explicitly, establishing an int64 result. An associated type is neither a string containing a type name nor a field that changes arbitrarily at runtime.

Compile-time metadata retains this relationship: Container as implemented by Numbers binds Item to int64, whereas Container's requirement alone is unbound. Chapter 22 shows how to test hasDefinition before generating code that depends on the concrete type.


The interface itself can be generic. Source<int64> specifies integer output, ScoreSource implements it, and the caller uses that interface.

<!-- verify-book -->
```chali
interface Source<T>
{
    T read();
};
struct ScoreSource : Source<int64>
{
    int64 read() { return 42; }
};
void printScore(Source<int64> source)
{
    println("{}", source.read());
}
void main()
{
    ScoreSource source;
    printScore(source);
}
```

```text
42
```

Source<T> parameterizes read's result. ScoreSource implements the concrete interface Source<int64>, so its read must return int64. It returns a fixed 42 rather than reading a file or input.

PrintScore accepts a Source<int64> interface value, and main supplies a matching implementation. Interface dispatch calls read and prints 42. The interface type argument remains fixed: Source<String> is not thereby an interchangeable argument. The shared printing logic need not be duplicated for every concrete implementation.


## 18.4 Returns and control flow

Return types constrain every reachable path. Generic functions follow the same if, match and loop result rules as other functions. Pending inference does not justify missing returns.

<!-- verify-book -->
```chali
T fromLocal<T>(T value)
{
    return
    {
        T local = value;
        yield local;
    };
}

T chooseValue<T>(bool first, T left, T right)
{
    return if (first)
    {
        T local = left;
        yield local;
    }
    else
    {
        yield right;
    };
}

void main()
{
    String word = "generic";
    String copied = fromLocal<String>(word);
    int64 number = chooseValue<int64>(false, 4, 9);
    println("{} {} {}", word, copied, number);
}
```

```text
generic generic 9
```

FromLocal uses T consistently for its parameter, local and result. Instantiating it as String subjects the whole path to string copy, move and cleanup rules. Local yields the block's result, which return passes outward. Word is later printed, so it cannot be invalidated early merely to produce a return value.

`chooseValue<int64>` receives false, skips the left arm and yields right, nine. Both word and copied therefore display generic, followed by nine. Generics reuse program structure while concrete types still govern checking inside every branch; template locals do not become untyped storage.


## 18.5 Instantiation and validation

Concrete instances give the backend type information but can increase compilation work and artifact size. Test the combinations the application uses; one scalar case does not establish behavior for owned values, references or resources. Chapter 22 covers macro type parameters and #res inference.

### Where type arguments are resolved

Type names in generic code retain scope. This example deliberately declares a user type named String, then selects the standard string through its qualified name. They remain distinct types when passed to a generic function.

<!-- verify-book -->
```chali
struct String
{
    int64 code;
}

T echo<T>(T value)
{
    return value;
}

void main()
{
    String token;
    token.code = 7;
    String copied = echo<String>(token);
    std::core::String label = echo<std::core::String>("标签");
    println("{} {}", copied.code, label);
}
```

```text
7 标签
```

The first declaration creates the current module's String with one integer field. The bare name in `echo<String>` resolves at the call site, selecting that user type. The qualified name in the next call selects the standard string for both parameter and result.

Containers, tuples, references and function types preserve the source of their component types. Defaults resolve at the template definition: `Box<T = String>` keeps that module's String when a caller writes `Box<>`; explicit arguments still come from the caller. Qualify ambiguous names introduced by direct imports. Any remains reserved and cannot be redeclared this way.

### Derivation is checked for each concrete instance

A member macro in a generic type generates methods for each concrete instance. The compiler checks the fields after the type arguments are known.

<!-- verify-book -->
```chali
import std::macro;

struct Box<T>
{
    T value;
    $std::macro::Clone<Box<T>>(clone);
}

void main()
{
    Box<int64> number;
    number.value = 7;
    Box<int64> copiedNumber = number.clone();
    println("{}", copiedNumber.value);

    Box<String> text;
    text.value = "kept";
    Box<String> copiedText = text.clone();
    text.value = "changed";
    println("{}", copiedText.value);
}
```

```text
7
kept
```

The first five lines define Box and request Clone. The field still has type T; at the first use it becomes int64, so copiedNumber.value is 7.

The second half selects String. The clone independently owns its text. Changing text.value leaves copiedText.value unchanged, so the second output remains kept. Ordinary copying, explicit cloning and resource transfer retain their usual rules.

A user-defined field type must provide the required Clone protocol. Deriving the outer template does not supply the inner requirement. Modules may each declare Box; each definition retains its own derivation and operator settings. Diagnostics and previews point back to the member macro call, and concrete instances are checked separately.
### Call the same template on two types

The calls infer integer and string types and preserve the respective result types.

<!-- verify-book -->
```chali
T identity<T>(T value) {
    return value;
}
void main() {
    println("{} {}", identity(42), identity("tea"));
}
```

```text
42 tea
```

### Summary

Type parameters choose types, constants can determine array shape, constraints permit operations and associated types connect implementation choices to results. These are distinct static relationships rather than interchangeable runtime data.

Instances retain copy, move, borrow and cleanup rules. Substitute the current arguments when diagnosing a template, then inspect the resulting path as ordinary code. Chapter 19 passes varying behavior as function values.

## 18.6 Named Factories in Generic Structs

<!-- verify-book -->
```chali
struct Cell<T>
{
    T value;
    static Cell<T> create(T value)
    {
        self.value = value;
        return self;
    }
};
void main()
{
    let cell = Cell::create((int64)42);
    println("{}", cell.value);
}
```

```text
42
```

Cell<T> uses T for both its field and factory argument. create writes self.value and returns self, producing an initialized owned result rather than a dangling reference. The int64 argument determines T. Read back 42 to verify initialization; a factory need not be named new.

## 18.7 Implementing a Propagation Protocol

<!-- verify-book -->
```chali
enum Step: Try<int64, NoneResidual>
{
    Value(int64 value), Empty;
    TryBranch<int64, NoneResidual> branch()
    {
        match (self)
        {
            Step::Value(value) =>
            {
                return TryBranch<int64, NoneResidual>::Continue(value);
            }
            Step::Empty =>
            {
                return TryBranch<int64, NoneResidual>::Break(NoneResidual::None);
            }
        }
        panic("unreachable");
    }
};
Option<int64> addOne(Step step)
{
    int64 value = step?;
    return Option<int64>::Value(value + 1);
}
void main()
{
    println("{} {}", addOne(Step::Value(4)).unwrap(), addOne(Step::Empty).isNone());
}
```

```text
5 1
```

Step separates a usable integer from absence. branch maps its variants to Continue and Break under Try. addOne extracts 4 and returns Value(5), or propagates Empty into None without performing addition. main verifies both paths. The interface, success type and residual conversion must all agree.

Next: [Chapter 19](19-closures.md).
