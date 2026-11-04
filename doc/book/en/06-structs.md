# Chapter 6 Structs

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../06-结构体.md)
<!-- END LANGUAGE NAVIGATION -->

A student has a name and a score. Keeping them as unrelated variables makes it easier to pair one person's name with another person's score. A struct groups those related values.

## 6.1 Group fields

<!-- verify-book -->
```chali
struct Student
{
    String name;
    int64 score;
}

void main()
{
    Student student;
    student.name = "Ada";
    student.score = 70;
    println("{} {}", student.name, student.score);
}
```

```text
Ada 70
```

The first section defines a type named Student. Each Student value has its own name and score fields, using the familiar String and int64 types.

In main, the declaration creates a Student value. Each dot selects a field for assignment or reading. The type name describes the kind of value; student names this particular value.

## 6.2 Add an operation

Add this method after the fields:

```chali
void addBonus(int64 bonus)
{
    self.score += bonus;
}
```

A method is a function declared in the type. bonus is an explicit parameter; self is the receiving object. `student.addBonus(5);` updates that student's score, not every Student value. The method returns void because this version performs a modification without returning a result.

## 6.3 Collect initialization

Repeated field assignments can be collected in a named construction entry:

<!-- verify-book -->
```chali
struct Student
{
    String name;
    int64 score;

    static Student create(String name, int64 score)
    {
        self.name = name;
        self.score = score;
        return self;
    }
}

void main()
{
    Student student = Student::create("Ada", 70);
    println("{} {}", student.name, student.score);
}
```

```text
Ada 70
```

The header marks a static entry returning the owning Student type. Its first two statements initialize the result's fields, and `return self;` finishes that initialization.

main calls `Student::create` through the type, without an existing student. In contrast, an instance call such as student.addBonus has a receiver. self has special meaning in this construction body; arbitrary static helpers cannot use it as a global object.

## 6.4 Design one step at a time

Choose the fields, then the operations that need them. Chapter 7 examines copying this model; Chapter 18 varies field types through generics; Chapter 22 introduces method derivation through member macros. The first model does not need all three at once.

### Combine instance fields

Store width and height on the same object and use them to compute area.

<!-- verify-book -->
```chali
struct Size {
    int64 width;
    int64 height;
}
void main() {
    Size box;
    box.width = 3;
    box.height = 4;
    println("{}", box.width * box.height);
}
```

```text
12
```
