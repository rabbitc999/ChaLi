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
    Thread task = std::thread::create(worker);
    std::thread::wait(task);
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
    Thread task = std::thread::createWith(report, score);
    score = 90;
    std::thread::wait(task);
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
    String message = std::thread::receive();
    println("received:{}", message);
}

void main()
{
    Thread task = std::thread::create(receiver);
    std::thread::send(task, "ready");
    std::thread::wait(task);
    println("joined");
}
```

```text
received:ready
joined
```

receiver waits on its inbox instead of polling a caller-local variable. main sends ready, the task prints it and finishes, and wait allows main to print joined afterward.

Longer protocols need an explicit end condition. An empty message is not a closed channel. Use `std::concurrency::Channel<T>` when typed messages and close states are needed; consult the library reference for signatures.

## 21.4 Timeout and Cancellation

waitFor(task, millis) uses a nonnegative millisecond limit. true means completion; false means timeout without consuming the handle. tryWait uses a zero limit. A timed-out task still needs lifecycle management.

requestStop is cooperative. A task responds through stopRequested or cancellable waits; it is not forcibly killed during an arbitrary update. waitResult and waitResultFor expose status, error and trace. ThreadGroup can manage joining and cancelling several tasks.

## 21.5 Choose Around Ownership and Waiting

The HTTP server's sockets belong to their creating context and cannot simply be copied through createWith. Chapter 28 interleaves nonblocking connections on one thread. A slow connection does not block other clients, but CPU-heavy work is not thereby parallelized across cores.

## 21.6 Exercises

1. Which operation guarantees that done prints last?
2. Why does changing score not change the task argument?
3. Can a timed-out handle be abandoned?
4. Why not copy a socket into createWith?

### Answers and checks

1. wait joins the task before the final output.
2. Initial data follows cross-thread copying, without sharing the caller's local variable.
3. No; timeout does not consume it.
4. A socket is an owned resource, not freely copyable initial data across isolated contexts.
