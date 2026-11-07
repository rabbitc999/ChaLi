# Chapter 14 Error Handling

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../14-错误处理.md)
<!-- END LANGUAGE NAVIGATION -->

Option represents presence or absence; Result represents success or failure. Keep these states distinct from legitimate zero or empty values.

## 14.1 A Missing Search Result

<!-- verify-book -->
```chali
Option<int64> findFirst(List<int64> xs, int64 target)
{
    for (int64 x in xs)
    {
        if (x == target)
        {
            return Value(x);
        }
    }
    return None;
}

void main()
{
    List<int64> xs;
    xs.push(3);
    xs.push(7);
    println("{}", findFirst(xs, 7).unwrap());
    println("{}", findFirst(xs, 9).unwrapOr(-1));
    match (findFirst(xs, 3))
    {
        Value(v) => { println("found = {}", v); }
        None => { println("no"); }
    }
}
```

```text
7
-1
found = 3
```

findFirst returns Value on a match and None after exhausting the list. main unwraps a known match, supplies a fallback for a missing value, and handles both cases with match. Unwrapping absence fails; it is not a presence test.

## 14.2 Carry Success Data and Failure Information

<!-- verify-book -->
```chali
Result<int64, Error> checkedScore(int64 value)
{
    if (value < 0 || value > 100)
    {
        return MessageError::from("invalid score");
    }
    return Ok(value);
}

Result<int64, Error> addBonus(int64 value)
{
    int64 score = checkedScore(value)?;
    return Ok(score + 5);
}

void main()
{
    match (addBonus(70))
    {
        Ok(score) => { println("{}", score); }
        Err(error) => { println("{}", error.toString()); }
    }
    match (addBonus(-1))
    {
        Ok(score) => { println("{}", score); }
        Err(error) => { println("{}", error.toString()); }
    }
}
```

```text
75
invalid score
```

checkedScore accepts zero through one hundred. It otherwise constructs MessageError and returns it through Error. The common toString interface is enough to display it; Chapter 17 explains concrete implementations.

main prints 75 for the valid input and invalid score for the rejected input. An error is not a successful score or a magic sentinel integer.

The declared Result<int64, Error> return type selects its unique matching Err payload automatically, so the error object can be returned directly. Initialization, assignment and arguments with known target types follow the same rule, including Option and custom single-payload enum variants. Equal matches require an explicit variant: a String cannot choose between Ok and Err in Result<String, String>. Keeping Ok on successful returns can still make the two paths easier to read.

## 14.3 Propagate When Computation Cannot Continue

checkedScore(value)? extracts an Ok payload or immediately returns Err from addBonus. The latter path never adds five. Propagation neither repairs nor retries the operation; main decides how to display failure.

The enclosing return type must support the protocol. A void main cannot arbitrarily propagate Result. Option propagates absence through its own protocol. Early returns still perform established scope cleanup.

## 14.4 Recover Where a Decision Can Be Made

Libraries return Result so callers can retry, prompt or stop. Converting every file error into empty text would erase the difference between an empty file and a failed read. The log project attaches the path and lets its entry report failure. Concrete interface payloads and custom Try move to Chapters 17 and 18.

### Keep the parse failure branch

Valid and invalid inputs yield success and failure, respectively.

<!-- verify-book -->
```chali
import std::json;

void main() {
    println("{} {}", parse("42").isOk(), parse("{").isErr());
}
```

```text
1 1
```
