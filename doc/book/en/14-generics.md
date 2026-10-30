# Chapter 14 Generics

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../14-泛型.md)
<!-- END LANGUAGE NAVIGATION -->

Use generics when code differs mainly in its data types. Type and constant parameters express that variation, while constraints state which operations are available.

## 14.1 Type and constant parameters

Buffer<T, N> parameterizes the element type and length separately. Supply arguments or let sufficiently specific context infer them. Inference is static, not a runtime type test.

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

## 14.4 Instantiation and validation

Concrete instances give the backend type information but can increase compilation work and artifact size. Test the combinations the application uses; one scalar case does not establish behavior for owned values, references or resources. Chapter 19 covers macro type parameters and #res inference.

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
