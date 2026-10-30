# Chapter 14 Generics

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../14-泛型.md)
<!-- END LANGUAGE NAVIGATION -->

An algorithm depending on a small set of operations can parameterize the varying types. Generics retain static checking while concrete types, constants and constraints determine valid instantiations.

Distinguish type parameters, integer constants, associated types and generic interfaces, then place control expressions inside generic functions. Explain each parameter's role and its effect on instantiated fields, methods and results.

## 14.1 Type and constant parameters

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

### Code walkthrough

Buffer's T is an element type parameter and N is an int64 constant parameter. N determines the shape of data[N], rather than being a freely mutable length field. Len returns that type-level capacity.

`Buffer<int64, 4>` supplies integer elements and four slots. Writing slot zero gives the output four and seven. Valid indices are zero through three; the returned length four is not the final index. Different N values affect the instantiated type and layout rather than acting like ordinary runtime assignments.


## 14.2 Express required capabilities

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

### Code walkthrough

Container declares an associated type Item, which each implementation defines. Numbers sets it to int64, and get returns index+10, making index zero produce ten.

First's `C: Container` constraint permits calling get. Its `C.Item` return type ties the result to that same implementation's associated type. Main selects Numbers explicitly, establishing an int64 result. An associated type is neither a string containing a type name nor a field that changes arbitrarily at runtime.


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

### Code walkthrough

Source<T> parameterizes read's result. ScoreSource implements the concrete interface Source<int64>, so its read must return int64. It returns a fixed 42 rather than reading a file or input.

PrintScore accepts a Source<int64> interface value, and main supplies a matching implementation. Interface dispatch calls read and prints 42. The interface type argument remains fixed: Source<String> is not thereby an interchangeable argument. The shared printing logic need not be duplicated for every concrete implementation.


## 14.3 Returns and control flow

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

### Code walkthrough

FromLocal uses T consistently for its parameter, local and result. Instantiating it as String subjects the whole path to string copy, move and cleanup rules. Local yields the block's result, which return passes outward. Word is later printed, so it cannot be invalidated early merely to produce a return value.

`chooseValue<int64>` receives false, skips the left arm and yields right, nine. Both word and copied therefore display generic, followed by nine. Generics reuse program structure while concrete types still govern checking inside every branch; template locals do not become untyped storage.


## 14.4 Instantiation and validation

Concrete instances give the backend type information but can increase compilation work and artifact size. Test the combinations the application uses; one scalar case does not establish behavior for owned values, references or resources. Chapter 19 covers macro type parameters and #res inference.

### Summary

Type parameters choose types, constants can determine array shape, constraints permit operations and associated types connect implementation choices to results. These are distinct static relationships rather than interchangeable runtime data.

Instances retain copy, move, borrow and cleanup rules. Substitute the current arguments when diagnosing a template, then inspect the resulting path as ordinary code. Chapter 15 passes varying behavior as function values.

## 14.5 Exercises

1. Identify what each Buffer parameter controls.
2. How do constraints differ from casts?
3. Add a branch to a generic return example.
4. Choose three element types for a generic collection test.

### Answers and checks

1. T selects the element type and N the constant dimension.
2. Constraints state capabilities; a cast does not implement them.
3. Ensure the added path returns the declared type.
4. For example an integer, String and a resource-bearing struct, checking copy and cleanup rules.

Next: [Chapter 15](15-functions-closures.md).
