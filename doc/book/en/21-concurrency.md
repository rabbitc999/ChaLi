# Chapter 21 Concurrency and Threads

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../21-并发与线程.md)
<!-- END LANGUAGE NAVIGATION -->

Independent work can run in threads; network waits can also be interleaved in one thread. Begin with task lifetime before choosing an execution model.

## 21.1 Create and Join a Task

<!-- verify-book -->
```chali
import std::thread;

void worker()
{
    println("worker");
}

void main()
{
    Thread task = create(worker);
    wait(task);
    println("done");
}
```

```text
worker
done
```

The import supplies the thread API. worker is a static entry with no parameters. create starts the task and returns a handle; scheduling may start before or after create returns. wait establishes the output order by joining the task before main prints done. Do not join the same handle twice.

## 21.2 Supply Initial Data

<!-- verify-book -->
```chali
import std::thread;

void report(int64 score)
{
    println("score:{}", score);
}

void main()
{
    int64 score = 70;
    Thread task = createWith(report, score);
    score = 90;
    wait(task);
    println("main:{}", score);
}
```

```text
score:70
main:90
```

createWith transfers initial data under the cross-thread copy rules. The task receives 70; changing the caller's variable to 90 does not update that copy. wait establishes the final output order.

References, resources and borrowed closures are not arbitrary transferable arguments. Threads use isolated contexts. A copyable configuration struct can carry several ordinary values.

## 21.3 Communicate with Messages

<!-- verify-book -->
```chali
import std::thread;

void receiver()
{
    String message = receive();
    println("received:{}", message);
}

void main()
{
    Thread task = create(receiver);
    send(task, "ready");
    wait(task);
    println("joined");
}
```

```text
received:ready
joined
```

receiver waits on its inbox instead of polling a caller-local variable. main sends ready, the task prints it and finishes, and wait allows main to print joined afterward.

Longer protocols need an explicit end condition. An empty message is not a closed channel. Use `std::concurrency::Channel<T>` when typed messages and close states are needed; consult the library reference for signatures.

### Claim shared state with compare-and-exchange

`AtomicInt64::create(0)` creates a shared integer slot. Passing its handle through `createWith` gives the task access to that same slot. `compareExchange(expected, desired)` performs the comparison and write as one atomic operation. A match writes `desired` and returns `true`. A mismatch returns `false`. With a separate `get()` and `set()`, another thread could claim the same work between the calls.

The example uses `0` for unclaimed, and `1` and `2` for the two claimants. The main thread collects both results through a channel and uses `wait` to join the tasks. Scheduling determines which task wins, so the program checks the number of successes and the allowed final values.

A single comparison protects this integer slot. Other fields updated after a successful claim need their own synchronization protocol. On failure, exit, reload, or retry according to the protocol. If a slot can change from `0` to another value and back to `0`, the comparison sees its current value and cannot detect the intervening changes.

<!-- verify-book -->
```chali
import std::concurrency;
import std::thread;

struct Claim {
    AtomicInt64 state;
    Channel<bool> outcomes;
    int64 id;
};

void claim(Claim job) {
    bool won = job.state.compareExchange(0, job.id);
    assert(job.outcomes.send(won) == READY);
}

void main() {
    Claim job;
    job.state = AtomicInt64::create(0);
    job.outcomes = Channel<bool>::create(2);
    job.id = 1;
    Thread first = createWith(claim, job);
    job.id = 2;
    Thread second = createWith(claim, job);

    let a = job.outcomes.receive();
    let b = job.outcomes.receive();
    assert(a.isReady() && b.isReady() && a.value != b.value);
    wait(first);
    wait(second);
    assert(job.state.get() == 1 || job.state.get() == 2);
    job.outcomes.close();
    println("one claimant won");
}
```

```text
one claimant won
```

## 21.4 Timeout and Cancellation

waitFor(task, millis) uses a nonnegative millisecond limit. true means completion; false means timeout without consuming the handle. tryWait uses a zero limit. A timed-out task still needs lifecycle management.

requestStop is cooperative. A task responds through stopRequested or cancellable waits; it is not forcibly killed during an arbitrary update. waitResult and waitResultFor expose status, error and trace. ThreadGroup can manage joining and cancelling several tasks.

### Share one monotonic deadline across several waits

Passing `250` milliseconds to each of two waits lets each call spend that much time. To share a budget, create a `Duration` with `Duration::fromMillis(250)` and compute a deadline with `Instant::now().plus(budget)`. Call `deadline.remaining().toMillisCeil()` before each timed operation. `Instant` comes from a monotonic clock and measures elapsed time or deadlines. The clock origin is platform-dependent. Use `std::date_time` for dates and wall-clock times.

`Duration::fromNanos` and `Duration::fromMillis` accept nonnegative integers. A millisecond value whose nanosecond conversion exceeds `int64` triggers an assertion. `Instant.plus` saturates at the largest representable instant. `end.durationSince(start)` returns a nonnegative interval, with zero for reversed endpoints. `start.elapsed()` uses the current instant as its endpoint. `deadline.remaining()` uses the current instant as its starting point and returns zero once the deadline arrives.

Millisecond APIs take whole numbers. `toMillisCeil()` rounds a positive remainder smaller than one millisecond up to one, so it does not become a zero-time attempt early. The rounding adds less than one millisecond. Zero milliseconds means an immediate attempt. If your protocol must stop accepting results at the deadline, check for zero remaining time first. The example queues both messages ahead of time to give deterministic output. Each receive still derives its timeout from the same deadline.

<!-- verify-book -->
```chali
import std::clock;
import std::concurrency;

void main() {
    let replies = Channel<int64>::create(2);
    assert(replies.send(10) == READY);
    assert(replies.send(20) == READY);
    replies.close();

    let start = Instant::now();
    let budget = Duration::fromMillis(250);
    let deadline = start.plus(budget);

    let first = replies.receiveFor(deadline.remaining().toMillisCeil());
    let second = replies.receiveFor(deadline.remaining().toMillisCeil());
    assert(first.isReady() && second.isReady());
    assert(start.remaining().toNanos() == 0);
    assert(Duration::fromNanos(1).toMillisCeil() == 1);
    println("sum={}", first.value + second.value);
}
```

```text
sum=30
```

## 21.5 Choose Around Ownership and Waiting

The HTTP server's sockets belong to their creating context and cannot simply be copied through createWith. Chapter 28 interleaves nonblocking connections on one thread. A slow connection does not block other clients, but CPU-heavy work is not thereby parallelized across cores.

### Use a wait to order output

The main thread prints the last line after the task ends.

<!-- verify-book -->
```chali
import std::thread;

void task() {
    println("finished");
}
void main() {
    Thread worker = create(task);
    wait(worker);
    println("joined");
}
```

```text
finished
joined
```
