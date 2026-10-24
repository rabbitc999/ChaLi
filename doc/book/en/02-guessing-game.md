# Chapter 2 A guessing game

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../02-猜数字.md)
<!-- END LANGUAGE NAVIGATION -->

A small game connects variables, loops, input and decisions. First separate the game rules from input parsing. The secret and each guess are integers; input text must be converted before comparison. EOF and invalid input are different outcomes and should both have a deliberate response.

The following deterministic core makes the branching behavior easy to inspect:

```chali
void main()
{
    int64 secret = 37;
    int64 guesses[3];
    guesses[0] = 20;
    guesses[1] = 50;
    guesses[2] = 37;
    for (int64 guess in guesses)
    {
        if (guess < secret)
        {
            println("Too small");
        }
        elif (guess > secret)
        {
            println("Too large");
        }
        else
        {
            println("Correct");
            break;
        }
    }
}
```

Once the rules work, replace the fixed guesses with the text-input facilities documented under `std::io` and `std::text`. Check the returned result before using its value. Keep one counter for accepted guesses rather than accidentally counting parse failures as attempts.

Exercise: limit accepted guesses to five, print the remaining attempts and stop cleanly on EOF. The [Chinese chapter](../02-猜数字.md) develops the interactive version in detail. Next: [variables and types](03-variables-types.md).
