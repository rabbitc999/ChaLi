# 全部库函数符号

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](symbols.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

[返回库目录](README.md)

| 限定名 | 签名 |
| --- | --- |
| [std::algorithms::allValue](std-algorithms.md#api-18) | `bool allValue<T>(List<T> items,bool fn(T) predicate)` |
| [std::algorithms::anyValue](std-algorithms.md#api-19) | `bool anyValue<T>(List<T> items,bool fn(T) predicate)` |
| [std::algorithms::binarySearchReference](std-algorithms.md#api-7) | `int64 binarySearchReference<T>(T const& data, int64 count, T value)` |
| [std::algorithms::binarySearchValue](std-algorithms.md#api-14) | `int64 binarySearchValue<T>(List<T> items,T value)` |
| [std::algorithms::choiceValue](std-algorithms.md#api-0) | `T choiceValue<T>(std::core::List<T> items, uint64 seed)` |
| [std::algorithms::clampValue](std-algorithms.md#api-17) | `T clampValue<T>(T value,T low,T high)` |
| [std::algorithms::countReference](std-algorithms.md#api-10) | `int64 countReference<T>(T const& data, int64 count, T value)` |
| [std::algorithms::countValue](std-algorithms.md#api-26) | `int64 countValue<T>(List<T> items,T value)` |
| [std::algorithms::countValue](std-algorithms.md#api-27) | `int64 countValue<T>(List<T> items,bool fn(T) predicate)` |
| [std::algorithms::filterValue](std-algorithms.md#api-22) | `List<T> filterValue<T>(List<T> items,bool fn(T) predicate)` |
| [std::algorithms::findReference](std-algorithms.md#api-6) | `int64 findReference<T>(T const& data, int64 count, T value)` |
| [std::algorithms::findValue](std-algorithms.md#api-13) | `int64 findValue<T>(List<T> items,T value)` |
| [std::algorithms::foldValue](std-algorithms.md#api-20) | `T foldValue<T>(List<T> items,T initial,T fn(T,T) folder)` |
| [std::algorithms::mapValue](std-algorithms.md#api-21) | `List<R> mapValue<T>(List<T> items,R fn(T) mapper)` |
| [std::algorithms::maxReference](std-algorithms.md#api-9) | `Option<T mut&> maxReference<T>(T mut& data, int64 count)` |
| [std::algorithms::maxValue](std-algorithms.md#api-16) | `T maxValue<T>(T first,T rest...)` |
| [std::algorithms::minReference](std-algorithms.md#api-8) | `Option<T mut&> minReference<T>(T mut& data, int64 count)` |
| [std::algorithms::minValue](std-algorithms.md#api-15) | `T minValue<T>(T first,T rest...)` |
| [std::algorithms::reduceValue](std-algorithms.md#api-23) | `T reduceValue<T>(List<T> items,T fn(T,T) folder)` |
| [std::algorithms::reduceValue](std-algorithms.md#api-24) | `T reduceValue<T>(List<T> items,T initial,T fn(T,T) folder)` |
| [std::algorithms::reverseReference](std-algorithms.md#api-5) | `void reverseReference<T>(T mut& data, int64 count)` |
| [std::algorithms::shuffleReference](std-algorithms.md#api-2) | `void shuffleReference<T>(T mut& data, int64 count, uint64 seed)` |
| [std::algorithms::shuffleValue](std-algorithms.md#api-1) | `std::core::List<T> shuffleValue<T>(std::core::List<T> items, uint64 seed)` |
| [std::algorithms::sortReference](std-algorithms.md#api-4) | `void sortReference<T>(T mut& data, int64 count)` |
| [std::algorithms::sortValue](std-algorithms.md#api-12) | `List<T> sortValue<T>(List<T> items)` |
| [std::algorithms::sumReference](std-algorithms.md#api-11) | `T sumReference<T>(T mut& data, int64 count)` |
| [std::algorithms::sumValue](std-algorithms.md#api-25) | `T sumValue<T>(List<T> items)` |
| [std::algorithms::zipValue](std-algorithms.md#api-3) | `std::core::List<(T, U)> zipValue<T, U>(std::core::List<T> left, std::core::List<U> right)` |
| [std::array::DynamicArray::at](std-array.md#api-10) | `T at(int64 position)` |
| [std::array::DynamicArray::capacity](std-array.md#api-3) | `int64 capacity()` |
| [std::array::DynamicArray::clear](std-array.md#api-9) | `void clear()` |
| [std::array::DynamicArray::copy](std-array.md#api-5) | `DynamicArray<T> copy()` |
| [std::array::DynamicArray::create](std-array.md#api-0) | `static DynamicArray<T> create()` |
| [std::array::DynamicArray::index](std-array.md#api-11) | `T index(int64 position)` |
| [std::array::DynamicArray::isEmpty](std-array.md#api-4) | `bool isEmpty()` |
| [std::array::DynamicArray::iterator](std-array.md#api-14) | `Option<T> iterator(int64 position)` |
| [std::array::DynamicArray::len](std-array.md#api-2) | `int64 len()` |
| [std::array::DynamicArray::pop](std-array.md#api-13) | `Option<T> pop()` |
| [std::array::DynamicArray::push](std-array.md#api-7) | `void push(T value)` |
| [std::array::DynamicArray::reserve](std-array.md#api-6) | `void reserve(int64 minimumCapacity)` |
| [std::array::DynamicArray::resize](std-array.md#api-8) | `void resize(int64 length)` |
| [std::array::DynamicArray::set](std-array.md#api-12) | `void set(int64 position, T value)` |
| [std::array::DynamicArray::toArray](std-array.md#api-15) | `T[] toArray()` |
| [std::array::DynamicArray::withCapacity](std-array.md#api-1) | `static DynamicArray<T> withCapacity(int64 capacity)` |
| [std::audio::close](std-audio.md#api-6) | `void close()` |
| [std::audio::isPlaying](std-audio.md#api-4) | `bool isPlaying(int64 id)` |
| [std::audio::load](std-audio.md#api-1) | `int64 load(String path)` |
| [std::audio::open](std-audio.md#api-0) | `bool open()` |
| [std::audio::play](std-audio.md#api-2) | `bool play(int64 id)` |
| [std::audio::stop](std-audio.md#api-3) | `bool stop(int64 id)` |
| [std::audio::unload](std-audio.md#api-5) | `bool unload(int64 id)` |
| [std::big_number::BigDecimal::abs](std-big_number.md#api-20) | `BigDecimal abs()` |
| [std::big_number::BigDecimal::ceil](std-big_number.md#api-23) | `BigInteger ceil()` |
| [std::big_number::BigDecimal::copy](std-big_number.md#api-17) | `BigDecimal copy()` |
| [std::big_number::BigDecimal::floor](std-big_number.md#api-22) | `BigInteger floor()` |
| [std::big_number::BigDecimal::isInteger](std-big_number.md#api-21) | `bool isInteger()` |
| [std::big_number::BigDecimal::pow](std-big_number.md#api-19) | `BigDecimal pow(int64 exponent)` |
| [std::big_number::BigDecimal::root](std-big_number.md#api-26) | `BigDecimal root(int64 k, int64 digits)` |
| [std::big_number::BigDecimal::setPrecision](std-big_number.md#api-18) | `int64 setPrecision(int64 digits)` |
| [std::big_number::BigDecimal::sqrt](std-big_number.md#api-25) | `BigDecimal sqrt(int64 digits)` |
| [std::big_number::BigDecimal::str](std-big_number.md#api-16) | `String str()` |
| [std::big_number::BigDecimal::trunc](std-big_number.md#api-24) | `BigInteger trunc()` |
| [std::big_number::BigInteger::bitAnd](std-big_number.md#api-11) | `BigInteger bitAnd(BigInteger other)` |
| [std::big_number::BigInteger::bitLength](std-big_number.md#api-10) | `int64 bitLength()` |
| [std::big_number::BigInteger::bitOr](std-big_number.md#api-12) | `BigInteger bitOr(BigInteger other)` |
| [std::big_number::BigInteger::bitXor](std-big_number.md#api-13) | `BigInteger bitXor(BigInteger other)` |
| [std::big_number::BigInteger::copy](std-big_number.md#api-1) | `BigInteger copy()` |
| [std::big_number::BigInteger::gcd](std-big_number.md#api-4) | `BigInteger gcd(BigInteger other)` |
| [std::big_number::BigInteger::iroot](std-big_number.md#api-15) | `BigInteger iroot(int64 k)` |
| [std::big_number::BigInteger::isProbablePrime](std-big_number.md#api-8) | `bool isProbablePrime(int64 rounds)` |
| [std::big_number::BigInteger::lcm](std-big_number.md#api-5) | `BigInteger lcm(BigInteger other)` |
| [std::big_number::BigInteger::mod](std-big_number.md#api-6) | `BigInteger mod(BigInteger modulus)` |
| [std::big_number::BigInteger::modInverse](std-big_number.md#api-7) | `BigInteger modInverse(BigInteger modulus)` |
| [std::big_number::BigInteger::nextProbablePrime](std-big_number.md#api-9) | `BigInteger nextProbablePrime()` |
| [std::big_number::BigInteger::pow](std-big_number.md#api-2) | `BigInteger pow(int64 exponent)` |
| [std::big_number::BigInteger::powmod](std-big_number.md#api-3) | `BigInteger powmod(BigInteger exponent, BigInteger modulus)` |
| [std::big_number::BigInteger::str](std-big_number.md#api-0) | `String str()` |
| [std::big_number::BigInteger::strRadix](std-big_number.md#api-14) | `String strRadix(int64 base)` |
| [std::binary::BinaryReader::drop](std-binary.md#api-0) | `void drop()` |
| [std::binary::BinaryReader::fromArray](std-binary.md#api-3) | `static BinaryReader fromArray(uint8 bytes[])` |
| [std::binary::BinaryReader::fromArray](std-binary.md#api-2) | `static BinaryReader fromArray(uint8 const& bytes, int64 count)` |
| [std::binary::BinaryReader::fromBytes](std-binary.md#api-1) | `static BinaryReader fromBytes(List<uint8> bytes)` |
| [std::binary::BinaryReader::position](std-binary.md#api-4) | `int64 position()` |
| [std::binary::BinaryReader::readBool](std-binary.md#api-30) | `bool readBool()` |
| [std::binary::BinaryReader::readBoolArray](std-binary.md#api-31) | `List<bool> readBoolArray(int64 count)` |
| [std::binary::BinaryReader::readBytes](std-binary.md#api-32) | `List<uint8> readBytes(int64 count)` |
| [std::binary::BinaryReader::readFloat32](std-binary.md#api-26) | `float32 readFloat32()` |
| [std::binary::BinaryReader::readFloat32Array](std-binary.md#api-27) | `List<float32> readFloat32Array(int64 count)` |
| [std::binary::BinaryReader::readFloat64](std-binary.md#api-28) | `float64 readFloat64()` |
| [std::binary::BinaryReader::readFloat64Array](std-binary.md#api-29) | `List<float64> readFloat64Array(int64 count)` |
| [std::binary::BinaryReader::readInt16](std-binary.md#api-14) | `int16 readInt16()` |
| [std::binary::BinaryReader::readInt16Array](std-binary.md#api-15) | `List<int16> readInt16Array(int64 count)` |
| [std::binary::BinaryReader::readInt32](std-binary.md#api-18) | `int32 readInt32()` |
| [std::binary::BinaryReader::readInt32Array](std-binary.md#api-19) | `List<int32> readInt32Array(int64 count)` |
| [std::binary::BinaryReader::readInt64](std-binary.md#api-22) | `int64 readInt64()` |
| [std::binary::BinaryReader::readInt64Array](std-binary.md#api-23) | `List<int64> readInt64Array(int64 count)` |
| [std::binary::BinaryReader::readInt8](std-binary.md#api-10) | `int8 readInt8()` |
| [std::binary::BinaryReader::readInt8Array](std-binary.md#api-11) | `List<int8> readInt8Array(int64 count)` |
| [std::binary::BinaryReader::readString](std-binary.md#api-33) | `String readString(int64 length)` |
| [std::binary::BinaryReader::readStringArray](std-binary.md#api-34) | `List<String> readStringArray(int64 count, int64 width)` |
| [std::binary::BinaryReader::readUint16](std-binary.md#api-16) | `uint16 readUint16()` |
| [std::binary::BinaryReader::readUint16Array](std-binary.md#api-17) | `List<uint16> readUint16Array(int64 count)` |
| [std::binary::BinaryReader::readUint32](std-binary.md#api-20) | `uint32 readUint32()` |
| [std::binary::BinaryReader::readUint32Array](std-binary.md#api-21) | `List<uint32> readUint32Array(int64 count)` |
| [std::binary::BinaryReader::readUint64](std-binary.md#api-24) | `uint64 readUint64()` |
| [std::binary::BinaryReader::readUint64Array](std-binary.md#api-25) | `List<uint64> readUint64Array(int64 count)` |
| [std::binary::BinaryReader::readUint8](std-binary.md#api-12) | `uint8 readUint8()` |
| [std::binary::BinaryReader::readUint8Array](std-binary.md#api-13) | `List<uint8> readUint8Array(int64 count)` |
| [std::binary::BinaryReader::remaining](std-binary.md#api-5) | `int64 remaining()` |
| [std::binary::BinaryReader::seek](std-binary.md#api-8) | `bool seek(int64 position)` |
| [std::binary::BinaryReader::setBigEndian](std-binary.md#api-7) | `void setBigEndian(bool enabled)` |
| [std::binary::BinaryReader::size](std-binary.md#api-6) | `int64 size()` |
| [std::binary::BinaryReader::skip](std-binary.md#api-9) | `bool skip(int64 count)` |
| [std::cas::Expr::acos](std-cas.md#api-38) | `Expr acos()` |
| [std::cas::Expr::asin](std-cas.md#api-37) | `Expr asin()` |
| [std::cas::Expr::ast](std-cas.md#api-4) | `Expr ast()` |
| [std::cas::Expr::atan](std-cas.md#api-39) | `Expr atan()` |
| [std::cas::Expr::ceil](std-cas.md#api-45) | `Expr ceil()` |
| [std::cas::Expr::child](std-cas.md#api-11) | `Expr child(int64 index)` |
| [std::cas::Expr::childCount](std-cas.md#api-10) | `int64 childCount()` |
| [std::cas::Expr::children](std-cas.md#api-12) | `List<Expr> children()` |
| [std::cas::Expr::cos](std-cas.md#api-35) | `Expr cos()` |
| [std::cas::Expr::decimal](std-cas.md#api-15) | `BigDecimal decimal(int64 digits = 50)` |
| [std::cas::Expr::denominator](std-cas.md#api-14) | `BigInteger denominator()` |
| [std::cas::Expr::differentiate](std-cas.md#api-28) | `Expr differentiate(String variable = "x", int64 order = 1)` |
| [std::cas::Expr::exp](std-cas.md#api-40) | `Expr exp()` |
| [std::cas::Expr::expand](std-cas.md#api-25) | `Expr expand()` |
| [std::cas::Expr::factor](std-cas.md#api-26) | `Expr factor()` |
| [std::cas::Expr::floor](std-cas.md#api-44) | `Expr floor()` |
| [std::cas::Expr::fmod](std-cas.md#api-48) | `Expr fmod(Expr other)` |
| [std::cas::Expr::fromDecimal](std-cas.md#api-2) | `static Expr fromDecimal(BigDecimal number)` |
| [std::cas::Expr::fromInteger](std-cas.md#api-1) | `static Expr fromInteger(BigInteger number)` |
| [std::cas::Expr::integrate](std-cas.md#api-29) | `Expr integrate(String variable = "x")` |
| [std::cas::Expr::integrateDefinite](std-cas.md#api-33) | `Expr integrateDefinite(String variable, Expr lower, Expr upper)` |
| [std::cas::Expr::kind](std-cas.md#api-8) | `NodeKind kind()` |
| [std::cas::Expr::latex](std-cas.md#api-7) | `String latex()` |
| [std::cas::Expr::leftAdd](std-cas.md#api-16) | `Expr leftAdd(Expr other)` |
| [std::cas::Expr::leftDiv](std-cas.md#api-19) | `Expr leftDiv(Expr other)` |
| [std::cas::Expr::leftEqual](std-cas.md#api-20) | `bool leftEqual(Expr other)` |
| [std::cas::Expr::leftMul](std-cas.md#api-18) | `Expr leftMul(Expr other)` |
| [std::cas::Expr::leftSub](std-cas.md#api-17) | `Expr leftSub(Expr other)` |
| [std::cas::Expr::limit](std-cas.md#api-31) | `Expr limit(String variable, Expr point)` |
| [std::cas::Expr::ln](std-cas.md#api-41) | `Expr ln()` |
| [std::cas::Expr::log10](std-cas.md#api-42) | `Expr log10()` |
| [std::cas::Expr::name](std-cas.md#api-9) | `String name()` |
| [std::cas::Expr::negate](std-cas.md#api-21) | `Expr negate(Expr operand)` |
| [std::cas::Expr::numerator](std-cas.md#api-13) | `BigInteger numerator()` |
| [std::cas::Expr::parse](std-cas.md#api-0) | `static Expr parse(String text)` |
| [std::cas::Expr::pow](std-cas.md#api-22) | `Expr pow(Expr exponent)` |
| [std::cas::Expr::pow](std-cas.md#api-23) | `Expr pow(int64 exponent)` |
| [std::cas::Expr::round](std-cas.md#api-46) | `Expr round()` |
| [std::cas::Expr::simplify](std-cas.md#api-24) | `Expr simplify()` |
| [std::cas::Expr::sin](std-cas.md#api-34) | `Expr sin()` |
| [std::cas::Expr::sqrt](std-cas.md#api-43) | `Expr sqrt()` |
| [std::cas::Expr::str](std-cas.md#api-5) | `String str()` |
| [std::cas::Expr::substitute](std-cas.md#api-30) | `Expr substitute(String variable, Expr replacement)` |
| [std::cas::Expr::symbol](std-cas.md#api-3) | `static Expr symbol(String name)` |
| [std::cas::Expr::tan](std-cas.md#api-36) | `Expr tan()` |
| [std::cas::Expr::taylor](std-cas.md#api-32) | `Expr taylor(String variable, Expr point, int64 order)` |
| [std::cas::Expr::toString](std-cas.md#api-6) | `String toString()` |
| [std::cas::Expr::together](std-cas.md#api-27) | `Expr together()` |
| [std::cas::Expr::trunc](std-cas.md#api-47) | `Expr trunc()` |
| [std::cas::parse](std-cas.md#api-49) | `Expr parse(String text)` |
| [std::cli::CliArguments::get](std-cli.md#api-1) | `Option<String> get(String name)` |
| [std::cli::CliArguments::getInt](std-cli.md#api-2) | `Result<int64,Error> getInt(String name)` |
| [std::cli::CliError::toString](std-cli.md#api-0) | `String toString()` |
| [std::cli::CliParser::add](std-cli.md#api-5) | `bool add(CliOption option)` |
| [std::cli::CliParser::help](std-cli.md#api-6) | `String help()` |
| [std::cli::CliParser::parse](std-cli.md#api-7) | `Result<CliArguments,Error> parse(List<String> arguments)` |
| [std::cli::cliError](std-cli.md#api-3) | `CliError cliError(String argument, String message)` |
| [std::cli::cliTail](std-cli.md#api-4) | `String cliTail(String text,int64 start)` |
| [std::clock::Duration::fromMillis](std-clock.md#api-3) | `static Duration fromMillis(int64 millis)` |
| [std::clock::Duration::fromNanos](std-clock.md#api-2) | `static Duration fromNanos(int64 nanos)` |
| [std::clock::Duration::toMillisCeil](std-clock.md#api-5) | `int64 toMillisCeil()` |
| [std::clock::Duration::toNanos](std-clock.md#api-4) | `int64 toNanos()` |
| [std::clock::Instant::durationSince](std-clock.md#api-8) | `Duration durationSince(Instant earlier)` |
| [std::clock::Instant::elapsed](std-clock.md#api-9) | `Duration elapsed()` |
| [std::clock::Instant::now](std-clock.md#api-6) | `static Instant now()` |
| [std::clock::Instant::plus](std-clock.md#api-7) | `Instant plus(Duration duration)` |
| [std::clock::Instant::remaining](std-clock.md#api-10) | `Duration remaining()` |
| [std::clock::nowMs](std-clock.md#api-0) | `int64 nowMs()` |
| [std::clock::nowNanos](std-clock.md#api-1) | `int64 nowNanos()` |
| [std::collections::Channel::create](std-collections.md#api-53) | `static Channel<T> create()` |
| [std::collections::Channel::drop](std-collections.md#api-59) | `void drop()` |
| [std::collections::Channel::isEmpty](std-collections.md#api-58) | `bool isEmpty()` |
| [std::collections::Channel::iterator](std-collections.md#api-56) | `Option<T> iterator(int64 index)` |
| [std::collections::Channel::len](std-collections.md#api-57) | `int64 len()` |
| [std::collections::Channel::receive](std-collections.md#api-55) | `Option<T> receive()` |
| [std::collections::Channel::send](std-collections.md#api-54) | `void send(T value)` |
| [std::collections::Deque::clear](std-collections.md#api-12) | `void clear()` |
| [std::collections::Deque::create](std-collections.md#api-0) | `static Deque<T> create()` |
| [std::collections::Deque::drop](std-collections.md#api-13) | `void drop()` |
| [std::collections::Deque::index](std-collections.md#api-5) | `T index(int64 key)` |
| [std::collections::Deque::isEmpty](std-collections.md#api-11) | `bool isEmpty()` |
| [std::collections::Deque::iterator](std-collections.md#api-6) | `Option<T> iterator(int64 index)` |
| [std::collections::Deque::len](std-collections.md#api-10) | `int64 len()` |
| [std::collections::Deque::peekBack](std-collections.md#api-8) | `Option<T> peekBack()` |
| [std::collections::Deque::peekFront](std-collections.md#api-7) | `Option<T> peekFront()` |
| [std::collections::Deque::popBack](std-collections.md#api-3) | `Option<T> popBack()` |
| [std::collections::Deque::popFront](std-collections.md#api-4) | `Option<T> popFront()` |
| [std::collections::Deque::pushBack](std-collections.md#api-1) | `void pushBack(T value)` |
| [std::collections::Deque::pushFront](std-collections.md#api-2) | `void pushFront(T value)` |
| [std::collections::Deque::toList](std-collections.md#api-9) | `List<T> toList()` |
| [std::collections::FixedVector::capacityValue](std-collections.md#api-51) | `int64 capacityValue()` |
| [std::collections::FixedVector::drop](std-collections.md#api-52) | `void drop()` |
| [std::collections::FixedVector::index](std-collections.md#api-48) | `T index(int64 key)` |
| [std::collections::FixedVector::iterator](std-collections.md#api-49) | `Option<T> iterator(int64 index)` |
| [std::collections::FixedVector::len](std-collections.md#api-50) | `int64 len()` |
| [std::collections::FixedVector::pop](std-collections.md#api-47) | `Option<T> pop()` |
| [std::collections::FixedVector::push](std-collections.md#api-46) | `bool push(T value)` |
| [std::collections::FixedVector::withCapacity](std-collections.md#api-45) | `static FixedVector<T> withCapacity(int64 capacity)` |
| [std::collections::Queue::clear](std-collections.md#api-22) | `void clear()` |
| [std::collections::Queue::create](std-collections.md#api-14) | `static Queue<T> create()` |
| [std::collections::Queue::drop](std-collections.md#api-23) | `void drop()` |
| [std::collections::Queue::isEmpty](std-collections.md#api-21) | `bool isEmpty()` |
| [std::collections::Queue::iterator](std-collections.md#api-19) | `Option<T> iterator(int64 index)` |
| [std::collections::Queue::len](std-collections.md#api-20) | `int64 len()` |
| [std::collections::Queue::peek](std-collections.md#api-17) | `Option<T> peek()` |
| [std::collections::Queue::peekBack](std-collections.md#api-18) | `Option<T> peekBack()` |
| [std::collections::Queue::pop](std-collections.md#api-16) | `Option<T> pop()` |
| [std::collections::Queue::push](std-collections.md#api-15) | `void push(T value)` |
| [std::collections::RingBuffer::available](std-collections.md#api-39) | `int64 available()` |
| [std::collections::RingBuffer::clear](std-collections.md#api-43) | `void clear()` |
| [std::collections::RingBuffer::drop](std-collections.md#api-44) | `void drop()` |
| [std::collections::RingBuffer::isEmpty](std-collections.md#api-40) | `bool isEmpty()` |
| [std::collections::RingBuffer::isFull](std-collections.md#api-41) | `bool isFull()` |
| [std::collections::RingBuffer::iterator](std-collections.md#api-37) | `Option<T> iterator(int64 index)` |
| [std::collections::RingBuffer::len](std-collections.md#api-38) | `int64 len()` |
| [std::collections::RingBuffer::peek](std-collections.md#api-42) | `Option<T> peek()` |
| [std::collections::RingBuffer::pop](std-collections.md#api-36) | `Option<T> pop()` |
| [std::collections::RingBuffer::push](std-collections.md#api-35) | `bool push(T value)` |
| [std::collections::RingBuffer::withCapacity](std-collections.md#api-34) | `static RingBuffer<T> withCapacity(int64 capacity)` |
| [std::collections::Stack::clear](std-collections.md#api-32) | `void clear()` |
| [std::collections::Stack::create](std-collections.md#api-24) | `static Stack<T> create()` |
| [std::collections::Stack::drop](std-collections.md#api-33) | `void drop()` |
| [std::collections::Stack::isEmpty](std-collections.md#api-31) | `bool isEmpty()` |
| [std::collections::Stack::iterator](std-collections.md#api-29) | `Option<T> iterator(int64 index)` |
| [std::collections::Stack::len](std-collections.md#api-30) | `int64 len()` |
| [std::collections::Stack::peek](std-collections.md#api-27) | `Option<T> peek()` |
| [std::collections::Stack::peekBottom](std-collections.md#api-28) | `Option<T> peekBottom()` |
| [std::collections::Stack::pop](std-collections.md#api-26) | `Option<T> pop()` |
| [std::collections::Stack::push](std-collections.md#api-25) | `void push(T value)` |
| [std::complex::Complex::conjugate](std-complex.md#api-1) | `Complex<T> conjugate()` |
| [std::complex::Complex::leftAdd](std-complex.md#api-4) | `Complex<T> leftAdd(Complex<T> right)` |
| [std::complex::Complex::leftDiv](std-complex.md#api-7) | `Complex<T> leftDiv(Complex<T> right)` |
| [std::complex::Complex::leftMul](std-complex.md#api-6) | `Complex<T> leftMul(Complex<T> right)` |
| [std::complex::Complex::leftSub](std-complex.md#api-5) | `Complex<T> leftSub(Complex<T> right)` |
| [std::complex::Complex::normSquared](std-complex.md#api-2) | `T normSquared()` |
| [std::complex::Complex::of](std-complex.md#api-0) | `static Complex<T> of(T real, T imaginary)` |
| [std::complex::Complex::scale](std-complex.md#api-3) | `Complex<T> scale(T factor)` |
| [std::compress::compress](std-compress.md#api-0) | `List<uint8> compress(String text)` |
| [std::compress::decompress](std-compress.md#api-1) | `String decompress(List<uint8> bytes)` |
| [std::concurrency::Arc::clone](std-concurrency.md#api-2) | `Arc<T> clone()` |
| [std::concurrency::Arc::close](std-concurrency.md#api-5) | `void close()` |
| [std::concurrency::Arc::downgrade](std-concurrency.md#api-4) | `ArcWeak<T> downgrade()` |
| [std::concurrency::Arc::get](std-concurrency.md#api-3) | `T get()` |
| [std::concurrency::Arc::isEmpty](std-concurrency.md#api-1) | `bool isEmpty()` |
| [std::concurrency::ArcWeak::close](std-concurrency.md#api-8) | `void close()` |
| [std::concurrency::ArcWeak::isExpired](std-concurrency.md#api-6) | `bool isExpired()` |
| [std::concurrency::ArcWeak::upgrade](std-concurrency.md#api-7) | `Arc<T> upgrade()` |
| [std::concurrency::AtomicInt64::compareExchange](std-concurrency.md#api-19) | `bool compareExchange(int64 expected, int64 desired)` |
| [std::concurrency::AtomicInt64::create](std-concurrency.md#api-15) | `static AtomicInt64 create(int64 value)` |
| [std::concurrency::AtomicInt64::fetchAdd](std-concurrency.md#api-18) | `int64 fetchAdd(int64 delta)` |
| [std::concurrency::AtomicInt64::get](std-concurrency.md#api-16) | `int64 get()` |
| [std::concurrency::AtomicInt64::set](std-concurrency.md#api-17) | `void set(int64 value)` |
| [std::concurrency::Channel::close](std-concurrency.md#api-27) | `void close()` |
| [std::concurrency::Channel::create](std-concurrency.md#api-20) | `static Channel<T> create(int64 capacity)` |
| [std::concurrency::Channel::receive](std-concurrency.md#api-25) | `Receive<T> receive()` |
| [std::concurrency::Channel::receiveFor](std-concurrency.md#api-24) | `Receive<T> receiveFor(int64 millis)` |
| [std::concurrency::Channel::send](std-concurrency.md#api-22) | `int64 send(T value)` |
| [std::concurrency::Channel::sendFor](std-concurrency.md#api-21) | `int64 sendFor(T value, int64 millis)` |
| [std::concurrency::Channel::tryReceive](std-concurrency.md#api-26) | `Receive<T> tryReceive()` |
| [std::concurrency::Channel::trySend](std-concurrency.md#api-23) | `int64 trySend(T value)` |
| [std::concurrency::Receive::isReady](std-concurrency.md#api-0) | `bool isReady()` |
| [std::concurrency::Shared::create](std-concurrency.md#api-10) | `static Shared<T> create(T value)` |
| [std::concurrency::Shared::get](std-concurrency.md#api-11) | `T get()` |
| [std::concurrency::Shared::id](std-concurrency.md#api-14) | `int64 id()` |
| [std::concurrency::Shared::set](std-concurrency.md#api-12) | `void set(T value)` |
| [std::concurrency::Shared::swap](std-concurrency.md#api-13) | `T swap(T value)` |
| [std::concurrency::arc](std-concurrency.md#api-9) | `Arc<T> arc<T>(T value)` |
| [std::core::Add::leftAdd](std-core.md#api-231) | `R leftAdd(T value)` |
| [std::core::And::leftBitAnd](std-core.md#api-253) | `R leftBitAnd(T value)` |
| [std::core::Any::clear](std-core.md#api-214) | `void clear()` |
| [std::core::Any::copy](std-core.md#api-212) | `Any copy()` |
| [std::core::Any::getOptional](std-core.md#api-218) | `Option<T mut&> getOptional()` |
| [std::core::Any::getReference](std-core.md#api-217) | `T mut& getReference()` |
| [std::core::Any::getValue](std-core.md#api-216) | `T getValue()` |
| [std::core::Any::isEmpty](std-core.md#api-213) | `bool isEmpty()` |
| [std::core::Any::set](std-core.md#api-215) | `int64 set(T value)` |
| [std::core::BarePtr::castTo](std-core.md#api-275) | `unsafe BarePtr<U> castTo<U>()` |
| [std::core::BarePtr::leftAdd](std-core.md#api-133) | `BarePtr<T> leftAdd(int64 elements)` |
| [std::core::BarePtr::leftSub](std-core.md#api-135) | `BarePtr<T> leftSub(int64 elements)` |
| [std::core::BarePtr::leftSub](std-core.md#api-136) | `int64 leftSub(BarePtr<T> other)` |
| [std::core::BarePtr::rightAdd](std-core.md#api-134) | `BarePtr<T> rightAdd(int64 elements)` |
| [std::core::BitNot::bitNot](std-core.md#api-264) | `R bitNot(T value)` |
| [std::core::Box::close](std-core.md#api-157) | `void close()` |
| [std::core::Box::get](std-core.md#api-155) | `T get()` |
| [std::core::Box::isEmpty](std-core.md#api-154) | `bool isEmpty()` |
| [std::core::Box::newBox](std-core.md#api-153) | `static Box<T> newBox(T value)` |
| [std::core::Box::take](std-core.md#api-156) | `T take()` |
| [std::core::Closeable::close](std-core.md#api-162) | `void close()` |
| [std::core::Copy::copy](std-core.md#api-267) | `T copy()` |
| [std::core::Debug::debug](std-core.md#api-55) | `String debug()` |
| [std::core::Divide::leftDiv](std-core.md#api-237) | `R leftDiv(T value)` |
| [std::core::Drop::drop](std-core.md#api-161) | `void drop()` |
| [std::core::Equal::leftEqual](std-core.md#api-241) | `R leftEqual(T value)` |
| [std::core::Error::toString](std-core.md#api-56) | `String toString()` |
| [std::core::ExplicitConvert::explicitConvert](std-core.md#api-268) | `T explicitConvert()` |
| [std::core::File::print](std-core.md#api-59) | `int64 print(String text)` |
| [std::core::File::println](std-core.md#api-60) | `int64 println(String text = "")` |
| [std::core::FromResidual::fromResidual](std-core.md#api-98) | `static F fromResidual(R residual)` |
| [std::core::Greater::leftGreater](std-core.md#api-249) | `R leftGreater(T value)` |
| [std::core::GreaterOrEqual::leftGreaterEqual](std-core.md#api-251) | `R leftGreaterEqual(T value)` |
| [std::core::Hash::hash](std-core.md#api-266) | `uint64 hash(T value)` |
| [std::core::ImplicitConvert::implicitConvert](std-core.md#api-269) | `T implicitConvert()` |
| [std::core::Index::index](std-core.md#api-270) | `R index(T key)` |
| [std::core::Iterator::iterator](std-core.md#api-271) | `Option<R> iterator(int64 index)` |
| [std::core::Less::leftLess](std-core.md#api-245) | `R leftLess(T value)` |
| [std::core::LessOrEqual::leftLessEqual](std-core.md#api-247) | `R leftLessEqual(T value)` |
| [std::core::List::at](std-core.md#api-187) | `T at(int64 index)` |
| [std::core::List::clear](std-core.md#api-193) | `int64 clear()` |
| [std::core::List::contains](std-core.md#api-191) | `bool contains(T value)` |
| [std::core::List::copy](std-core.md#api-184) | `List<T> copy()` |
| [std::core::List::has](std-core.md#api-192) | `bool has(T value)` |
| [std::core::List::indexOf](std-core.md#api-190) | `int64 indexOf(T value)` |
| [std::core::List::insert](std-core.md#api-186) | `int64 insert(int64 index, T value)` |
| [std::core::List::len](std-core.md#api-194) | `int64 len()` |
| [std::core::List::pop](std-core.md#api-188) | `T pop()` |
| [std::core::List::push](std-core.md#api-185) | `int64 push(T value)` |
| [std::core::List::remove](std-core.md#api-189) | `int64 remove(int64 index)` |
| [std::core::Map::clear](std-core.md#api-210) | `int64 clear()` |
| [std::core::Map::contains](std-core.md#api-206) | `int64 contains(K key)` |
| [std::core::Map::copy](std-core.md#api-203) | `Map<K,V> copy()` |
| [std::core::Map::has](std-core.md#api-205) | `int64 has(K key)` |
| [std::core::Map::keyAt](std-core.md#api-208) | `K keyAt(int64 index)` |
| [std::core::Map::len](std-core.md#api-211) | `int64 len()` |
| [std::core::Map::put](std-core.md#api-204) | `int64 put(K key,V value)` |
| [std::core::Map::remove](std-core.md#api-207) | `int64 remove(K key)` |
| [std::core::Map::valAt](std-core.md#api-209) | `V valAt(int64 index)` |
| [std::core::MessageError::from](std-core.md#api-58) | `static MessageError from(String text)` |
| [std::core::MessageError::toString](std-core.md#api-57) | `String toString()` |
| [std::core::Modulo::leftMod](std-core.md#api-239) | `R leftMod(T value)` |
| [std::core::Multiply::leftMul](std-core.md#api-235) | `R leftMul(T value)` |
| [std::core::Negate::negate](std-core.md#api-263) | `R negate(T value)` |
| [std::core::NextValue::nextValue](std-core.md#api-124) | `Option<T> nextValue(T end, bool inclusive)` |
| [std::core::NotEqual::leftNotEqual](std-core.md#api-243) | `R leftNotEqual(T value)` |
| [std::core::Option::andThen](std-core.md#api-109) | `Option<T> andThen(Option<T> fn(T) mapper)` |
| [std::core::Option::branch](std-core.md#api-99) | `TryBranch<T, NoneResidual> branch()` |
| [std::core::Option::expect](std-core.md#api-107) | `T expect(String message)` |
| [std::core::Option::fromResidual](std-core.md#api-101) | `static Option<T> fromResidual(NoneResidual residual)` |
| [std::core::Option::intoBranch](std-core.md#api-100) | `TryBranch<T, NoneResidual> intoBranch()` |
| [std::core::Option::isNone](std-core.md#api-103) | `bool isNone()` |
| [std::core::Option::isValue](std-core.md#api-102) | `bool isValue()` |
| [std::core::Option::okOr](std-core.md#api-273) | `Result<T,E> okOr<E>(E error)` |
| [std::core::Option::okOrElse](std-core.md#api-274) | `Result<T,E> okOrElse<E>(E fn() factory)` |
| [std::core::Option::take](std-core.md#api-104) | `T take()` |
| [std::core::Option::transform](std-core.md#api-108) | `Option<T> transform(T fn(T) mapper)` |
| [std::core::Option::unwrap](std-core.md#api-105) | `T unwrap()` |
| [std::core::Option::unwrapOr](std-core.md#api-106) | `T unwrapOr(T fallback)` |
| [std::core::Or::leftBitOr](std-core.md#api-255) | `R leftBitOr(T value)` |
| [std::core::Pair::create](std-core.md#api-126) | `static Pair<K, V> create((K, V) value)` |
| [std::core::Pair::create](std-core.md#api-125) | `static Pair<K, V> create(K first, V second)` |
| [std::core::Pair::toTuple](std-core.md#api-127) | `(K, V) toTuple()` |
| [std::core::Positive::positive](std-core.md#api-265) | `R positive(T value)` |
| [std::core::RawStorage::allocate](std-core.md#api-148) | `static RawStorage<T> allocate(int64 capacity)` |
| [std::core::RawStorage::capacity](std-core.md#api-149) | `int64 capacity()` |
| [std::core::RawStorage::close](std-core.md#api-152) | `void close()` |
| [std::core::RawStorage::isInitialized](std-core.md#api-151) | `bool isInitialized(int64 index)` |
| [std::core::RawStorage::pointer](std-core.md#api-150) | `RawPtr<T> pointer(int64 index)` |
| [std::core::Result::andThen](std-core.md#api-123) | `Result<T, E> andThen(Result<T, E> fn(T) mapper)` |
| [std::core::Result::branch](std-core.md#api-110) | `TryBranch<T, ErrorResidual<E>> branch()` |
| [std::core::Result::error](std-core.md#api-121) | `E error()` |
| [std::core::Result::expect](std-core.md#api-119) | `T expect(String message)` |
| [std::core::Result::fromResidual](std-core.md#api-112) | `static Result<T,E> fromResidual(ErrorResidual<E> residual)` |
| [std::core::Result::intoBranch](std-core.md#api-111) | `TryBranch<T, ErrorResidual<E>> intoBranch()` |
| [std::core::Result::isErr](std-core.md#api-115) | `bool isErr()` |
| [std::core::Result::isOk](std-core.md#api-114) | `bool isOk()` |
| [std::core::Result::mapErr](std-core.md#api-272) | `Result<T,F> mapErr<F>(F fn(E) mapper)` |
| [std::core::Result::ok](std-core.md#api-113) | `Option<T> ok()` |
| [std::core::Result::take](std-core.md#api-116) | `T take()` |
| [std::core::Result::takeError](std-core.md#api-120) | `E takeError()` |
| [std::core::Result::transform](std-core.md#api-122) | `Result<T, E> transform(T fn(T) mapper)` |
| [std::core::Result::unwrap](std-core.md#api-117) | `T unwrap()` |
| [std::core::Result::unwrapOr](std-core.md#api-118) | `T unwrapOr(T fallback)` |
| [std::core::RightAdd::rightAdd](std-core.md#api-232) | `R rightAdd(T value)` |
| [std::core::RightAnd::rightBitAnd](std-core.md#api-254) | `R rightBitAnd(T value)` |
| [std::core::RightDivide::rightDiv](std-core.md#api-238) | `R rightDiv(T value)` |
| [std::core::RightEqual::rightEqual](std-core.md#api-242) | `R rightEqual(T value)` |
| [std::core::RightGreater::rightGreater](std-core.md#api-250) | `R rightGreater(T value)` |
| [std::core::RightGreaterOrEqual::rightGreaterEqual](std-core.md#api-252) | `R rightGreaterEqual(T value)` |
| [std::core::RightLess::rightLess](std-core.md#api-246) | `R rightLess(T value)` |
| [std::core::RightLessOrEqual::rightLessEqual](std-core.md#api-248) | `R rightLessEqual(T value)` |
| [std::core::RightModulo::rightMod](std-core.md#api-240) | `R rightMod(T value)` |
| [std::core::RightMultiply::rightMul](std-core.md#api-236) | `R rightMul(T value)` |
| [std::core::RightNotEqual::rightNotEqual](std-core.md#api-244) | `R rightNotEqual(T value)` |
| [std::core::RightOr::rightBitOr](std-core.md#api-256) | `R rightBitOr(T value)` |
| [std::core::RightShiftLeft::rightShiftLeft](std-core.md#api-260) | `R rightShiftLeft(T value)` |
| [std::core::RightShiftRight::rightShiftRight](std-core.md#api-262) | `R rightShiftRight(T value)` |
| [std::core::RightSubtract::rightSub](std-core.md#api-234) | `R rightSub(T value)` |
| [std::core::RightXor::rightBitXor](std-core.md#api-258) | `R rightBitXor(T value)` |
| [std::core::Set::add](std-core.md#api-196) | `int64 add(T value)` |
| [std::core::Set::at](std-core.md#api-200) | `T at(int64 index)` |
| [std::core::Set::clear](std-core.md#api-201) | `int64 clear()` |
| [std::core::Set::contains](std-core.md#api-198) | `int64 contains(T value)` |
| [std::core::Set::copy](std-core.md#api-195) | `Set<T> copy()` |
| [std::core::Set::has](std-core.md#api-197) | `int64 has(T value)` |
| [std::core::Set::len](std-core.md#api-202) | `int64 len()` |
| [std::core::Set::remove](std-core.md#api-199) | `int64 remove(T value)` |
| [std::core::ShiftLeft::leftShiftLeft](std-core.md#api-259) | `R leftShiftLeft(T value)` |
| [std::core::ShiftRight::leftShiftRight](std-core.md#api-261) | `R leftShiftRight(T value)` |
| [std::core::String::append](std-core.md#api-7) | `void append(String text)` |
| [std::core::String::byteAt](std-core.md#api-24) | `Option<uint8> byteAt(int64 index)` |
| [std::core::String::byteLength](std-core.md#api-3) | `int64 byteLength()` |
| [std::core::String::bytes](std-core.md#api-51) | `List<uint8> bytes()` |
| [std::core::String::capacity](std-core.md#api-4) | `int64 capacity()` |
| [std::core::String::charAt](std-core.md#api-46) | `Option<String> charAt(int64 index)` |
| [std::core::String::charLength](std-core.md#api-16) | `int64 charLength()` |
| [std::core::String::clear](std-core.md#api-8) | `void clear()` |
| [std::core::String::compare](std-core.md#api-23) | `int64 compare(String text)` |
| [std::core::String::concat](std-core.md#api-53) | `String concat(String text)` |
| [std::core::String::contains](std-core.md#api-19) | `bool contains(String text)` |
| [std::core::String::count](std-core.md#api-22) | `int64 count(String text)` |
| [std::core::String::endsWith](std-core.md#api-21) | `bool endsWith(String text)` |
| [std::core::String::indexOf](std-core.md#api-17) | `int64 indexOf(String text, int64 start = 0)` |
| [std::core::String::insert](std-core.md#api-43) | `String insert(int64 index, String text)` |
| [std::core::String::isAlnum](std-core.md#api-14) | `bool isAlnum()` |
| [std::core::String::isAlpha](std-core.md#api-13) | `bool isAlpha()` |
| [std::core::String::isAscii](std-core.md#api-15) | `bool isAscii()` |
| [std::core::String::isBlank](std-core.md#api-11) | `bool isBlank()` |
| [std::core::String::isDigit](std-core.md#api-12) | `bool isDigit()` |
| [std::core::String::isEmpty](std-core.md#api-10) | `bool isEmpty()` |
| [std::core::String::join](std-core.md#api-52) | `String join(List<String> items)` |
| [std::core::String::lastIndexOf](std-core.md#api-18) | `int64 lastIndexOf(String text, int64 start = 0 - 1)` |
| [std::core::String::left](std-core.md#api-27) | `String left(int64 count)` |
| [std::core::String::length](std-core.md#api-1) | `int64 length()` |
| [std::core::String::lines](std-core.md#api-49) | `List<String> lines(bool keepEmpty = true)` |
| [std::core::String::maxCapacity](std-core.md#api-5) | `int64 maxCapacity()` |
| [std::core::String::padEnd](std-core.md#api-40) | `String padEnd(int64 length, String fill = " ")` |
| [std::core::String::padStart](std-core.md#api-39) | `String padStart(int64 length, String fill = " ")` |
| [std::core::String::prepend](std-core.md#api-54) | `String prepend(String text)` |
| [std::core::String::remove](std-core.md#api-44) | `String remove(int64 start, int64 count = 0 - 1)` |
| [std::core::String::removePrefix](std-core.md#api-41) | `String removePrefix(String prefix)` |
| [std::core::String::removeSuffix](std-core.md#api-42) | `String removeSuffix(String suffix)` |
| [std::core::String::repeat](std-core.md#api-37) | `String repeat(int64 count)` |
| [std::core::String::replace](std-core.md#api-36) | `String replace(String before, String after)` |
| [std::core::String::replaceAll](std-core.md#api-35) | `String replaceAll(String before, String after)` |
| [std::core::String::replaceFirst](std-core.md#api-34) | `String replaceFirst(String before, String after)` |
| [std::core::String::reserve](std-core.md#api-6) | `void reserve(int64 bytes)` |
| [std::core::String::reverse](std-core.md#api-38) | `String reverse()` |
| [std::core::String::right](std-core.md#api-28) | `String right(int64 count)` |
| [std::core::String::size](std-core.md#api-2) | `int64 size()` |
| [std::core::String::slice](std-core.md#api-26) | `String slice(int64 start, int64 end)` |
| [std::core::String::split](std-core.md#api-47) | `List<String> split(String separator, bool keepEmpty = true)` |
| [std::core::String::splitWhitespace](std-core.md#api-48) | `List<String> splitWhitespace()` |
| [std::core::String::startsWith](std-core.md#api-20) | `bool startsWith(String text)` |
| [std::core::String::substring](std-core.md#api-25) | `String substring(int64 start, int64 count = 0 - 1)` |
| [std::core::String::toLower](std-core.md#api-32) | `String toLower()` |
| [std::core::String::toUpper](std-core.md#api-33) | `String toUpper()` |
| [std::core::String::trim](std-core.md#api-29) | `String trim()` |
| [std::core::String::trimEnd](std-core.md#api-31) | `String trimEnd()` |
| [std::core::String::trimStart](std-core.md#api-30) | `String trimStart()` |
| [std::core::String::truncate](std-core.md#api-9) | `void truncate(int64 bytes)` |
| [std::core::String::unicodeChars](std-core.md#api-50) | `List<String> unicodeChars()` |
| [std::core::String::utf8Substring](std-core.md#api-45) | `String utf8Substring(int64 start, int64 count = 0 - 1)` |
| [std::core::Subtract::leftSub](std-core.md#api-233) | `R leftSub(T value)` |
| [std::core::Try::branch](std-core.md#api-96) | `TryBranch<T, R> branch()` |
| [std::core::TryMove::intoBranch](std-core.md#api-97) | `TryBranch<T, R> intoBranch()` |
| [std::core::Xor::leftBitXor](std-core.md#api-257) | `R leftBitXor(T value)` |
| [std::core::add](std-core.md#api-225) | `R add(T left,U right)` |
| [std::core::assert](std-core.md#api-92) | `void assert(bool condition)` |
| [std::core::assert](std-core.md#api-93) | `void assert(bool condition, String message)` |
| [std::core::assert](std-core.md#api-95) | `void assert(bool condition, String message, int64 line)` |
| [std::core::assert](std-core.md#api-94) | `void assert(bool condition, int64 line)` |
| [std::core::barePointer](std-core.md#api-137) | `unsafe BarePtr<T> barePointer<T>(RawPtr<T> pointer)` |
| [std::core::borrowAt](std-core.md#api-144) | `unsafe T mut& borrowAt<T>(RawPtr<T> pointer)` |
| [std::core::bytesToFloat32](std-core.md#api-71) | `Option<float32> bytesToFloat32(List<uint8> bytes)` |
| [std::core::bytesToFloat32](std-core.md#api-70) | `Option<float32> bytesToFloat32(List<uint8> bytes, bool bigEndian)` |
| [std::core::bytesToFloat32](std-core.md#api-75) | `Option<float32> bytesToFloat32(uint8 bytes[])` |
| [std::core::bytesToFloat32](std-core.md#api-74) | `Option<float32> bytesToFloat32(uint8 bytes[], bool bigEndian)` |
| [std::core::bytesToFloat32](std-core.md#api-73) | `Option<float32> bytesToFloat32(uint8 const& bytes, int64 count)` |
| [std::core::bytesToFloat32](std-core.md#api-72) | `Option<float32> bytesToFloat32(uint8 const& bytes, int64 count, bool bigEndian)` |
| [std::core::bytesToFloat64](std-core.md#api-83) | `Option<float64> bytesToFloat64(List<uint8> bytes)` |
| [std::core::bytesToFloat64](std-core.md#api-82) | `Option<float64> bytesToFloat64(List<uint8> bytes, bool bigEndian)` |
| [std::core::bytesToFloat64](std-core.md#api-87) | `Option<float64> bytesToFloat64(uint8 bytes[])` |
| [std::core::bytesToFloat64](std-core.md#api-86) | `Option<float64> bytesToFloat64(uint8 bytes[], bool bigEndian)` |
| [std::core::bytesToFloat64](std-core.md#api-85) | `Option<float64> bytesToFloat64(uint8 const& bytes, int64 count)` |
| [std::core::bytesToFloat64](std-core.md#api-84) | `Option<float64> bytesToFloat64(uint8 const& bytes, int64 count, bool bigEndian)` |
| [std::core::dereference](std-core.md#api-139) | `unsafe T mut& dereference<T>(BarePtr<T> pointer)` |
| [std::core::div](std-core.md#api-228) | `R div(T left,U right)` |
| [std::core::dropAt](std-core.md#api-146) | `unsafe void dropAt<T>(RawPtr<T> pointer)` |
| [std::core::eprint](std-core.md#api-165) | `int64 eprint(String format, Any args...)` |
| [std::core::eprintln](std-core.md#api-166) | `int64 eprintln(String format, Any args...)` |
| [std::core::eq](std-core.md#api-219) | `bool eq(T left,T right)` |
| [std::core::float32ToBytes](std-core.md#api-69) | `List<uint8> float32ToBytes(float32 value)` |
| [std::core::float32ToBytes](std-core.md#api-68) | `List<uint8> float32ToBytes(float32 value, bool bigEndian)` |
| [std::core::float32ToBytes](std-core.md#api-77) | `bool float32ToBytes(float32 value, uint8 mut& bytes, int64 count)` |
| [std::core::float32ToBytes](std-core.md#api-76) | `bool float32ToBytes(float32 value, uint8 mut& bytes, int64 count, bool bigEndian)` |
| [std::core::float32ToBytes](std-core.md#api-79) | `bool float32ToBytes(float32 value, uint8[] mut& bytes)` |
| [std::core::float32ToBytes](std-core.md#api-78) | `bool float32ToBytes(float32 value, uint8[] mut& bytes, bool bigEndian)` |
| [std::core::float64ToBytes](std-core.md#api-81) | `List<uint8> float64ToBytes(float64 value)` |
| [std::core::float64ToBytes](std-core.md#api-80) | `List<uint8> float64ToBytes(float64 value, bool bigEndian)` |
| [std::core::float64ToBytes](std-core.md#api-89) | `bool float64ToBytes(float64 value, uint8 mut& bytes, int64 count)` |
| [std::core::float64ToBytes](std-core.md#api-88) | `bool float64ToBytes(float64 value, uint8 mut& bytes, int64 count, bool bigEndian)` |
| [std::core::float64ToBytes](std-core.md#api-91) | `bool float64ToBytes(float64 value, uint8[] mut& bytes)` |
| [std::core::float64ToBytes](std-core.md#api-90) | `bool float64ToBytes(float64 value, uint8[] mut& bytes, bool bigEndian)` |
| [std::core::format](std-core.md#api-167) | `String format(String format, Any args...)` |
| [std::core::ge](std-core.md#api-224) | `bool ge(T left,T right)` |
| [std::core::gt](std-core.md#api-223) | `bool gt(T left,T right)` |
| [std::core::index](std-core.md#api-230) | `T index(T const& data,int64 index)` |
| [std::core::initAt](std-core.md#api-142) | `unsafe void initAt<T>(RawPtr<T> pointer, T value)` |
| [std::core::inputBigDecimal](std-core.md#api-177) | `BigDecimal inputBigDecimal()` |
| [std::core::inputBigInteger](std-core.md#api-176) | `BigInteger inputBigInteger()` |
| [std::core::inputBool](std-core.md#api-174) | `bool inputBool()` |
| [std::core::inputChar](std-core.md#api-175) | `char inputChar()` |
| [std::core::inputFloat](std-core.md#api-173) | `float64 inputFloat()` |
| [std::core::inputInt](std-core.md#api-171) | `int64 inputInt()` |
| [std::core::inputLine](std-core.md#api-170) | `String inputLine()` |
| [std::core::inputUInt](std-core.md#api-172) | `uint64 inputUInt()` |
| [std::core::isType](std-core.md#api-128) | `bool isType<T>(Any const& value)` |
| [std::core::le](std-core.md#api-222) | `bool le(T left,T right)` |
| [std::core::len](std-core.md#api-179) | `int64 len(Any value)` |
| [std::core::lt](std-core.md#api-221) | `bool lt(T left,T right)` |
| [std::core::memcpy](std-core.md#api-182) | `int64 memcpy(char mut& destination, char const& source, int64 bytes)` |
| [std::core::memcpy](std-core.md#api-140) | `unsafe void memcpy<T,U>(BarePtr<T> destination, BarePtr<U> source, int64 bytes)` |
| [std::core::memset](std-core.md#api-183) | `int64 memset(char mut& destination, int64 value, int64 bytes)` |
| [std::core::memset](std-core.md#api-141) | `unsafe void memset<T>(BarePtr<T> destination, uint8 value, int64 bytes)` |
| [std::core::mod](std-core.md#api-229) | `R mod(T left,U right)` |
| [std::core::mul](std-core.md#api-227) | `R mul(T left,U right)` |
| [std::core::ne](std-core.md#api-220) | `bool ne(T left,T right)` |
| [std::core::panic](std-core.md#api-178) | `int64 panic(String message)` |
| [std::core::pause](std-core.md#api-169) | `int64 pause()` |
| [std::core::pointerFromAddress](std-core.md#api-138) | `unsafe BarePtr<T> pointerFromAddress<T>(int64 address)` |
| [std::core::print](std-core.md#api-163) | `int64 print(String format, Any args...)` |
| [std::core::println](std-core.md#api-164) | `int64 println(String format, Any args...)` |
| [std::core::putChar](std-core.md#api-168) | `int64 putChar(char value)` |
| [std::core::range](std-core.md#api-158) | `__Range range(int64 start, int64 stop)` |
| [std::core::range](std-core.md#api-159) | `__Range range(int64 start, int64 stop, int64 step)` |
| [std::core::rangeEq](std-core.md#api-160) | `__Interval_int64 rangeEq(int64 start, int64 stop)` |
| [std::core::rawSize](std-core.md#api-0) | `int64 rawSize<T>()` |
| [std::core::readAt](std-core.md#api-143) | `unsafe T readAt<T>(RawPtr<T> pointer)` |
| [std::core::readLine](std-core.md#api-61) | `String readLine()` |
| [std::core::readLines](std-core.md#api-62) | `String readLines(int32 N)` |
| [std::core::readLines](std-core.md#api-63) | `String readLines(int64 N)` |
| [std::core::relocateNonOverlapping](std-core.md#api-147) | `unsafe void relocateNonOverlapping<T>(RawPtr<T> destination, RawPtr<T> source, int64 count)` |
| [std::core::stringBytes](std-core.md#api-132) | `List<uint8> stringBytes(String text)` |
| [std::core::sub](std-core.md#api-226) | `R sub(T left,U right)` |
| [std::core::takeAt](std-core.md#api-145) | `unsafe T takeAt<T>(RawPtr<T> pointer)` |
| [std::core::toInt32](std-core.md#api-65) | `Option<int32> toInt32(float32 value)` |
| [std::core::toInt32](std-core.md#api-64) | `Option<int32> toInt32(float64 value)` |
| [std::core::toInt64](std-core.md#api-67) | `int64 toInt64(float32 value)` |
| [std::core::toInt64](std-core.md#api-66) | `int64 toInt64(float64 value)` |
| [std::core::toOptional](std-core.md#api-181) | `Option<T mut&> toOptional<T>(T mut& value)` |
| [std::core::toReference](std-core.md#api-180) | `T mut& toReference<T>(Option<T mut&> value)` |
| [std::core::tryGet](std-core.md#api-129) | `Option<T> tryGet<T>(Any const& value)` |
| [std::core::tryGetMut](std-core.md#api-131) | `Option<T mut&> tryGetMut<T>(Any mut& value)` |
| [std::core::tryGetReference](std-core.md#api-130) | `Option<T const&> tryGetReference<T>(Any const& value)` |
| [std::crypto::hmacSha256](std-crypto.md#api-2) | `Result<String,Error> hmacSha256(String input, String key)` |
| [std::crypto::randomHex](std-crypto.md#api-0) | `Result<String,Error> randomHex(int64 count)` |
| [std::crypto::sha256](std-crypto.md#api-1) | `Result<String,Error> sha256(String input)` |
| [std::crypto::verifyEd25519](std-crypto.md#api-3) | `bool verifyEd25519(String input, String public_key, String signature)` |
| [std::csv::CsvError::toString](std-csv.md#api-0) | `String toString()` |
| [std::csv::csvError](std-csv.md#api-1) | `CsvError csvError(int64 line, int64 column, String message)` |
| [std::csv::parseCsv](std-csv.md#api-2) | `Result<List<List<String>>,Error> parseCsv(String text, char delimiter=',')` |
| [std::csv::writeCsv](std-csv.md#api-3) | `Result<String,Error> writeCsv(List<List<String>> rows, char delimiter=',')` |
| [std::date_time::DateTimeError::toString](std-date_time.md#api-0) | `String toString()` |
| [std::date_time::DateTimeOffset::utc](std-date_time.md#api-11) | `Result<DateTime,Error> utc()` |
| [std::date_time::addDays](std-date_time.md#api-9) | `Result<DateTime,Error> addDays(DateTime value, int64 days)` |
| [std::date_time::addSeconds](std-date_time.md#api-8) | `Result<DateTime,Error> addSeconds(DateTime value, int64 delta)` |
| [std::date_time::atOffset](std-date_time.md#api-12) | `Result<DateTimeOffset,Error> atOffset(DateTime utc, int64 offset_minutes)` |
| [std::date_time::dateDigits](std-date_time.md#api-5) | `String dateDigits(int64 value, int64 width)` |
| [std::date_time::daysInMonth](std-date_time.md#api-2) | `int64 daysInMonth(int64 year, int64 month)` |
| [std::date_time::formatOffset](std-date_time.md#api-13) | `Result<String,Error> formatOffset(DateTimeOffset value)` |
| [std::date_time::formatUtc](std-date_time.md#api-6) | `Result<String,Error> formatUtc(DateTime value)` |
| [std::date_time::fromUnixSeconds](std-date_time.md#api-4) | `Result<DateTime,Error> fromUnixSeconds(int64 seconds)` |
| [std::date_time::isLeapYear](std-date_time.md#api-1) | `bool isLeapYear(int64 year)` |
| [std::date_time::nowUtc](std-date_time.md#api-10) | `Result<DateTime,Error> nowUtc()` |
| [std::date_time::parseOffset](std-date_time.md#api-14) | `Result<DateTimeOffset,Error> parseOffset(String text)` |
| [std::date_time::parseUtc](std-date_time.md#api-7) | `Result<DateTime,Error> parseUtc(String text)` |
| [std::date_time::toUnixSeconds](std-date_time.md#api-3) | `Result<int64,Error> toUnixSeconds(DateTime value)` |
| [std::encoding::EncodingError::toString](std-encoding.md#api-0) | `String toString()` |
| [std::encoding::decodeBase64](std-encoding.md#api-6) | `Result<List<uint8>,Error> decodeBase64(String text, bool url=false)` |
| [std::encoding::decodeBase64Url](std-encoding.md#api-7) | `Result<List<uint8>,Error> decodeBase64Url(String text)` |
| [std::encoding::decodeHex](std-encoding.md#api-3) | `Result<List<uint8>,Error> decodeHex(String text)` |
| [std::encoding::encodeBase64](std-encoding.md#api-4) | `String encodeBase64(List<uint8> bytes, bool url=false, bool padding=true)` |
| [std::encoding::encodeBase64Url](std-encoding.md#api-5) | `String encodeBase64Url(List<uint8> bytes, bool padding=false)` |
| [std::encoding::encodeHex](std-encoding.md#api-1) | `String encodeHex(List<uint8> bytes, bool uppercase=false)` |
| [std::encoding::hexDigit](std-encoding.md#api-2) | `int64 hexDigit(char value)` |
| [std::event::Timer::afterMillis](std-event.md#api-8) | `static Timer afterMillis(int64 millis)` |
| [std::event::Timer::afterNanos](std-event.md#api-6) | `static Timer afterNanos(int64 nanos)` |
| [std::event::Timer::everyMillis](std-event.md#api-7) | `static Timer everyMillis(int64 millis)` |
| [std::event::Timer::everyNanos](std-event.md#api-5) | `static Timer everyNanos(int64 nanos)` |
| [std::event::Timer::id](std-event.md#api-10) | `int64 id()` |
| [std::event::Timer::isValid](std-event.md#api-9) | `bool isValid()` |
| [std::event::Timer::stop](std-event.md#api-11) | `void stop()` |
| [std::event::close](std-event.md#api-4) | `void close()` |
| [std::event::post](std-event.md#api-3) | `int64 post(int64 tag, int64 source)` |
| [std::event::receive](std-event.md#api-1) | `Receive<Event> receive()` |
| [std::event::receiveFor](std-event.md#api-0) | `Receive<Event> receiveFor(int64 millis)` |
| [std::event::tryReceive](std-event.md#api-2) | `Receive<Event> tryReceive()` |
| [std::graph::Graph::addEdge](std-graph.md#api-15) | `void addEdge(int64 from,int64 to,int64 weight)` |
| [std::graph::Graph::bfs](std-graph.md#api-16) | `List<int64> bfs(int64 start)` |
| [std::graph::Graph::connectedComponents](std-graph.md#api-18) | `int64 connectedComponents()` |
| [std::graph::Graph::copy](std-graph.md#api-11) | `Graph copy()` |
| [std::graph::Graph::create](std-graph.md#api-10) | `static Graph create(int64 vertexCount,bool directed)` |
| [std::graph::Graph::dfs](std-graph.md#api-17) | `List<int64> dfs(int64 start)` |
| [std::graph::Graph::dijkstra](std-graph.md#api-20) | `List<int64> dijkstra(int64 start)` |
| [std::graph::Graph::hasCycle](std-graph.md#api-21) | `bool hasCycle()` |
| [std::graph::Graph::isDirected](std-graph.md#api-13) | `bool isDirected()` |
| [std::graph::Graph::len](std-graph.md#api-14) | `int64 len()` |
| [std::graph::Graph::minimumSpanningTreeWeight](std-graph.md#api-22) | `int64 minimumSpanningTreeWeight()` |
| [std::graph::Graph::snapshot](std-graph.md#api-23) | `GraphSnapshot snapshot()` |
| [std::graph::Graph::topologicalSort](std-graph.md#api-19) | `List<int64> topologicalSort()` |
| [std::graph::Graph::vertexCount](std-graph.md#api-12) | `int64 vertexCount()` |
| [std::graph::UnionFind::connected](std-graph.md#api-4) | `bool connected(int64 a,int64 b)` |
| [std::graph::UnionFind::copy](std-graph.md#api-1) | `UnionFind copy()` |
| [std::graph::UnionFind::count](std-graph.md#api-6) | `int64 count()` |
| [std::graph::UnionFind::create](std-graph.md#api-0) | `static UnionFind create(int64 n)` |
| [std::graph::UnionFind::find](std-graph.md#api-2) | `int64 find(int64 x)` |
| [std::graph::UnionFind::len](std-graph.md#api-7) | `int64 len()` |
| [std::graph::UnionFind::parents](std-graph.md#api-8) | `List<int64> parents()` |
| [std::graph::UnionFind::sizeOf](std-graph.md#api-5) | `int64 sizeOf(int64 x)` |
| [std::graph::UnionFind::sizes](std-graph.md#api-9) | `List<int64> sizes()` |
| [std::graph::UnionFind::unionSets](std-graph.md#api-3) | `bool unionSets(int64 a,int64 b)` |
| [std::gui::CustomWidget::blur](std-gui.md#api-183) | `void blur()` |
| [std::gui::CustomWidget::bounds](std-gui.md#api-198) | `Rect bounds()` |
| [std::gui::CustomWidget::checked](std-gui.md#api-197) | `bool checked()` |
| [std::gui::CustomWidget::clearCallbacks](std-gui.md#api-180) | `void clearCallbacks()` |
| [std::gui::CustomWidget::close](std-gui.md#api-181) | `void close()` |
| [std::gui::CustomWidget::draw](std-gui.md#api-200) | `bool draw(Rect bounds)` |
| [std::gui::CustomWidget::emit](std-gui.md#api-202) | `bool emit(Event event)` |
| [std::gui::CustomWidget::enabled](std-gui.md#api-188) | `bool enabled()` |
| [std::gui::CustomWidget::focus](std-gui.md#api-182) | `bool focus()` |
| [std::gui::CustomWidget::handle](std-gui.md#api-201) | `bool handle(Event event)` |
| [std::gui::CustomWidget::hasFocus](std-gui.md#api-184) | `bool hasFocus()` |
| [std::gui::CustomWidget::id](std-gui.md#api-174) | `int64 id()` |
| [std::gui::CustomWidget::isValid](std-gui.md#api-173) | `bool isValid()` |
| [std::gui::CustomWidget::number](std-gui.md#api-195) | `int64 number()` |
| [std::gui::CustomWidget::off](std-gui.md#api-179) | `bool off(int64 id)` |
| [std::gui::CustomWidget::on](std-gui.md#api-177) | `int64 on(String name, void fn(Event) callback)` |
| [std::gui::CustomWidget::onDraw](std-gui.md#api-175) | `void onDraw(void fn(Rect) callback)` |
| [std::gui::CustomWidget::onEvent](std-gui.md#api-176) | `void onEvent(void fn(Event) callback)` |
| [std::gui::CustomWidget::once](std-gui.md#api-178) | `int64 once(String name, void fn(Event) callback)` |
| [std::gui::CustomWidget::setChecked](std-gui.md#api-196) | `bool setChecked(bool value)` |
| [std::gui::CustomWidget::setClipping](std-gui.md#api-199) | `void setClipping(bool enabled)` |
| [std::gui::CustomWidget::setEnabled](std-gui.md#api-186) | `bool setEnabled(bool enabled)` |
| [std::gui::CustomWidget::setNumber](std-gui.md#api-194) | `bool setNumber(int64 value)` |
| [std::gui::CustomWidget::setRange](std-gui.md#api-191) | `bool setRange(float64 minimum, float64 maximum)` |
| [std::gui::CustomWidget::setText](std-gui.md#api-189) | `bool setText(String text)` |
| [std::gui::CustomWidget::setValue](std-gui.md#api-192) | `bool setValue(float64 value)` |
| [std::gui::CustomWidget::setVisible](std-gui.md#api-185) | `bool setVisible(bool visible)` |
| [std::gui::CustomWidget::text](std-gui.md#api-190) | `String text()` |
| [std::gui::CustomWidget::value](std-gui.md#api-193) | `float64 value()` |
| [std::gui::CustomWidget::visible](std-gui.md#api-187) | `bool visible()` |
| [std::gui::Event::changed](std-gui.md#api-9) | `bool changed()` |
| [std::gui::Event::clicked](std-gui.md#api-10) | `bool clicked()` |
| [std::gui::Event::closed](std-gui.md#api-13) | `bool closed()` |
| [std::gui::Event::editingChanged](std-gui.md#api-12) | `bool editingChanged()` |
| [std::gui::Event::focusChanged](std-gui.md#api-17) | `bool focusChanged()` |
| [std::gui::Event::hovered](std-gui.md#api-16) | `bool hovered()` |
| [std::gui::Event::isValid](std-gui.md#api-7) | `bool isValid()` |
| [std::gui::Event::kind](std-gui.md#api-8) | `EventKind kind()` |
| [std::gui::Event::scrolled](std-gui.md#api-15) | `bool scrolled()` |
| [std::gui::Event::selectionChanged](std-gui.md#api-14) | `bool selectionChanged()` |
| [std::gui::Event::submitted](std-gui.md#api-11) | `bool submitted()` |
| [std::gui::EventBus::clear](std-gui.md#api-168) | `void clear()` |
| [std::gui::EventBus::emit](std-gui.md#api-169) | `int64 emit(Event event)` |
| [std::gui::EventBus::off](std-gui.md#api-167) | `bool off(int64 id)` |
| [std::gui::EventBus::on](std-gui.md#api-165) | `int64 on(String name, void fn(Event) callback)` |
| [std::gui::EventBus::once](std-gui.md#api-166) | `int64 once(String name, void fn(Event) callback)` |
| [std::gui::EventBus::pending](std-gui.md#api-171) | `int64 pending()` |
| [std::gui::EventBus::post](std-gui.md#api-170) | `bool post(Event event)` |
| [std::gui::EventBus::pump](std-gui.md#api-172) | `int64 pump(int64 maximum = 256)` |
| [std::gui::Point::at](std-gui.md#api-22) | `static Point at(float64 x, float64 y)` |
| [std::gui::Rect::at](std-gui.md#api-18) | `static Rect at(float64 x, float64 y, float64 width, float64 height)` |
| [std::gui::Rect::contains](std-gui.md#api-19) | `bool contains(float64 x, float64 y)` |
| [std::gui::Rect::intersection](std-gui.md#api-20) | `Rect intersection(Rect other)` |
| [std::gui::Rect::isValid](std-gui.md#api-21) | `bool isValid()` |
| [std::gui::Texture::close](std-gui.md#api-159) | `void close()` |
| [std::gui::Texture::draw](std-gui.md#api-161) | `bool draw(Rect destination, uint32 tint = (uint32)4294967295)` |
| [std::gui::Texture::drawRegion](std-gui.md#api-162) | `bool drawRegion(Rect source, Rect destination, Point origin, float64 rotation, uint32 tint = (uint32)4294967295)` |
| [std::gui::Texture::isValid](std-gui.md#api-158) | `bool isValid()` |
| [std::gui::Texture::size](std-gui.md#api-160) | `Point size()` |
| [std::gui::Widget::beginContent](std-gui.md#api-105) | `bool beginContent()` |
| [std::gui::Widget::brightness](std-gui.md#api-99) | `float64 brightness()` |
| [std::gui::Widget::checked](std-gui.md#api-74) | `bool checked()` |
| [std::gui::Widget::clearCallbacks](std-gui.md#api-57) | `void clearCallbacks()` |
| [std::gui::Widget::close](std-gui.md#api-58) | `void close()` |
| [std::gui::Widget::color](std-gui.md#api-95) | `uint32 color()` |
| [std::gui::Widget::draw](std-gui.md#api-64) | `Event draw(Rect bounds)` |
| [std::gui::Widget::editing](std-gui.md#api-76) | `bool editing()` |
| [std::gui::Widget::enabled](std-gui.md#api-82) | `bool enabled()` |
| [std::gui::Widget::focusIndex](std-gui.md#api-90) | `int64 focusIndex()` |
| [std::gui::Widget::hue](std-gui.md#api-97) | `float64 hue()` |
| [std::gui::Widget::id](std-gui.md#api-56) | `int64 id()` |
| [std::gui::Widget::isValid](std-gui.md#api-55) | `bool isValid()` |
| [std::gui::Widget::number](std-gui.md#api-86) | `int64 number()` |
| [std::gui::Widget::onChange](std-gui.md#api-61) | `void onChange(void fn(Event) callback)` |
| [std::gui::Widget::onClick](std-gui.md#api-60) | `void onClick(void fn(Event) callback)` |
| [std::gui::Widget::onClose](std-gui.md#api-63) | `void onClose(void fn(Event) callback)` |
| [std::gui::Widget::onEvent](std-gui.md#api-59) | `void onEvent(void fn(Event) callback)` |
| [std::gui::Widget::onSubmit](std-gui.md#api-62) | `void onSubmit(void fn(Event) callback)` |
| [std::gui::Widget::result](std-gui.md#api-87) | `int64 result()` |
| [std::gui::Widget::saturation](std-gui.md#api-98) | `float64 saturation()` |
| [std::gui::Widget::scrollIndex](std-gui.md#api-89) | `int64 scrollIndex()` |
| [std::gui::Widget::scrollOffset](std-gui.md#api-103) | `Point scrollOffset()` |
| [std::gui::Widget::secret](std-gui.md#api-78) | `bool secret()` |
| [std::gui::Widget::selected](std-gui.md#api-84) | `int64 selected()` |
| [std::gui::Widget::setButtons](std-gui.md#api-70) | `bool setButtons(String text)` |
| [std::gui::Widget::setChecked](std-gui.md#api-73) | `bool setChecked(bool value)` |
| [std::gui::Widget::setColor](std-gui.md#api-94) | `bool setColor(uint32 rgba)` |
| [std::gui::Widget::setContent](std-gui.md#api-101) | `bool setContent(Rect bounds)` |
| [std::gui::Widget::setEditing](std-gui.md#api-75) | `bool setEditing(bool value)` |
| [std::gui::Widget::setEnabled](std-gui.md#api-81) | `bool setEnabled(bool value)` |
| [std::gui::Widget::setHsv](std-gui.md#api-96) | `bool setHsv(float64 hue, float64 saturation, float64 brightness)` |
| [std::gui::Widget::setItems](std-gui.md#api-71) | `bool setItems(List<String> items)` |
| [std::gui::Widget::setLeftText](std-gui.md#api-67) | `bool setLeftText(String text)` |
| [std::gui::Widget::setMessage](std-gui.md#api-69) | `bool setMessage(String text)` |
| [std::gui::Widget::setNumber](std-gui.md#api-85) | `bool setNumber(int64 value)` |
| [std::gui::Widget::setRange](std-gui.md#api-91) | `bool setRange(float64 minimum, float64 maximum)` |
| [std::gui::Widget::setRightText](std-gui.md#api-68) | `bool setRightText(String text)` |
| [std::gui::Widget::setScrollIndex](std-gui.md#api-88) | `bool setScrollIndex(int64 value)` |
| [std::gui::Widget::setScrollOffset](std-gui.md#api-104) | `bool setScrollOffset(float64 x, float64 y)` |
| [std::gui::Widget::setSecret](std-gui.md#api-77) | `bool setSecret(bool value)` |
| [std::gui::Widget::setSelected](std-gui.md#api-83) | `bool setSelected(int64 value)` |
| [std::gui::Widget::setSpacing](std-gui.md#api-100) | `bool setSpacing(float64 spacing, int64 subdivisions)` |
| [std::gui::Widget::setText](std-gui.md#api-65) | `bool setText(String text)` |
| [std::gui::Widget::setTextCapacity](std-gui.md#api-72) | `bool setTextCapacity(int64 capacity)` |
| [std::gui::Widget::setValue](std-gui.md#api-92) | `bool setValue(float64 value)` |
| [std::gui::Widget::setVisible](std-gui.md#api-79) | `bool setVisible(bool value)` |
| [std::gui::Widget::text](std-gui.md#api-66) | `String text()` |
| [std::gui::Widget::value](std-gui.md#api-93) | `float64 value()` |
| [std::gui::Widget::viewport](std-gui.md#api-102) | `Rect viewport()` |
| [std::gui::Widget::visible](std-gui.md#api-80) | `bool visible()` |
| [std::gui::beginClip](std-gui.md#api-44) | `bool beginClip(Rect bounds)` |
| [std::gui::beginFrame](std-gui.md#api-1) | `bool beginFrame()` |
| [std::gui::button](std-gui.md#api-115) | `Widget button(String text)` |
| [std::gui::button](std-gui.md#api-6) | `bool button(int32 x, int32 y, int32 width, int32 height, String text)` |
| [std::gui::checkBox](std-gui.md#api-120) | `Widget checkBox(String text)` |
| [std::gui::clipBounds](std-gui.md#api-146) | `Rect clipBounds()` |
| [std::gui::close](std-gui.md#api-3) | `void close()` |
| [std::gui::colorBarAlpha](std-gui.md#api-138) | `Widget colorBarAlpha(String text)` |
| [std::gui::colorBarHue](std-gui.md#api-139) | `Widget colorBarHue(String text)` |
| [std::gui::colorPanel](std-gui.md#api-137) | `Widget colorPanel(String text)` |
| [std::gui::colorPanelHsv](std-gui.md#api-141) | `Widget colorPanelHsv(String text)` |
| [std::gui::colorPicker](std-gui.md#api-136) | `Widget colorPicker(String text)` |
| [std::gui::colorPickerHsv](std-gui.md#api-140) | `Widget colorPickerHsv(String text)` |
| [std::gui::comboBox](std-gui.md#api-121) | `Widget comboBox(String text)` |
| [std::gui::controlCount](std-gui.md#api-107) | `int64 controlCount()` |
| [std::gui::controlName](std-gui.md#api-142) | `String controlName(int64 index)` |
| [std::gui::createControl](std-gui.md#api-106) | `Widget createControl(int64 kind, String text)` |
| [std::gui::customEvent](std-gui.md#api-164) | `Event customEvent(String name, String text = "", int64 code = 0, float64 value = 0)` |
| [std::gui::customWidget](std-gui.md#api-203) | `CustomWidget customWidget()` |
| [std::gui::dpiScale](std-gui.md#api-41) | `Point dpiScale()` |
| [std::gui::drawBezier](std-gui.md#api-154) | `bool drawBezier(Point start, Point first_control, Point second_control, Point end, float64 thickness, uint32 color)` |
| [std::gui::drawCircle](std-gui.md#api-150) | `bool drawCircle(Point center, float64 radius, uint32 color, float64 thickness = 0)` |
| [std::gui::drawEllipse](std-gui.md#api-151) | `bool drawEllipse(Point center, Point radius, uint32 color, bool outline = false)` |
| [std::gui::drawLine](std-gui.md#api-148) | `bool drawLine(Point start, Point end, float64 thickness, uint32 color)` |
| [std::gui::drawPixel](std-gui.md#api-147) | `bool drawPixel(Point point, uint32 color)` |
| [std::gui::drawPolygon](std-gui.md#api-153) | `bool drawPolygon(Point center, int64 sides, float64 radius, float64 rotation, uint32 color, float64 thickness = 0)` |
| [std::gui::drawRectangle](std-gui.md#api-149) | `bool drawRectangle(Rect bounds, uint32 color, float64 thickness = 0, float64 roundness = 0)` |
| [std::gui::drawRing](std-gui.md#api-155) | `bool drawRing(Point center, float64 inner_radius, float64 outer_radius, float64 start_angle, float64 end_angle, uint32 color)` |
| [std::gui::drawText](std-gui.md#api-156) | `bool drawText(String text, Point position, float64 size, uint32 color, float64 spacing = 1)` |
| [std::gui::drawTriangle](std-gui.md#api-152) | `bool drawTriangle(Point a, Point b, Point c, uint32 color, bool outline = false)` |
| [std::gui::dropdownBox](std-gui.md#api-122) | `Widget dropdownBox(String text)` |
| [std::gui::dummyRec](std-gui.md#api-130) | `Widget dummyRec(String text)` |
| [std::gui::enable](std-gui.md#api-45) | `bool enable(bool value)` |
| [std::gui::enableTooltips](std-gui.md#api-49) | `bool enableTooltips(bool value)` |
| [std::gui::endClip](std-gui.md#api-25) | `void endClip()` |
| [std::gui::endFrame](std-gui.md#api-2) | `void endFrame()` |
| [std::gui::focused](std-gui.md#api-42) | `bool focused()` |
| [std::gui::frameActive](std-gui.md#api-143) | `bool frameActive()` |
| [std::gui::frameCount](std-gui.md#api-145) | `uint64 frameCount()` |
| [std::gui::frameTime](std-gui.md#api-38) | `float64 frameTime()` |
| [std::gui::getStyle](std-gui.md#api-27) | `int32 getStyle(int32 control, int32 property)` |
| [std::gui::grid](std-gui.md#api-131) | `Widget grid(String text)` |
| [std::gui::groupBox](std-gui.md#api-109) | `Widget groupBox(String text)` |
| [std::gui::height](std-gui.md#api-37) | `int64 height()` |
| [std::gui::inputEnabled](std-gui.md#api-144) | `bool inputEnabled()` |
| [std::gui::label](std-gui.md#api-114) | `Widget label(String text)` |
| [std::gui::label](std-gui.md#api-5) | `void label(int32 x, int32 y, int32 width, int32 height, String text)` |
| [std::gui::labelButton](std-gui.md#api-116) | `Widget labelButton(String text)` |
| [std::gui::line](std-gui.md#api-110) | `Widget line(String text)` |
| [std::gui::listView](std-gui.md#api-132) | `Widget listView(String text)` |
| [std::gui::listViewEx](std-gui.md#api-133) | `Widget listViewEx(String text)` |
| [std::gui::loadFont](std-gui.md#api-28) | `bool loadFont(String path, int32 size, String glyphs)` |
| [std::gui::loadTexture](std-gui.md#api-163) | `Texture loadTexture(String path)` |
| [std::gui::lock](std-gui.md#api-46) | `bool lock(bool value)` |
| [std::gui::measureText](std-gui.md#api-157) | `Point measureText(String text, float64 size, float64 spacing = 1)` |
| [std::gui::messageBox](std-gui.md#api-134) | `Widget messageBox(String text)` |
| [std::gui::mousePosition](std-gui.md#api-39) | `Point mousePosition()` |
| [std::gui::mouseWheel](std-gui.md#api-40) | `Point mouseWheel()` |
| [std::gui::open](std-gui.md#api-4) | `bool open(int32 width, int32 height, String title)` |
| [std::gui::open](std-gui.md#api-30) | `bool open(int64 width, int64 height, String title)` |
| [std::gui::openWindow](std-gui.md#api-29) | `bool openWindow(int64 width, int64 height, String title, int64 options)` |
| [std::gui::panel](std-gui.md#api-111) | `Widget panel(String text)` |
| [std::gui::pollEvent](std-gui.md#api-54) | `Event pollEvent()` |
| [std::gui::progressBar](std-gui.md#api-128) | `Widget progressBar(String text)` |
| [std::gui::requestClose](std-gui.md#api-23) | `void requestClose()` |
| [std::gui::resetStyle](std-gui.md#api-50) | `bool resetStyle()` |
| [std::gui::rgba](std-gui.md#api-53) | `uint32 rgba(int64 red, int64 green, int64 blue, int64 alpha)` |
| [std::gui::run](std-gui.md#api-205) | `void run(void fn() draw)` |
| [std::gui::run](std-gui.md#api-204) | `void run(void fn() draw, void fn(Event) onEvent)` |
| [std::gui::scrollPanel](std-gui.md#api-113) | `Widget scrollPanel(String text)` |
| [std::gui::setAlpha](std-gui.md#api-47) | `bool setAlpha(float64 value)` |
| [std::gui::setBackground](std-gui.md#api-24) | `void setBackground(uint32 rgba)` |
| [std::gui::setExitKey](std-gui.md#api-33) | `bool setExitKey(int64 key)` |
| [std::gui::setFontSize](std-gui.md#api-51) | `bool setFontSize(int64 value)` |
| [std::gui::setMinSize](std-gui.md#api-34) | `bool setMinSize(int64 width, int64 height)` |
| [std::gui::setOpacity](std-gui.md#api-35) | `bool setOpacity(float64 value)` |
| [std::gui::setState](std-gui.md#api-48) | `bool setState(int64 value)` |
| [std::gui::setStyle](std-gui.md#api-26) | `bool setStyle(int32 control, int32 property, int32 value)` |
| [std::gui::setTargetFps](std-gui.md#api-32) | `bool setTargetFps(int64 fps)` |
| [std::gui::setTitle](std-gui.md#api-31) | `bool setTitle(String text)` |
| [std::gui::setTooltip](std-gui.md#api-52) | `bool setTooltip(String text)` |
| [std::gui::shouldClose](std-gui.md#api-0) | `bool shouldClose()` |
| [std::gui::slider](std-gui.md#api-126) | `Widget slider(String text)` |
| [std::gui::sliderBar](std-gui.md#api-127) | `Widget sliderBar(String text)` |
| [std::gui::spinner](std-gui.md#api-123) | `Widget spinner(String text)` |
| [std::gui::statusBar](std-gui.md#api-129) | `Widget statusBar(String text)` |
| [std::gui::tabBar](std-gui.md#api-112) | `Widget tabBar(String text)` |
| [std::gui::textBox](std-gui.md#api-125) | `Widget textBox(String text)` |
| [std::gui::textInputBox](std-gui.md#api-135) | `Widget textInputBox(String text)` |
| [std::gui::time](std-gui.md#api-43) | `float64 time()` |
| [std::gui::toggle](std-gui.md#api-117) | `Widget toggle(String text)` |
| [std::gui::toggleGroup](std-gui.md#api-118) | `Widget toggleGroup(String text)` |
| [std::gui::toggleSlider](std-gui.md#api-119) | `Widget toggleSlider(String text)` |
| [std::gui::valueBox](std-gui.md#api-124) | `Widget valueBox(String text)` |
| [std::gui::width](std-gui.md#api-36) | `int64 width()` |
| [std::gui::windowBox](std-gui.md#api-108) | `Widget windowBox(String text)` |
| [std::image::Image::binarize](std-image.md#api-23) | `static Result<List<uint8>, Error> binarize(int64 width, int64 height, List<uint8> gray, int64 threshold)` |
| [std::image::Image::convolve](std-image.md#api-14) | `Result<Image, Error> convolve(List<float64> kernel, int64 kw, int64 kh)` |
| [std::image::Image::decode](std-image.md#api-4) | `static Result<Image, Error> decode(List<uint8> bytes)` |
| [std::image::Image::encodeBmp](std-image.md#api-6) | `Result<List<uint8>, Error> encodeBmp()` |
| [std::image::Image::encodePgm](std-image.md#api-8) | `Result<List<uint8>, Error> encodePgm()` |
| [std::image::Image::encodePpm](std-image.md#api-7) | `Result<List<uint8>, Error> encodePpm()` |
| [std::image::Image::fromARGB](std-image.md#api-19) | `static Result<Image, Error> fromARGB(int64 width, int64 height, List<uint32> argb)` |
| [std::image::Image::fromGray](std-image.md#api-17) | `static Result<Image, Error> fromGray(int64 width, int64 height, List<uint8> gray)` |
| [std::image::Image::fromRGB565](std-image.md#api-21) | `static Result<Image, Error> fromRGB565(int64 width, int64 height, List<uint16> rgb565)` |
| [std::image::Image::getPixel](std-image.md#api-12) | `Result<Pixel, Error> getPixel(int64 x, int64 y)` |
| [std::image::Image::height](std-image.md#api-1) | `int64 height()` |
| [std::image::Image::make](std-image.md#api-3) | `static Result<Image, Error> make(int64 width, int64 height)` |
| [std::image::Image::open](std-image.md#api-5) | `static Result<Image, Error> open(String path)` |
| [std::image::Image::otsuBinarize](std-image.md#api-24) | `Result<List<uint8>, Error> otsuBinarize()` |
| [std::image::Image::otsuThreshold](std-image.md#api-22) | `Result<int64, Error> otsuThreshold()` |
| [std::image::Image::pixels](std-image.md#api-2) | `List<uint8> pixels()` |
| [std::image::Image::pool](std-image.md#api-15) | `Result<Image, Error> pool(int64 mode, int64 kw, int64 kh)` |
| [std::image::Image::saveBmp](std-image.md#api-9) | `Result<int64, Error> saveBmp(String path)` |
| [std::image::Image::savePgm](std-image.md#api-11) | `Result<int64, Error> savePgm(String path)` |
| [std::image::Image::savePpm](std-image.md#api-10) | `Result<int64, Error> savePpm(String path)` |
| [std::image::Image::setPixel](std-image.md#api-13) | `Result<int64, Error> setPixel(int64 x, int64 y, int64 r, int64 g, int64 b, int64 a)` |
| [std::image::Image::toARGB](std-image.md#api-18) | `Result<List<uint32>, Error> toARGB()` |
| [std::image::Image::toGray](std-image.md#api-16) | `Result<List<uint8>, Error> toGray()` |
| [std::image::Image::toRGB565](std-image.md#api-20) | `Result<List<uint16>, Error> toRGB565()` |
| [std::image::Image::width](std-image.md#api-0) | `int64 width()` |
| [std::io::FileStream::close](std-io.md#api-21) | `int64 close()` |
| [std::io::FileStream::flush](std-io.md#api-19) | `Result<int64, Error> flush()` |
| [std::io::FileStream::open](std-io.md#api-7) | `static Result<FileStream, Error> open(String path, String mode)` |
| [std::io::FileStream::print](std-io.md#api-13) | `Result<int64, Error> print(String text)` |
| [std::io::FileStream::println](std-io.md#api-14) | `Result<int64, Error> println(String text = "")` |
| [std::io::FileStream::read](std-io.md#api-8) | `Result<String, Error> read()` |
| [std::io::FileStream::readAllString](std-io.md#api-11) | `Result<String,Error> readAllString(int64 maxBytes=16777216)` |
| [std::io::FileStream::readBytes](std-io.md#api-15) | `Result<List<uint8>,Error> readBytes(int64 count)` |
| [std::io::FileStream::readInto](std-io.md#api-17) | `Result<int64, Error> readInto(uint8[] mut& buffer)` |
| [std::io::FileStream::readLine](std-io.md#api-9) | `Result<String,Error> readLine()` |
| [std::io::FileStream::readString](std-io.md#api-10) | `Result<String,Error> readString(int64 count)` |
| [std::io::FileStream::seek](std-io.md#api-20) | `Result<int64, Error> seek(int64 offset, SeekOrigin origin)` |
| [std::io::FileStream::write](std-io.md#api-12) | `Result<int64, Error> write(String text)` |
| [std::io::FileStream::writeBytes](std-io.md#api-16) | `Result<int64,Error> writeBytes(List<uint8> bytes)` |
| [std::io::FileStream::writeFrom](std-io.md#api-18) | `Result<int64, Error> writeFrom(uint8[] const& buffer, int64 count)` |
| [std::io::Formattable::toString](std-io.md#api-6) | `String toString(String style)` |
| [std::io::IoError::toString](std-io.md#api-0) | `String toString()` |
| [std::io::Reader::read](std-io.md#api-1) | `Result<String, Error> read()` |
| [std::io::Seek::seek](std-io.md#api-4) | `Result<int64, Error> seek(int64 offset, SeekOrigin origin)` |
| [std::io::ToString::toString](std-io.md#api-5) | `String toString()` |
| [std::io::Writer::flush](std-io.md#api-3) | `Result<int64, Error> flush()` |
| [std::io::Writer::write](std-io.md#api-2) | `Result<int64, Error> write(String text)` |
| [std::io::copyTo](std-io.md#api-40) | `Result<int64,Error> copyTo(FileStream mut& source, FileStream mut& destination)` |
| [std::io::createDirectories](std-io.md#api-38) | `Result<int64,Error> createDirectories(String path)` |
| [std::io::createTempFile](std-io.md#api-42) | `Result<String,Error> createTempFile(String path)` |
| [std::io::fileClose](std-io.md#api-31) | `int64 fileClose(file const& handle)` |
| [std::io::fileEof](std-io.md#api-30) | `int64 fileEof(file const& handle)` |
| [std::io::fileInfo](std-io.md#api-37) | `Result<FileInfo,Error> fileInfo(String path)` |
| [std::io::fileOpen](std-io.md#api-22) | `Result<File, Error> fileOpen(String path, String mode)` |
| [std::io::filePrint](std-io.md#api-28) | `int64 filePrint(file const& handle, String text)` |
| [std::io::filePrintln](std-io.md#api-29) | `int64 filePrintln(file const& handle, String text = "")` |
| [std::io::fileRead](std-io.md#api-23) | `String fileRead(file const& handle)` |
| [std::io::fileReadAllString](std-io.md#api-26) | `String fileReadAllString(File const& handle, int64 maxBytes = 16777216)` |
| [std::io::fileReadLine](std-io.md#api-24) | `String fileReadLine(File const& handle)` |
| [std::io::fileReadString](std-io.md#api-25) | `String fileReadString(File const& handle, int64 count)` |
| [std::io::fileWrite](std-io.md#api-27) | `int64 fileWrite(file const& handle, String text)` |
| [std::io::listDirectory](std-io.md#api-36) | `Result<List<String>,Error> listDirectory(String path)` |
| [std::io::readAllBytes](std-io.md#api-32) | `Result<List<uint8>,Error> readAllBytes(String path, int64 maxBytes=16777216)` |
| [std::io::readAllText](std-io.md#api-34) | `Result<String,Error> readAllText(String path, int64 maxBytes=16777216)` |
| [std::io::renamePath](std-io.md#api-39) | `Result<int64,Error> renamePath(String from, String to)` |
| [std::io::replaceFile](std-io.md#api-43) | `Result<int64,Error> replaceFile(String from, String to)` |
| [std::io::walkDirectory](std-io.md#api-41) | `Result<List<String>,Error> walkDirectory(String path)` |
| [std::io::writeAllBytes](std-io.md#api-33) | `Result<int64,Error> writeAllBytes(String path, List<uint8> bytes)` |
| [std::io::writeAllText](std-io.md#api-35) | `Result<int64,Error> writeAllText(String path, String text)` |
| [std::iter::Iter::all](std-iter.md#api-9) | `bool all(bool fn(T) predicate)` |
| [std::iter::Iter::any](std-iter.md#api-10) | `bool any(bool fn(T) predicate)` |
| [std::iter::Iter::count](std-iter.md#api-6) | `int64 count()` |
| [std::iter::Iter::find](std-iter.md#api-11) | `Option<T> find(bool fn(T) predicate)` |
| [std::iter::Iter::forEach](std-iter.md#api-12) | `void forEach(void fn(T) action)` |
| [std::iter::Iter::iterator](std-iter.md#api-5) | `Option<T> iterator(int64 index)` |
| [std::iter::Iter::last](std-iter.md#api-8) | `Option<T> last()` |
| [std::iter::Iter::next](std-iter.md#api-3) | `Option<T> next()` |
| [std::iter::Iter::nth](std-iter.md#api-7) | `Option<T> nth(int64 index)` |
| [std::iter::Iter::peek](std-iter.md#api-2) | `Option<T> peek()` |
| [std::iter::Iter::sizeHint](std-iter.md#api-4) | `(int64, Option<int64>) sizeHint()` |
| [std::iter::IterBufferState::next](std-iter.md#api-77) | `Option<T> next()` |
| [std::iter::IterBufferState::sizeHint](std-iter.md#api-78) | `(int64,Option<int64>) sizeHint()` |
| [std::iter::IterChainState::next](std-iter.md#api-48) | `Option<T> next()` |
| [std::iter::IterChainState::sizeHint](std-iter.md#api-49) | `(int64, Option<int64>) sizeHint()` |
| [std::iter::IterCharsState::next](std-iter.md#api-103) | `Option<char> next()` |
| [std::iter::IterCharsState::sizeHint](std-iter.md#api-104) | `(int64, Option<int64>) sizeHint()` |
| [std::iter::IterConstListState::next](std-iter.md#api-90) | `Option<T const&> next()` |
| [std::iter::IterConstListState::sizeHint](std-iter.md#api-91) | `(int64, Option<int64>) sizeHint()` |
| [std::iter::IterCycleState::next](std-iter.md#api-66) | `Option<T> next()` |
| [std::iter::IterCycleState::sizeHint](std-iter.md#api-67) | `(int64, Option<int64>) sizeHint()` |
| [std::iter::IterEnumerateState::next](std-iter.md#api-54) | `Option<(int64,T)> next()` |
| [std::iter::IterEnumerateState::sizeHint](std-iter.md#api-55) | `(int64, Option<int64>) sizeHint()` |
| [std::iter::IterFilterMapState::next](std-iter.md#api-30) | `Option<U> next()` |
| [std::iter::IterFilterMapState::sizeHint](std-iter.md#api-31) | `(int64, Option<int64>) sizeHint()` |
| [std::iter::IterFilterState::next](std-iter.md#api-27) | `Option<T> next()` |
| [std::iter::IterFilterState::sizeHint](std-iter.md#api-28) | `(int64, Option<int64>) sizeHint()` |
| [std::iter::IterFuseState::next](std-iter.md#api-60) | `Option<T> next()` |
| [std::iter::IterFuseState::sizeHint](std-iter.md#api-61) | `(int64, Option<int64>) sizeHint()` |
| [std::iter::IterInspectState::next](std-iter.md#api-63) | `Option<T> next()` |
| [std::iter::IterInspectState::sizeHint](std-iter.md#api-64) | `(int64, Option<int64>) sizeHint()` |
| [std::iter::IterListState::next](std-iter.md#api-17) | `Option<T> next()` |
| [std::iter::IterListState::sizeHint](std-iter.md#api-18) | `(int64, Option<int64>) sizeHint()` |
| [std::iter::IterMapState::next](std-iter.md#api-24) | `Option<U> next()` |
| [std::iter::IterMapState::sizeHint](std-iter.md#api-25) | `(int64, Option<int64>) sizeHint()` |
| [std::iter::IterMapWhileState::next](std-iter.md#api-33) | `Option<U> next()` |
| [std::iter::IterMapWhileState::sizeHint](std-iter.md#api-34) | `(int64, Option<int64>) sizeHint()` |
| [std::iter::IterMutListState::next](std-iter.md#api-92) | `Option<T mut&> next()` |
| [std::iter::IterMutListState::sizeHint](std-iter.md#api-93) | `(int64, Option<int64>) sizeHint()` |
| [std::iter::IterNext::next](std-iter.md#api-0) | `Option<T> next()` |
| [std::iter::IterNext::sizeHint](std-iter.md#api-1) | `(int64, Option<int64>) sizeHint()` |
| [std::iter::IterRangeState::next](std-iter.md#api-20) | `Option<int64> next()` |
| [std::iter::IterRangeState::sizeHint](std-iter.md#api-21) | `(int64, Option<int64>) sizeHint()` |
| [std::iter::IterRepeatState::next](std-iter.md#api-106) | `Option<T> next()` |
| [std::iter::IterRepeatState::sizeHint](std-iter.md#api-107) | `(int64, Option<int64>) sizeHint()` |
| [std::iter::IterScanState::next](std-iter.md#api-69) | `Option<U> next()` |
| [std::iter::IterScanState::sizeHint](std-iter.md#api-70) | `(int64, Option<int64>) sizeHint()` |
| [std::iter::IterSkipState::next](std-iter.md#api-45) | `Option<T> next()` |
| [std::iter::IterSkipState::sizeHint](std-iter.md#api-46) | `(int64, Option<int64>) sizeHint()` |
| [std::iter::IterSkipWhileState::next](std-iter.md#api-39) | `Option<T> next()` |
| [std::iter::IterSkipWhileState::sizeHint](std-iter.md#api-40) | `(int64, Option<int64>) sizeHint()` |
| [std::iter::IterSpanState::next](std-iter.md#api-100) | `Option<T const&> next()` |
| [std::iter::IterSpanState::sizeHint](std-iter.md#api-101) | `(int64, Option<int64>) sizeHint()` |
| [std::iter::IterStepState::next](std-iter.md#api-57) | `Option<T> next()` |
| [std::iter::IterStepState::sizeHint](std-iter.md#api-58) | `(int64, Option<int64>) sizeHint()` |
| [std::iter::IterTakeState::next](std-iter.md#api-42) | `Option<T> next()` |
| [std::iter::IterTakeState::sizeHint](std-iter.md#api-43) | `(int64, Option<int64>) sizeHint()` |
| [std::iter::IterTakeWhileState::next](std-iter.md#api-36) | `Option<T> next()` |
| [std::iter::IterTakeWhileState::sizeHint](std-iter.md#api-37) | `(int64, Option<int64>) sizeHint()` |
| [std::iter::IterZipState::next](std-iter.md#api-51) | `Option<(T,U)> next()` |
| [std::iter::IterZipState::sizeHint](std-iter.md#api-52) | `(int64, Option<int64>) sizeHint()` |
| [std::iter::IteratorValueState::next](std-iter.md#api-14) | `Option<T> next()` |
| [std::iter::IteratorValueState::sizeHint](std-iter.md#api-15) | `(int64, Option<int64>) sizeHint()` |
| [std::iter::all](std-iter.md#api-84) | `bool all<T>(Iter<T> mut& source, bool fn(T) predicate)` |
| [std::iter::any](std-iter.md#api-85) | `bool any<T>(Iter<T> mut& source, bool fn(T) predicate)` |
| [std::iter::chain](std-iter.md#api-50) | `Iter<T> chain<T>(Iter<T> left, Iter<T> right)` |
| [std::iter::chars](std-iter.md#api-105) | `Iter<char> chars(String source)` |
| [std::iter::chunk](std-iter.md#api-110) | `Iter<List<T>> chunk<T>(Iter<T> source, int64 length)` |
| [std::iter::collect](std-iter.md#api-72) | `List<T> collect<T>(Iter<T> source)` |
| [std::iter::countIter](std-iter.md#api-81) | `int64 countIter<T>(Iter<T> mut& source)` |
| [std::iter::cycle](std-iter.md#api-68) | `Iter<T> cycle<T>(Iter<T> source)` |
| [std::iter::enumerate](std-iter.md#api-56) | `Iter<(int64,T)> enumerate<T>(Iter<T> source)` |
| [std::iter::filter](std-iter.md#api-29) | `Iter<T> filter<T>(Iter<T> source, bool fn(T) predicate)` |
| [std::iter::filterMap](std-iter.md#api-32) | `Iter<U> filterMap<T,U>(Iter<T> source, Option<U> fn(T) callback)` |
| [std::iter::find](std-iter.md#api-86) | `Option<T> find<T>(Iter<T> mut& source, bool fn(T) predicate)` |
| [std::iter::findMap](std-iter.md#api-75) | `Option<U> findMap<T,U>(Iter<T> source, Option<U> fn(T) callback)` |
| [std::iter::fold](std-iter.md#api-73) | `A fold<T,A>(Iter<T> source, A initial, A fn(A, T) callback)` |
| [std::iter::forEach](std-iter.md#api-87) | `void forEach<T>(Iter<T> mut& source, void fn(T) action)` |
| [std::iter::fromIterator](std-iter.md#api-16) | `Iter<T> fromIterator<T,S : Iterator<T>>(S source)` |
| [std::iter::fromList](std-iter.md#api-19) | `Iter<T> fromList<T>(List<T> values)` |
| [std::iter::fromRange](std-iter.md#api-89) | `Iter<int64> fromRange(std::core::__Range source)` |
| [std::iter::fromSpan](std-iter.md#api-102) | `Iter<T const&> fromSpan<T>(T const& source, int64 length)` |
| [std::iter::fromState](std-iter.md#api-13) | `Iter<T> fromState<T,S>(S state)` |
| [std::iter::fuse](std-iter.md#api-62) | `Iter<T> fuse<T>(Iter<T> source)` |
| [std::iter::groupBy](std-iter.md#api-109) | `Map<K,List<T>> groupBy<T,K>(Iter<T> source, K fn(T) key_of)` |
| [std::iter::inspect](std-iter.md#api-65) | `Iter<T> inspect<T>(Iter<T> source, void fn(T) action)` |
| [std::iter::intIter](std-iter.md#api-23) | `Iter<int64> intIter(int64 start, int64 stop)` |
| [std::iter::intIter](std-iter.md#api-22) | `Iter<int64> intIter(int64 start, int64 stop, int64 step)` |
| [std::iter::last](std-iter.md#api-83) | `Option<T> last<T>(Iter<T> mut& source)` |
| [std::iter::listIntoIter](std-iter.md#api-88) | `Iter<T> listIntoIter<T>(List<T> mut& source)` |
| [std::iter::listIter](std-iter.md#api-94) | `Iter<T const&> listIter<T>(List<T> const& source)` |
| [std::iter::listIterMut](std-iter.md#api-95) | `Iter<T mut&> listIterMut<T>(List<T> mut& source)` |
| [std::iter::map](std-iter.md#api-26) | `Iter<U> map<T,U>(Iter<T> source, U fn(T) callback)` |
| [std::iter::mapIntoIter](std-iter.md#api-98) | `Iter<Pair<K,V>> mapIntoIter<K,V>(Map<K,V> mut& source)` |
| [std::iter::mapIter](std-iter.md#api-99) | `Iter<Pair<K,V>> mapIter<K,V>(Map<K,V> const& source)` |
| [std::iter::mapWhile](std-iter.md#api-35) | `Iter<U> mapWhile<T,U>(Iter<T> source, Option<U> fn(T) callback)` |
| [std::iter::nth](std-iter.md#api-82) | `Option<T> nth<T>(Iter<T> mut& source, int64 index)` |
| [std::iter::peekingTakeWhile](std-iter.md#api-80) | `(Iter<T>,Iter<T>) peekingTakeWhile<T>(Iter<T> source, bool fn(T) predicate)` |
| [std::iter::repeat](std-iter.md#api-108) | `Iter<T> repeat<T>(T value)` |
| [std::iter::rev](std-iter.md#api-79) | `Iter<T> rev<T>(Iter<T> source)` |
| [std::iter::sameItems](std-iter.md#api-76) | `bool sameItems<T>(Iter<T> left, Iter<T> right)` |
| [std::iter::scan](std-iter.md#api-71) | `Iter<U> scan<T,A,U>(Iter<T> source, A initial, Option<U> fn(A mut&, T) callback)` |
| [std::iter::setIntoIter](std-iter.md#api-96) | `Iter<T> setIntoIter<T>(Set<T> mut& source)` |
| [std::iter::setIter](std-iter.md#api-97) | `Iter<T> setIter<T>(Set<T> const& source)` |
| [std::iter::skip](std-iter.md#api-47) | `Iter<T> skip<T>(Iter<T> source, int64 amount)` |
| [std::iter::skipWhile](std-iter.md#api-41) | `Iter<T> skipWhile<T>(Iter<T> source, bool fn(T) predicate)` |
| [std::iter::stepBy](std-iter.md#api-59) | `Iter<T> stepBy<T>(Iter<T> source, int64 step)` |
| [std::iter::take](std-iter.md#api-44) | `Iter<T> take<T>(Iter<T> source, int64 amount)` |
| [std::iter::takeWhile](std-iter.md#api-38) | `Iter<T> takeWhile<T>(Iter<T> source, bool fn(T) predicate)` |
| [std::iter::tryFold](std-iter.md#api-74) | `Result<A,E> tryFold<T,A,E>(Iter<T> source, A initial, Result<A,E> fn(A, T) callback)` |
| [std::iter::zip](std-iter.md#api-53) | `Iter<(T,U)> zip<T,U>(Iter<T> left, Iter<U> right)` |
| [std::json::Json::toJson](std-json.md#api-0) | `Result<String,Error> toJson()` |
| [std::json::JsonValue::asBool](std-json.md#api-18) | `Option<bool> asBool()` |
| [std::json::JsonValue::asBytes](std-json.md#api-20) | `Option<List<uint8>> asBytes()` |
| [std::json::JsonValue::asDateTime](std-json.md#api-21) | `Option<String> asDateTime()` |
| [std::json::JsonValue::asFloat64](std-json.md#api-17) | `Option<float64> asFloat64()` |
| [std::json::JsonValue::asInt](std-json.md#api-16) | `Option<int64> asInt()` |
| [std::json::JsonValue::asString](std-json.md#api-19) | `Option<String> asString()` |
| [std::json::JsonValue::at](std-json.md#api-24) | `Option<JsonValue> at(int64 index)` |
| [std::json::JsonValue::close](std-json.md#api-4) | `bool close()` |
| [std::json::JsonValue::dump](std-json.md#api-28) | `String dump()` |
| [std::json::JsonValue::find](std-json.md#api-22) | `Option<JsonValue> find(String key)` |
| [std::json::JsonValue::findBytes](std-json.md#api-23) | `Option<JsonValue> findBytes(List<uint8> key)` |
| [std::json::JsonValue::isArray](std-json.md#api-11) | `bool isArray()` |
| [std::json::JsonValue::isBool](std-json.md#api-7) | `bool isBool()` |
| [std::json::JsonValue::isDateTime](std-json.md#api-14) | `bool isDateTime()` |
| [std::json::JsonValue::isFloat](std-json.md#api-9) | `bool isFloat()` |
| [std::json::JsonValue::isInt](std-json.md#api-8) | `bool isInt()` |
| [std::json::JsonValue::isNull](std-json.md#api-6) | `bool isNull()` |
| [std::json::JsonValue::isObject](std-json.md#api-12) | `bool isObject()` |
| [std::json::JsonValue::isString](std-json.md#api-10) | `bool isString()` |
| [std::json::JsonValue::isTable](std-json.md#api-13) | `bool isTable()` |
| [std::json::JsonValue::isValid](std-json.md#api-3) | `bool isValid()` |
| [std::json::JsonValue::iterator](std-json.md#api-25) | `Option<JsonValue> iterator(int64 index)` |
| [std::json::JsonValue::keyAt](std-json.md#api-26) | `Option<String> keyAt(int64 index)` |
| [std::json::JsonValue::keyBytesAt](std-json.md#api-27) | `Option<List<uint8>> keyBytesAt(int64 index)` |
| [std::json::JsonValue::kind](std-json.md#api-5) | `int64 kind()` |
| [std::json::JsonValue::len](std-json.md#api-15) | `int64 len()` |
| [std::json::deserialize](std-json.md#api-2) | `Result<T,Error> deserialize<T>(String text)` |
| [std::json::parse](std-json.md#api-29) | `Result<JsonValue, Error> parse(String text)` |
| [std::json::serialize](std-json.md#api-1) | `Result<String,Error> serialize<T>(T const& value)` |
| [std::linear_algebra::Matrix::add](std-linear_algebra.md#api-39) | `Matrix<T> add(Matrix<T> other)` |
| [std::linear_algebra::Matrix::apply](std-linear_algebra.md#api-43) | `Vector<T> apply(Vector<T> vector)` |
| [std::linear_algebra::Matrix::at](std-linear_algebra.md#api-36) | `T at(int64 row, int64 col)` |
| [std::linear_algebra::Matrix::column](std-linear_algebra.md#api-32) | `Vector<T> column(int64 key)` |
| [std::linear_algebra::Matrix::drop](std-linear_algebra.md#api-27) | `void drop()` |
| [std::linear_algebra::Matrix::fromArray](std-linear_algebra.md#api-29) | `static Matrix<T> fromArray(T const& data, int64 count, int64 rows, int64 cols)` |
| [std::linear_algebra::Matrix::fromArray](std-linear_algebra.md#api-30) | `static Matrix<T> fromArray(T data[], int64 rows, int64 cols)` |
| [std::linear_algebra::Matrix::identity](std-linear_algebra.md#api-35) | `static Matrix<T> identity(int64 n)` |
| [std::linear_algebra::Matrix::index](std-linear_algebra.md#api-33) | `Vector<T> index(int64 key)` |
| [std::linear_algebra::Matrix::iterator](std-linear_algebra.md#api-34) | `Option<Vector<T>> iterator(int64 position)` |
| [std::linear_algebra::Matrix::leftAdd](std-linear_algebra.md#api-45) | `Matrix<T> leftAdd(Matrix<T> right)` |
| [std::linear_algebra::Matrix::leftMul](std-linear_algebra.md#api-47) | `Matrix<T> leftMul(Matrix<T> right)` |
| [std::linear_algebra::Matrix::leftMul](std-linear_algebra.md#api-49) | `Matrix<T> leftMul(T right)` |
| [std::linear_algebra::Matrix::leftMul](std-linear_algebra.md#api-48) | `Vector<T> leftMul(Vector<T> right)` |
| [std::linear_algebra::Matrix::leftSub](std-linear_algebra.md#api-46) | `Matrix<T> leftSub(Matrix<T> right)` |
| [std::linear_algebra::Matrix::multiply](std-linear_algebra.md#api-42) | `Matrix<T> multiply(Matrix<T> other)` |
| [std::linear_algebra::Matrix::scale](std-linear_algebra.md#api-41) | `Matrix<T> scale(T factor)` |
| [std::linear_algebra::Matrix::set](std-linear_algebra.md#api-37) | `void set(int64 row, int64 col, T value)` |
| [std::linear_algebra::Matrix::solve](std-linear_algebra.md#api-44) | `Vector<T> solve(Vector<T> rhs)` |
| [std::linear_algebra::Matrix::subtract](std-linear_algebra.md#api-40) | `Matrix<T> subtract(Matrix<T> other)` |
| [std::linear_algebra::Matrix::toArray](std-linear_algebra.md#api-31) | `T[] toArray()` |
| [std::linear_algebra::Matrix::transpose](std-linear_algebra.md#api-38) | `Matrix<T> transpose()` |
| [std::linear_algebra::Matrix::zeros](std-linear_algebra.md#api-28) | `static Matrix<T> zeros(int64 rows, int64 cols)` |
| [std::linear_algebra::ScalarArray::at](std-linear_algebra.md#api-2) | `T at(int64 index)` |
| [std::linear_algebra::ScalarArray::clear](std-linear_algebra.md#api-1) | `void clear()` |
| [std::linear_algebra::ScalarArray::copy](std-linear_algebra.md#api-6) | `ScalarArray<T> copy()` |
| [std::linear_algebra::ScalarArray::index](std-linear_algebra.md#api-3) | `T index(int64 key)` |
| [std::linear_algebra::ScalarArray::iterator](std-linear_algebra.md#api-7) | `Option<T> iterator(int64 position)` |
| [std::linear_algebra::ScalarArray::len](std-linear_algebra.md#api-0) | `int64 len()` |
| [std::linear_algebra::ScalarArray::push](std-linear_algebra.md#api-5) | `void push(T value)` |
| [std::linear_algebra::ScalarArray::set](std-linear_algebra.md#api-4) | `void set(int64 index, T value)` |
| [std::linear_algebra::Vector::add](std-linear_algebra.md#api-21) | `Vector<T> add(Vector<T> other)` |
| [std::linear_algebra::Vector::at](std-linear_algebra.md#api-16) | `T at(int64 i)` |
| [std::linear_algebra::Vector::dot](std-linear_algebra.md#api-18) | `T dot(Vector<T> other)` |
| [std::linear_algebra::Vector::drop](std-linear_algebra.md#api-8) | `void drop()` |
| [std::linear_algebra::Vector::fromArray](std-linear_algebra.md#api-10) | `static Vector<T> fromArray(T const& data, int64 count)` |
| [std::linear_algebra::Vector::fromArray](std-linear_algebra.md#api-11) | `static Vector<T> fromArray(T data[])` |
| [std::linear_algebra::Vector::index](std-linear_algebra.md#api-13) | `T index(int64 key)` |
| [std::linear_algebra::Vector::iterator](std-linear_algebra.md#api-14) | `Option<T> iterator(int64 position)` |
| [std::linear_algebra::Vector::leftAdd](std-linear_algebra.md#api-24) | `Vector<T> leftAdd(Vector<T> right)` |
| [std::linear_algebra::Vector::leftMul](std-linear_algebra.md#api-26) | `Vector<T> leftMul(T right)` |
| [std::linear_algebra::Vector::leftSub](std-linear_algebra.md#api-25) | `Vector<T> leftSub(Vector<T> right)` |
| [std::linear_algebra::Vector::len](std-linear_algebra.md#api-15) | `int64 len()` |
| [std::linear_algebra::Vector::norm](std-linear_algebra.md#api-19) | `float64 norm()` |
| [std::linear_algebra::Vector::normalized](std-linear_algebra.md#api-20) | `Vector<T> normalized()` |
| [std::linear_algebra::Vector::scale](std-linear_algebra.md#api-23) | `Vector<T> scale(T amount)` |
| [std::linear_algebra::Vector::set](std-linear_algebra.md#api-17) | `void set(int64 i, T value)` |
| [std::linear_algebra::Vector::subtract](std-linear_algebra.md#api-22) | `Vector<T> subtract(Vector<T> other)` |
| [std::linear_algebra::Vector::toArray](std-linear_algebra.md#api-12) | `T[] toArray()` |
| [std::linear_algebra::Vector::zeros](std-linear_algebra.md#api-9) | `static Vector<T> zeros(int64 n)` |
| [std::linear_algebra::diagonalize2](std-linear_algebra.md#api-55) | `JordanDecomposition diagonalize2(Matrix<float64> input)` |
| [std::linear_algebra::jordan2](std-linear_algebra.md#api-54) | `JordanDecomposition jordan2(Matrix<float64> input)` |
| [std::linear_algebra::luDecompose](std-linear_algebra.md#api-50) | `LuFactorization luDecompose(Matrix<float64> input)` |
| [std::linear_algebra::lurDecompose](std-linear_algebra.md#api-51) | `LuFactorization lurDecompose(Matrix<float64> input)` |
| [std::linear_algebra::svd](std-linear_algebra.md#api-53) | `SingularValueDecomposition svd(Matrix<float64> input)` |
| [std::linear_algebra::symmetricEigen](std-linear_algebra.md#api-52) | `EigenDecomposition symmetricEigen(Matrix<float64> input)` |
| [std::linked_list::LinkedList::back](std-linked_list.md#api-15) | `Option<int64> back()` |
| [std::linked_list::LinkedList::clear](std-linked_list.md#api-19) | `void clear()` |
| [std::linked_list::LinkedList::contains](std-linked_list.md#api-13) | `bool contains(int64 value)` |
| [std::linked_list::LinkedList::copy](std-linked_list.md#api-1) | `LinkedList copy()` |
| [std::linked_list::LinkedList::create](std-linked_list.md#api-0) | `static LinkedList create()` |
| [std::linked_list::LinkedList::find](std-linked_list.md#api-12) | `int64 find(int64 value)` |
| [std::linked_list::LinkedList::fromArray](std-linked_list.md#api-11) | `static LinkedList fromArray(List<int64> items)` |
| [std::linked_list::LinkedList::front](std-linked_list.md#api-14) | `Option<int64> front()` |
| [std::linked_list::LinkedList::insertAfter](std-linked_list.md#api-5) | `int64 insertAfter(int64 cursor, int64 value)` |
| [std::linked_list::LinkedList::insertBefore](std-linked_list.md#api-4) | `int64 insertBefore(int64 cursor, int64 value)` |
| [std::linked_list::LinkedList::isEmpty](std-linked_list.md#api-17) | `bool isEmpty()` |
| [std::linked_list::LinkedList::iterator](std-linked_list.md#api-18) | `Option<int64> iterator(int64 index)` |
| [std::linked_list::LinkedList::len](std-linked_list.md#api-16) | `int64 len()` |
| [std::linked_list::LinkedList::popBack](std-linked_list.md#api-8) | `Option<int64> popBack()` |
| [std::linked_list::LinkedList::popFront](std-linked_list.md#api-7) | `Option<int64> popFront()` |
| [std::linked_list::LinkedList::pushBack](std-linked_list.md#api-3) | `int64 pushBack(int64 value)` |
| [std::linked_list::LinkedList::pushFront](std-linked_list.md#api-2) | `int64 pushFront(int64 value)` |
| [std::linked_list::LinkedList::removeAt](std-linked_list.md#api-6) | `Option<int64> removeAt(int64 cursor)` |
| [std::linked_list::LinkedList::reverse](std-linked_list.md#api-9) | `void reverse()` |
| [std::linked_list::LinkedList::toArray](std-linked_list.md#api-10) | `List<int64> toArray()` |
| [std::log::LogLevel::label](std-log.md#api-0) | `String label()` |
| [std::log::formatRecord](std-log.md#api-1) | `String formatRecord(LogLevel level, String message, Map<String,String> fields)` |
| [std::log::writeRecord](std-log.md#api-2) | `void writeRecord(LogLevel level, String message, Map<String,String> fields)` |
| [std::math::abs](std-math.md#api-19) | `T abs<T>(T value)` |
| [std::math::acos](std-math.md#api-5) | `let acos<T>(T value)` |
| [std::math::asin](std-math.md#api-4) | `let asin<T>(T value)` |
| [std::math::atan](std-math.md#api-6) | `let atan<T>(T value)` |
| [std::math::ceil](std-math.md#api-12) | `let ceil<T>(T value)` |
| [std::math::clamp](std-math.md#api-23) | `float64 clamp(float64 value, float64 low, float64 high)` |
| [std::math::clamp](std-math.md#api-22) | `int64 clamp(int64 value, int64 low, int64 high)` |
| [std::math::cos](std-math.md#api-2) | `let cos<T>(T value)` |
| [std::math::exp](std-math.md#api-7) | `let exp<T>(T value)` |
| [std::math::floor](std-math.md#api-11) | `let floor<T>(T value)` |
| [std::math::fmod](std-math.md#api-16) | `let fmod<T, U>(T value, U other)` |
| [std::math::isInf](std-math.md#api-18) | `bool isInf<T>(T value)` |
| [std::math::isNaN](std-math.md#api-17) | `bool isNaN<T>(T value)` |
| [std::math::ln](std-math.md#api-8) | `let ln<T>(T value)` |
| [std::math::log10](std-math.md#api-9) | `let log10<T>(T value)` |
| [std::math::max](std-math.md#api-21) | `T max<T>(T... values)` |
| [std::math::min](std-math.md#api-20) | `T min<T>(T... values)` |
| [std::math::pi](std-math.md#api-0) | `BigDecimal pi(int64 digits = 50)` |
| [std::math::pow](std-math.md#api-15) | `let pow<T, U>(T value, U other)` |
| [std::math::round](std-math.md#api-13) | `let round<T>(T value)` |
| [std::math::sin](std-math.md#api-1) | `let sin<T>(T value)` |
| [std::math::sqrt](std-math.md#api-10) | `let sqrt<T>(T value)` |
| [std::math::tan](std-math.md#api-3) | `let tan<T>(T value)` |
| [std::math::trunc](std-math.md#api-14) | `let trunc<T>(T value)` |
| [std::memory::Memory::stats](std-memory.md#api-4) | `static MemoryStats stats()` |
| [std::memory::Rc::clone](std-memory.md#api-6) | `Rc<T> clone()` |
| [std::memory::Rc::close](std-memory.md#api-9) | `void close()` |
| [std::memory::Rc::downgrade](std-memory.md#api-8) | `Weak<T> downgrade()` |
| [std::memory::Rc::get](std-memory.md#api-7) | `T get()` |
| [std::memory::Rc::isEmpty](std-memory.md#api-5) | `bool isEmpty()` |
| [std::memory::Span::from](std-memory.md#api-0) | `static Span<T> from(T mut& data, int64 length)` |
| [std::memory::Span::index](std-memory.md#api-1) | `T index(int64 key)` |
| [std::memory::Span::iterator](std-memory.md#api-2) | `Option<T mut&> iterator(int64 index)` |
| [std::memory::Span::len](std-memory.md#api-3) | `int64 len()` |
| [std::memory::Weak::close](std-memory.md#api-12) | `void close()` |
| [std::memory::Weak::isExpired](std-memory.md#api-10) | `bool isExpired()` |
| [std::memory::Weak::upgrade](std-memory.md#api-11) | `Rc<T> upgrade()` |
| [std::memory::rc](std-memory.md#api-13) | `Rc<T> rc<T>(T value)` |
| [std::message::receiveFor](std-message.md#api-4) | `Receive<T> receiveFor<T>(int64 millis)` |
| [std::message::receiveThread](std-message.md#api-1) | `T receiveThread<T>()` |
| [std::message::sendFor](std-message.md#api-2) | `int64 sendFor<T>(Thread thread, T value, int64 millis)` |
| [std::message::sendThread](std-message.md#api-0) | `void sendThread<T>(Thread thread, T value)` |
| [std::message::tryReceive](std-message.md#api-5) | `Receive<T> tryReceive<T>()` |
| [std::message::trySend](std-message.md#api-3) | `int64 trySend<T>(Thread thread, T value)` |
| [std::net::HttpClient::close](std-net.md#api-34) | `void close()` |
| [std::net::HttpClient::connect](std-net.md#api-31) | `bool connect(String host, int32 port, int32 timeoutMs)` |
| [std::net::HttpClient::exchange](std-net.md#api-33) | `HttpResponse exchange(String method, String url, String body, String contentType)` |
| [std::net::HttpClient::fetch](std-net.md#api-35) | `static HttpResponse fetch(String url)` |
| [std::net::HttpClient::fetchWithTimeout](std-net.md#api-36) | `static HttpResponse fetchWithTimeout(String url, int32 timeoutMs)` |
| [std::net::HttpClient::readAll](std-net.md#api-32) | `String readAll()` |
| [std::net::HttpClient::submit](std-net.md#api-37) | `static HttpResponse submit(String url, String body, String contentType)` |
| [std::net::HttpClient::submitWithTimeout](std-net.md#api-38) | `static HttpResponse submitWithTimeout(String url, String body, String contentType, int32 timeoutMs)` |
| [std::net::HttpResponse::header](std-net.md#api-26) | `String header(String name)` |
| [std::net::HttpResponse::isSuccess](std-net.md#api-25) | `bool isSuccess()` |
| [std::net::TcpListener::accept](std-net.md#api-22) | `TcpSocket accept()` |
| [std::net::TcpListener::accept](std-net.md#api-21) | `TcpSocket accept(int32 timeoutMs)` |
| [std::net::TcpListener::acceptAsync](std-net.md#api-23) | `TcpSocket acceptAsync()` |
| [std::net::TcpListener::close](std-net.md#api-24) | `void close()` |
| [std::net::TcpListener::isOpen](std-net.md#api-18) | `bool isOpen()` |
| [std::net::TcpListener::lastError](std-net.md#api-19) | `String lastError()` |
| [std::net::TcpListener::listen](std-net.md#api-17) | `static TcpListener listen(String host, int32 port)` |
| [std::net::TcpListener::listen](std-net.md#api-16) | `static TcpListener listen(String host, int32 port, int32 backlog)` |
| [std::net::TcpListener::localPort](std-net.md#api-20) | `int64 localPort()` |
| [std::net::TcpSocket::close](std-net.md#api-12) | `void close()` |
| [std::net::TcpSocket::connect](std-net.md#api-1) | `static TcpSocket connect(String host, int32 port)` |
| [std::net::TcpSocket::connect](std-net.md#api-0) | `static TcpSocket connect(String host, int32 port, int32 timeoutMs)` |
| [std::net::TcpSocket::connectAsync](std-net.md#api-3) | `static TcpSocket connectAsync(String host, int32 port)` |
| [std::net::TcpSocket::connectAsync](std-net.md#api-2) | `static TcpSocket connectAsync(String host, int32 port, int32 timeoutMs)` |
| [std::net::TcpSocket::isOpen](std-net.md#api-9) | `bool isOpen()` |
| [std::net::TcpSocket::lastError](std-net.md#api-10) | `String lastError()` |
| [std::net::TcpSocket::pollConnect](std-net.md#api-4) | `int64 pollConnect()` |
| [std::net::TcpSocket::receive](std-net.md#api-14) | `String receive(int32 maxBytes)` |
| [std::net::TcpSocket::receiveAsync](std-net.md#api-7) | `String receiveAsync(int32 maxBytes)` |
| [std::net::TcpSocket::receiveLine](std-net.md#api-15) | `String receiveLine(int32 timeoutMs)` |
| [std::net::TcpSocket::receiveLineAsync](std-net.md#api-8) | `String receiveLineAsync()` |
| [std::net::TcpSocket::send](std-net.md#api-13) | `int64 send(String data)` |
| [std::net::TcpSocket::sendAsync](std-net.md#api-6) | `int64 sendAsync(String data)` |
| [std::net::TcpSocket::sendAsync](std-net.md#api-5) | `int64 sendAsync(String data, int64 offset)` |
| [std::net::TcpSocket::status](std-net.md#api-11) | `int64 status()` |
| [std::net::buildRequest](std-net.md#api-29) | `String buildRequest(String method, String url, String body, String contentType)` |
| [std::net::httpScheme](std-net.md#api-27) | `String httpScheme()` |
| [std::net::parseResponse](std-net.md#api-30) | `HttpResponse parseResponse(String raw)` |
| [std::net::parseUrl](std-net.md#api-28) | `Url parseUrl(String url)` |
| [std::number_theory::addMod](std-number_theory.md#api-2) | `T addMod<T>(T left, T right, T modulus)` |
| [std::number_theory::gcd](std-number_theory.md#api-0) | `T gcd<T>(T... values)` |
| [std::number_theory::isPrime](std-number_theory.md#api-6) | `bool isPrime<T>(T value)` |
| [std::number_theory::lcm](std-number_theory.md#api-1) | `T lcm<T>(T... values)` |
| [std::number_theory::modInverse](std-number_theory.md#api-5) | `T modInverse<T>(T value, T modulus)` |
| [std::number_theory::mulMod](std-number_theory.md#api-3) | `T mulMod<T>(T left, T right, T modulus)` |
| [std::number_theory::powMod](std-number_theory.md#api-4) | `T powMod<T>(T base, T exponent, T modulus)` |
| [std::numerics::Numerics::bisect](std-numerics.md#api-2) | `static float64 bisect(float64 fn(float64) function, float64 left, float64 right, float64 tolerance, int64 maxIterations)` |
| [std::numerics::Numerics::derivative](std-numerics.md#api-0) | `static float64 derivative(float64 fn(float64) function, float64 x, float64 step)` |
| [std::numerics::Numerics::integrateSimpson](std-numerics.md#api-1) | `static float64 integrateSimpson(float64 fn(float64) function, float64 begin, float64 end, int64 intervals)` |
| [std::numerics::Numerics::newton](std-numerics.md#api-4) | `static float64 newton(float64 fn(float64) function, float64 initial, float64 tolerance, int64 maxIterations)` |
| [std::numerics::Numerics::rk4Step](std-numerics.md#api-3) | `static float64 rk4Step(float64 fn(float64, float64) derivative, float64 time, float64 value, float64 step)` |
| [std::numerics::Numerics::solveOdeEuler](std-numerics.md#api-5) | `static List<float64> solveOdeEuler( float64 fn(float64, float64) rate, float64 initialTime, float64 initialValue, float64 step, int64 steps)` |
| [std::numerics::Numerics::solveOdeRK4](std-numerics.md#api-6) | `static List<float64> solveOdeRK4( float64 fn(float64, float64) rate, float64 initialTime, float64 initialValue, float64 step, int64 steps)` |
| [std::parse::Parse::parseBool](std-parse.md#api-4) | `static Result<bool, Error> parseBool(String text)` |
| [std::parse::Parse::parseFloat64](std-parse.md#api-3) | `static Result<float64, Error> parseFloat64(String text)` |
| [std::parse::Parse::parseInt64](std-parse.md#api-1) | `static Result<int64, Error> parseInt64(String text)` |
| [std::parse::Parse::parseUInt64](std-parse.md#api-2) | `static Result<uint64, Error> parseUInt64(String text)` |
| [std::parse::ParseError::toString](std-parse.md#api-0) | `String toString()` |
| [std::path::PathString::extension](std-path.md#api-2) | `static String extension(String path)` |
| [std::path::PathString::fileName](std-path.md#api-1) | `static String fileName(String path)` |
| [std::path::PathString::join](std-path.md#api-0) | `static String join(String left, String right)` |
| [std::path::PathString::normalize](std-path.md#api-3) | `static String normalize(String path)` |
| [std::path::PathString::parent](std-path.md#api-4) | `static String parent(String path)` |
| [std::path::PathString::stem](std-path.md#api-5) | `static String stem(String path)` |
| [std::probe::twice](std-probe.md#api-0) | `int64 twice(int64 x)` |
| [std::process::processClose](std-process.md#api-2) | `int64 processClose(Process const& process)` |
| [std::process::processRead](std-process.md#api-1) | `String processRead(Process const& process)` |
| [std::process::processRun](std-process.md#api-0) | `Process processRun(String command)` |
| [std::process::system](std-process.md#api-3) | `int64 system(String command)` |
| [std::random::Random::fromEntropy](std-random.md#api-9) | `static Random fromEntropy()` |
| [std::random::Random::nextBool](std-random.md#api-14) | `bool nextBool()` |
| [std::random::Random::nextFloat64](std-random.md#api-12) | `float64 nextFloat64()` |
| [std::random::Random::nextInt](std-random.md#api-11) | `int64 nextInt(int64 minimum, int64 maximum)` |
| [std::random::Random::nextRange](std-random.md#api-13) | `float64 nextRange(float64 minimum, float64 maximum)` |
| [std::random::Random::nextUInt64](std-random.md#api-10) | `uint64 nextUInt64()` |
| [std::random::Random::withSeed](std-random.md#api-8) | `static Random withSeed(uint64 seed)` |
| [std::random::randomDrop](std-random.md#api-7) | `void randomDrop(uint64 handle)` |
| [std::random::randomFromEntropy](std-random.md#api-1) | `Random randomFromEntropy()` |
| [std::random::randomNextBool](std-random.md#api-6) | `bool randomNextBool(uint64 handle)` |
| [std::random::randomNextFloat64](std-random.md#api-4) | `float64 randomNextFloat64(uint64 handle)` |
| [std::random::randomNextInt](std-random.md#api-3) | `int64 randomNextInt(uint64 handle, int64 minimum, int64 maximum)` |
| [std::random::randomNextRange](std-random.md#api-5) | `float64 randomNextRange(uint64 handle, float64 minimum, float64 maximum)` |
| [std::random::randomNextUInt64](std-random.md#api-2) | `uint64 randomNextUInt64(uint64 handle)` |
| [std::random::randomWithSeed](std-random.md#api-0) | `Random randomWithSeed(uint64 seed)` |
| [std::reflect::ReflectError::toString](std-reflect.md#api-1) | `String toString()` |
| [std::reflect::ReflectValue::asBool](std-reflect.md#api-3) | `Result<bool,Error> asBool()` |
| [std::reflect::ReflectValue::asChar](std-reflect.md#api-4) | `Result<char,Error> asChar()` |
| [std::reflect::ReflectValue::asF32](std-reflect.md#api-13) | `Result<float32,Error> asF32()` |
| [std::reflect::ReflectValue::asF64](std-reflect.md#api-14) | `Result<float64,Error> asF64()` |
| [std::reflect::ReflectValue::asI16](std-reflect.md#api-6) | `Result<int16,Error> asI16()` |
| [std::reflect::ReflectValue::asI32](std-reflect.md#api-7) | `Result<int32,Error> asI32()` |
| [std::reflect::ReflectValue::asI64](std-reflect.md#api-8) | `Result<int64,Error> asI64()` |
| [std::reflect::ReflectValue::asI8](std-reflect.md#api-5) | `Result<int8,Error> asI8()` |
| [std::reflect::ReflectValue::asText](std-reflect.md#api-15) | `Result<String,Error> asText()` |
| [std::reflect::ReflectValue::asU16](std-reflect.md#api-10) | `Result<uint16,Error> asU16()` |
| [std::reflect::ReflectValue::asU32](std-reflect.md#api-11) | `Result<uint32,Error> asU32()` |
| [std::reflect::ReflectValue::asU64](std-reflect.md#api-12) | `Result<uint64,Error> asU64()` |
| [std::reflect::ReflectValue::asU8](std-reflect.md#api-9) | `Result<uint8,Error> asU8()` |
| [std::reflect::ReflectValue::typeId](std-reflect.md#api-2) | `uint64 typeId()` |
| [std::reflect::call](std-reflect.md#api-29) | `Result<std::reflect::ReflectValue,Error> call(String name, List<std::reflect::ReflectValue> args)` |
| [std::reflect::callEnumMethod](std-reflect.md#api-33) | `Result<std::reflect::ReflectValue,Error> callEnumMethod<T>(T mut& obj, String name, List<std::reflect::ReflectValue> args)` |
| [std::reflect::callEnumMethodConst](std-reflect.md#api-34) | `Result<std::reflect::ReflectValue,Error> callEnumMethodConst<T>(T const& obj, String name, List<std::reflect::ReflectValue> args)` |
| [std::reflect::callEnumStatic](std-reflect.md#api-35) | `Result<std::reflect::ReflectValue,Error> callEnumStatic<T>(String name, List<std::reflect::ReflectValue> args)` |
| [std::reflect::callInterface](std-reflect.md#api-39) | `Result<std::reflect::ReflectValue,Error> callInterface<I>(I mut& obj, String name, List<std::reflect::ReflectValue> args)` |
| [std::reflect::callInterfaceConst](std-reflect.md#api-40) | `Result<std::reflect::ReflectValue,Error> callInterfaceConst<I>(I const& obj, String name, List<std::reflect::ReflectValue> args)` |
| [std::reflect::callMethod](std-reflect.md#api-30) | `Result<std::reflect::ReflectValue,Error> callMethod<T>(T mut& obj, String name, List<std::reflect::ReflectValue> args)` |
| [std::reflect::callMethodConst](std-reflect.md#api-31) | `Result<std::reflect::ReflectValue,Error> callMethodConst<T>(T const& obj, String name, List<std::reflect::ReflectValue> args)` |
| [std::reflect::callStatic](std-reflect.md#api-32) | `Result<std::reflect::ReflectValue,Error> callStatic<T>(String name, List<std::reflect::ReflectValue> args)` |
| [std::reflect::construct](std-reflect.md#api-36) | `Result<T,Error> construct<T>(List<std::reflect::ReflectValue> args)` |
| [std::reflect::getAnnotationInfo](std-reflect.md#api-17) | `Option<std::reflect::AnnotationInfo> getAnnotationInfo(String name)` |
| [std::reflect::getAnnotations](std-reflect.md#api-24) | `List<std::reflect::Annotation> getAnnotations<T>()` |
| [std::reflect::getEnumInfo](std-reflect.md#api-21) | `Option<std::reflect::EnumInfo> getEnumInfo<T>()` |
| [std::reflect::getField](std-reflect.md#api-37) | `Result<F,Error> getField<T,F>(T const& obj, String name)` |
| [std::reflect::getFieldAnnotations](std-reflect.md#api-25) | `List<std::reflect::Annotation> getFieldAnnotations<T>(String name)` |
| [std::reflect::getFuncAnnotations](std-reflect.md#api-26) | `List<std::reflect::Annotation> getFuncAnnotations(String name)` |
| [std::reflect::getFuncInfo](std-reflect.md#api-16) | `Option<std::reflect::FuncInfo> getFuncInfo(String name)` |
| [std::reflect::getInterfaceId](std-reflect.md#api-19) | `uint64 getInterfaceId<I>()` |
| [std::reflect::getInterfaceInfo](std-reflect.md#api-22) | `Option<std::reflect::InterfaceInfo> getInterfaceInfo<I>()` |
| [std::reflect::getMethodAnnotations](std-reflect.md#api-27) | `List<std::reflect::Annotation> getMethodAnnotations<T>(String name)` |
| [std::reflect::getStaticMemberAnnotations](std-reflect.md#api-28) | `List<std::reflect::Annotation> getStaticMemberAnnotations<T>(String name)` |
| [std::reflect::getStructInfo](std-reflect.md#api-20) | `Option<std::reflect::StructInfo> getStructInfo<T>()` |
| [std::reflect::getTypeId](std-reflect.md#api-18) | `uint64 getTypeId<T>()` |
| [std::reflect::implementsInterface](std-reflect.md#api-23) | `bool implementsInterface<T>(uint64 interfaceId)` |
| [std::reflect::requireReflection](std-reflect.md#api-0) | `void requireReflection()` |
| [std::reflect::setField](std-reflect.md#api-38) | `Result<bool,Error> setField<T,F>(T mut& obj, String name, F value)` |
| [std::regex::Regex::capture](std-regex.md#api-5) | `String capture(String text, int64 group)` |
| [std::regex::Regex::compile](std-regex.md#api-1) | `static Result<Regex, Error> compile(String pattern)` |
| [std::regex::Regex::contains](std-regex.md#api-3) | `bool contains(String text)` |
| [std::regex::Regex::find](std-regex.md#api-4) | `Option<int64> find(String text)` |
| [std::regex::Regex::fullMatch](std-regex.md#api-2) | `bool fullMatch(String text)` |
| [std::regex::Regex::replaceAll](std-regex.md#api-7) | `String replaceAll(String text, String replacement)` |
| [std::regex::Regex::replaceFirst](std-regex.md#api-6) | `String replaceFirst(String text, String replacement)` |
| [std::regex::RegexError::toString](std-regex.md#api-0) | `String toString()` |
| [std::serialization::deserialize](std-serialization.md#api-1) | `Result<T,Error> deserialize<T>(List<uint8> bytes)` |
| [std::serialization::serialize](std-serialization.md#api-0) | `Result<List<uint8>,Error> serialize<T>(T const& value)` |
| [std::signal::dft](std-signal.md#api-4) | `List<Complex<float64>> dft(List<Complex<float64>> input)` |
| [std::signal::fft](std-signal.md#api-6) | `List<Complex<float64>> fft(List<Complex<float64>> input)` |
| [std::signal::idft](std-signal.md#api-5) | `List<Complex<float64>> idft(List<Complex<float64>> input)` |
| [std::signal::ifft](std-signal.md#api-7) | `List<Complex<float64>> ifft(List<Complex<float64>> input)` |
| [std::signal::signalDft](std-signal.md#api-0) | `List<Complex<float64>> signalDft(List<Complex<float64>> input)` |
| [std::signal::signalFft](std-signal.md#api-2) | `List<Complex<float64>> signalFft(List<Complex<float64>> input)` |
| [std::signal::signalIdft](std-signal.md#api-1) | `List<Complex<float64>> signalIdft(List<Complex<float64>> input)` |
| [std::signal::signalIfft](std-signal.md#api-3) | `List<Complex<float64>> signalIfft(List<Complex<float64>> input)` |
| [std::sql::DataCell::asInt](std-sql.md#api-7) | `int64 asInt()` |
| [std::sql::DataCell::asReal](std-sql.md#api-8) | `float64 asReal()` |
| [std::sql::DataCell::isNull](std-sql.md#api-4) | `bool isNull()` |
| [std::sql::DataCell::isNumber](std-sql.md#api-5) | `bool isNumber()` |
| [std::sql::DataCell::text](std-sql.md#api-6) | `String text()` |
| [std::sql::DataRow::at](std-sql.md#api-10) | `DataCell at(int64 index)` |
| [std::sql::DataRow::size](std-sql.md#api-9) | `int64 size()` |
| [std::sql::Database::affectedRows](std-sql.md#api-37) | `int64 affectedRows()` |
| [std::sql::Database::backendName](std-sql.md#api-31) | `String backendName()` |
| [std::sql::Database::close](std-sql.md#api-38) | `void close()` |
| [std::sql::Database::count](std-sql.md#api-43) | `int64 count(String sql)` |
| [std::sql::Database::driver](std-sql.md#api-29) | `Driver driver()` |
| [std::sql::Database::exec](std-sql.md#api-40) | `ResultSet exec(SqlStatement statement)` |
| [std::sql::Database::exec](std-sql.md#api-39) | `ResultSet exec(String sql)` |
| [std::sql::Database::execOk](std-sql.md#api-41) | `bool execOk(String sql)` |
| [std::sql::Database::hasTable](std-sql.md#api-44) | `bool hasTable(String tableName)` |
| [std::sql::Database::isOpen](std-sql.md#api-35) | `bool isOpen()` |
| [std::sql::Database::lastError](std-sql.md#api-36) | `String lastError()` |
| [std::sql::Database::local](std-sql.md#api-34) | `static Database local(String database)` |
| [std::sql::Database::maria](std-sql.md#api-33) | `static Database maria(String host, String user, String password, String database, int32 port = (int32)3306)` |
| [std::sql::Database::mysql](std-sql.md#api-32) | `static Database mysql(String host, String user, String password, String database)` |
| [std::sql::Database::mysql](std-sql.md#api-24) | `static Database mysql(String host, String user, String password, String database, int32 port)` |
| [std::sql::Database::odbc](std-sql.md#api-27) | `static Database odbc(String options)` |
| [std::sql::Database::postgres](std-sql.md#api-26) | `static Database postgres(String options)` |
| [std::sql::Database::quote](std-sql.md#api-30) | `String quote(String text)` |
| [std::sql::Database::scalar](std-sql.md#api-42) | `String scalar(String sql)` |
| [std::sql::Database::sqlServer](std-sql.md#api-28) | `static Database sqlServer(String options)` |
| [std::sql::Database::sqlite](std-sql.md#api-25) | `static Database sqlite(String path)` |
| [std::sql::Query::build](std-sql.md#api-3) | `Result<SqlStatement,Error> build(String operation = "select", String values = "{}")` |
| [std::sql::Query::orderBy](std-sql.md#api-0) | `void orderBy(String field, bool descending = false)` |
| [std::sql::Query::page](std-sql.md#api-2) | `void page(int64 limit, int64 offset = 0)` |
| [std::sql::Query::select](std-sql.md#api-1) | `void select(String field)` |
| [std::sql::ResultSet::at](std-sql.md#api-16) | `DataCell at(int64 row, int64 column)` |
| [std::sql::ResultSet::colCount](std-sql.md#api-12) | `int64 colCount()` |
| [std::sql::ResultSet::columnIndex](std-sql.md#api-15) | `int64 columnIndex(String name)` |
| [std::sql::ResultSet::columnName](std-sql.md#api-14) | `String columnName(int64 index)` |
| [std::sql::ResultSet::count](std-sql.md#api-21) | `int64 count()` |
| [std::sql::ResultSet::intAt](std-sql.md#api-18) | `int64 intAt(int64 row, int64 column)` |
| [std::sql::ResultSet::isEmpty](std-sql.md#api-13) | `bool isEmpty()` |
| [std::sql::ResultSet::pad](std-sql.md#api-22) | `String pad(String text, int64 width)` |
| [std::sql::ResultSet::realAt](std-sql.md#api-19) | `float64 realAt(int64 row, int64 column)` |
| [std::sql::ResultSet::render](std-sql.md#api-23) | `String render()` |
| [std::sql::ResultSet::rowCount](std-sql.md#api-11) | `int64 rowCount()` |
| [std::sql::ResultSet::scalar](std-sql.md#api-20) | `String scalar()` |
| [std::sql::ResultSet::textAt](std-sql.md#api-17) | `String textAt(int64 row, int64 column)` |
| [std::sql::displayWidth](std-sql.md#api-49) | `int64 displayWidth(String text)` |
| [std::sql::filter](std-sql.md#api-51) | `Result<bool,Error> filter<V>(Query mut& query, String field, V value, int64 operation = 0)` |
| [std::sql::filterMany](std-sql.md#api-52) | `Result<bool,Error> filterMany<V>(Query mut& query, String field, List<V> values, int64 operation = 11)` |
| [std::sql::insert](std-sql.md#api-53) | `Result<SqlStatement,Error> insert<T>(Query mut& query, T const& value)` |
| [std::sql::intLiteral](std-sql.md#api-47) | `String intLiteral(int64 value)` |
| [std::sql::query](std-sql.md#api-50) | `Result<Query,Error> query<T>()` |
| [std::sql::quote](std-sql.md#api-46) | `String quote(String text)` |
| [std::sql::quote](std-sql.md#api-45) | `String quote(String text, bool escapeBackslash)` |
| [std::sql::realLiteral](std-sql.md#api-48) | `String realLiteral(float64 value)` |
| [std::sql::update](std-sql.md#api-54) | `Result<SqlStatement,Error> update<T>(Query mut& query, T const& value)` |
| [std::text::StringBuilder::append](std-text.md#api-29) | `void append(String text)` |
| [std::text::StringBuilder::appendBool](std-text.md#api-34) | `void appendBool(bool value)` |
| [std::text::StringBuilder::appendChar](std-text.md#api-31) | `void appendChar(char value)` |
| [std::text::StringBuilder::appendFloat](std-text.md#api-33) | `void appendFloat(float64 value)` |
| [std::text::StringBuilder::appendInt](std-text.md#api-32) | `void appendInt(int64 value)` |
| [std::text::StringBuilder::appendLine](std-text.md#api-30) | `void appendLine(String text)` |
| [std::text::StringBuilder::clear](std-text.md#api-35) | `void clear()` |
| [std::text::StringBuilder::create](std-text.md#api-26) | `static StringBuilder create()` |
| [std::text::StringBuilder::drop](std-text.md#api-25) | `void drop()` |
| [std::text::StringBuilder::isEmpty](std-text.md#api-28) | `bool isEmpty()` |
| [std::text::StringBuilder::len](std-text.md#api-27) | `int64 len()` |
| [std::text::StringBuilder::toString](std-text.md#api-36) | `String toString()` |
| [std::text::Text::charAt](std-text.md#api-19) | `static char charAt(String text, int64 index)` |
| [std::text::Text::compare](std-text.md#api-9) | `static int64 compare(String left, String right)` |
| [std::text::Text::contains](std-text.md#api-2) | `static bool contains(String text, String needle)` |
| [std::text::Text::countMatches](std-text.md#api-8) | `static int64 countMatches(String text, String needle)` |
| [std::text::Text::endsWith](std-text.md#api-4) | `static bool endsWith(String text, String suffix)` |
| [std::text::Text::find](std-text.md#api-0) | `static int64 find(String text, String needle)` |
| [std::text::Text::isBlank](std-text.md#api-7) | `static bool isBlank(String text)` |
| [std::text::Text::join](std-text.md#api-21) | `static String join(List<String> parts, String separator)` |
| [std::text::Text::kmpFailureTable](std-text.md#api-24) | `static List<int64> kmpFailureTable(String needle)` |
| [std::text::Text::kmpFind](std-text.md#api-22) | `static int64 kmpFind(String text, String needle)` |
| [std::text::Text::kmpFindAll](std-text.md#api-23) | `static List<int64> kmpFindAll(String text, String needle)` |
| [std::text::Text::ltrim](std-text.md#api-13) | `static String ltrim(String text)` |
| [std::text::Text::padEnd](std-text.md#api-18) | `static String padEnd(String text, int64 width, char fill)` |
| [std::text::Text::padStart](std-text.md#api-17) | `static String padStart(String text, int64 width, char fill)` |
| [std::text::Text::repeat](std-text.md#api-16) | `static String repeat(String text, int64 count)` |
| [std::text::Text::replace](std-text.md#api-15) | `static String replace(String text, String from, String to)` |
| [std::text::Text::rfind](std-text.md#api-1) | `static int64 rfind(String text, String needle)` |
| [std::text::Text::rtrim](std-text.md#api-14) | `static String rtrim(String text)` |
| [std::text::Text::split](std-text.md#api-20) | `static List<String> split(String text, String separator)` |
| [std::text::Text::startsWith](std-text.md#api-3) | `static bool startsWith(String text, String prefix)` |
| [std::text::Text::stripPrefix](std-text.md#api-5) | `static String stripPrefix(String text, String prefix)` |
| [std::text::Text::stripSuffix](std-text.md#api-6) | `static String stripSuffix(String text, String suffix)` |
| [std::text::Text::toLower](std-text.md#api-11) | `static String toLower(String text)` |
| [std::text::Text::toUpper](std-text.md#api-10) | `static String toUpper(String text)` |
| [std::text::Text::trim](std-text.md#api-12) | `static String trim(String text)` |
| [std::thread::ThreadGroup::add](std-thread.md#api-21) | `void add(Thread task)` |
| [std::thread::ThreadGroup::cancelAll](std-thread.md#api-22) | `void cancelAll()` |
| [std::thread::ThreadGroup::create](std-thread.md#api-19) | `static ThreadGroup create()` |
| [std::thread::ThreadGroup::drop](std-thread.md#api-24) | `void drop()` |
| [std::thread::ThreadGroup::start](std-thread.md#api-20) | `Thread start(void fn() task)` |
| [std::thread::ThreadGroup::waitAll](std-thread.md#api-23) | `bool waitAll()` |
| [std::thread::ThreadPool::cancelAll](std-thread.md#api-29) | `void cancelAll()` |
| [std::thread::ThreadPool::closeAndWait](std-thread.md#api-28) | `bool closeAndWait()` |
| [std::thread::ThreadPool::create](std-thread.md#api-25) | `static ThreadPool create(int64 workers, int64 capacity)` |
| [std::thread::ThreadPool::drop](std-thread.md#api-30) | `void drop()` |
| [std::thread::ThreadPool::submit](std-thread.md#api-27) | `int64 submit(void fn() task)` |
| [std::thread::ThreadPool::submitFor](std-thread.md#api-26) | `int64 submitFor(void fn() task, int64 millis)` |
| [std::thread::ThreadResult::isReady](std-thread.md#api-9) | `bool isReady()` |
| [std::thread::closeInbox](std-thread.md#api-18) | `void closeInbox()` |
| [std::thread::create](std-thread.md#api-0) | `Thread create(void fn() task)` |
| [std::thread::createWith](std-thread.md#api-1) | `Thread createWith<T>(void fn(T) task, T value)` |
| [std::thread::current](std-thread.md#api-14) | `Thread current()` |
| [std::thread::parent](std-thread.md#api-15) | `Thread parent()` |
| [std::thread::receive](std-thread.md#api-13) | `String receive()` |
| [std::thread::receiveFor](std-thread.md#api-16) | `Receive<String> receiveFor(int64 millis)` |
| [std::thread::requestStop](std-thread.md#api-5) | `void requestStop(Thread thread)` |
| [std::thread::send](std-thread.md#api-12) | `void send(Thread thread, String message)` |
| [std::thread::sleepFor](std-thread.md#api-8) | `int64 sleepFor(int64 millis)` |
| [std::thread::state](std-thread.md#api-7) | `int64 state(Thread thread)` |
| [std::thread::stopRequested](std-thread.md#api-6) | `bool stopRequested()` |
| [std::thread::tryReceive](std-thread.md#api-17) | `Receive<String> tryReceive()` |
| [std::thread::tryWait](std-thread.md#api-4) | `bool tryWait(Thread thread)` |
| [std::thread::wait](std-thread.md#api-2) | `void wait(Thread thread)` |
| [std::thread::waitFor](std-thread.md#api-3) | `bool waitFor(Thread thread, int64 millis)` |
| [std::thread::waitResult](std-thread.md#api-11) | `ThreadResult waitResult(Thread thread)` |
| [std::thread::waitResultFor](std-thread.md#api-10) | `ThreadResult waitResultFor(Thread thread, int64 millis)` |
| [std::toml::TomlValue::asBool](std-toml.md#api-17) | `Option<bool> asBool()` |
| [std::toml::TomlValue::asBytes](std-toml.md#api-19) | `Option<List<uint8>> asBytes()` |
| [std::toml::TomlValue::asDateTime](std-toml.md#api-20) | `Option<String> asDateTime()` |
| [std::toml::TomlValue::asFloat64](std-toml.md#api-16) | `Option<float64> asFloat64()` |
| [std::toml::TomlValue::asInt](std-toml.md#api-15) | `Option<int64> asInt()` |
| [std::toml::TomlValue::asString](std-toml.md#api-18) | `Option<String> asString()` |
| [std::toml::TomlValue::at](std-toml.md#api-23) | `Option<TomlValue> at(int64 index)` |
| [std::toml::TomlValue::close](std-toml.md#api-3) | `bool close()` |
| [std::toml::TomlValue::dump](std-toml.md#api-27) | `String dump()` |
| [std::toml::TomlValue::find](std-toml.md#api-21) | `Option<TomlValue> find(String key)` |
| [std::toml::TomlValue::findBytes](std-toml.md#api-22) | `Option<TomlValue> findBytes(List<uint8> key)` |
| [std::toml::TomlValue::isArray](std-toml.md#api-10) | `bool isArray()` |
| [std::toml::TomlValue::isBool](std-toml.md#api-6) | `bool isBool()` |
| [std::toml::TomlValue::isDateTime](std-toml.md#api-13) | `bool isDateTime()` |
| [std::toml::TomlValue::isFloat](std-toml.md#api-8) | `bool isFloat()` |
| [std::toml::TomlValue::isInt](std-toml.md#api-7) | `bool isInt()` |
| [std::toml::TomlValue::isNull](std-toml.md#api-5) | `bool isNull()` |
| [std::toml::TomlValue::isObject](std-toml.md#api-11) | `bool isObject()` |
| [std::toml::TomlValue::isString](std-toml.md#api-9) | `bool isString()` |
| [std::toml::TomlValue::isTable](std-toml.md#api-12) | `bool isTable()` |
| [std::toml::TomlValue::isValid](std-toml.md#api-2) | `bool isValid()` |
| [std::toml::TomlValue::iterator](std-toml.md#api-24) | `Option<TomlValue> iterator(int64 index)` |
| [std::toml::TomlValue::keyAt](std-toml.md#api-25) | `Option<String> keyAt(int64 index)` |
| [std::toml::TomlValue::keyBytesAt](std-toml.md#api-26) | `Option<List<uint8>> keyBytesAt(int64 index)` |
| [std::toml::TomlValue::kind](std-toml.md#api-4) | `int64 kind()` |
| [std::toml::TomlValue::len](std-toml.md#api-14) | `int64 len()` |
| [std::toml::deserialize](std-toml.md#api-1) | `Result<T,Error> deserialize<T>(String text)` |
| [std::toml::parse](std-toml.md#api-28) | `Result<TomlValue, Error> parse(String text)` |
| [std::toml::serialize](std-toml.md#api-0) | `Result<String,Error> serialize<T>(T const& value)` |
| [std::tree::AvlTree::clear](std-tree.md#api-31) | `void clear()` |
| [std::tree::AvlTree::contains](std-tree.md#api-30) | `bool contains(int64 key)` |
| [std::tree::AvlTree::copy](std-tree.md#api-27) | `AvlTree copy()` |
| [std::tree::AvlTree::create](std-tree.md#api-26) | `static AvlTree create()` |
| [std::tree::AvlTree::height](std-tree.md#api-32) | `int64 height()` |
| [std::tree::AvlTree::inorder](std-tree.md#api-33) | `List<int64> inorder()` |
| [std::tree::AvlTree::insert](std-tree.md#api-41) | `bool insert(int64 key)` |
| [std::tree::AvlTree::isEmpty](std-tree.md#api-29) | `bool isEmpty()` |
| [std::tree::AvlTree::len](std-tree.md#api-28) | `int64 len()` |
| [std::tree::AvlTree::lowerBound](std-tree.md#api-37) | `Option<int64> lowerBound(int64 key)` |
| [std::tree::AvlTree::max](std-tree.md#api-36) | `Option<int64> max()` |
| [std::tree::AvlTree::min](std-tree.md#api-35) | `Option<int64> min()` |
| [std::tree::AvlTree::popMax](std-tree.md#api-40) | `Option<int64> popMax()` |
| [std::tree::AvlTree::popMin](std-tree.md#api-39) | `Option<int64> popMin()` |
| [std::tree::AvlTree::remove](std-tree.md#api-42) | `bool remove(int64 key)` |
| [std::tree::AvlTree::snapshot](std-tree.md#api-34) | `TreeSnapshot snapshot()` |
| [std::tree::AvlTree::upperBound](std-tree.md#api-38) | `Option<int64> upperBound(int64 key)` |
| [std::tree::BinarySearchTree::clear](std-tree.md#api-3) | `void clear()` |
| [std::tree::BinarySearchTree::contains](std-tree.md#api-6) | `bool contains(int64 key)` |
| [std::tree::BinarySearchTree::copy](std-tree.md#api-1) | `BinarySearchTree copy()` |
| [std::tree::BinarySearchTree::create](std-tree.md#api-0) | `static BinarySearchTree create()` |
| [std::tree::BinarySearchTree::height](std-tree.md#api-9) | `int64 height()` |
| [std::tree::BinarySearchTree::inorder](std-tree.md#api-10) | `List<int64> inorder()` |
| [std::tree::BinarySearchTree::insert](std-tree.md#api-4) | `bool insert(int64 key)` |
| [std::tree::BinarySearchTree::len](std-tree.md#api-2) | `int64 len()` |
| [std::tree::BinarySearchTree::max](std-tree.md#api-8) | `Option<int64> max()` |
| [std::tree::BinarySearchTree::min](std-tree.md#api-7) | `Option<int64> min()` |
| [std::tree::BinarySearchTree::postorder](std-tree.md#api-12) | `List<int64> postorder()` |
| [std::tree::BinarySearchTree::preorder](std-tree.md#api-11) | `List<int64> preorder()` |
| [std::tree::BinarySearchTree::remove](std-tree.md#api-5) | `bool remove(int64 key)` |
| [std::tree::BinarySearchTree::snapshot](std-tree.md#api-13) | `TreeSnapshot snapshot()` |
| [std::tree::GeneralTree::addChild](std-tree.md#api-57) | `int64 addChild(int64 cursor,int64 value)` |
| [std::tree::GeneralTree::addRoot](std-tree.md#api-56) | `int64 addRoot(int64 value)` |
| [std::tree::GeneralTree::children](std-tree.md#api-60) | `List<int64> children(int64 cursor)` |
| [std::tree::GeneralTree::clear](std-tree.md#api-55) | `void clear()` |
| [std::tree::GeneralTree::copy](std-tree.md#api-53) | `GeneralTree copy()` |
| [std::tree::GeneralTree::create](std-tree.md#api-52) | `static GeneralTree create()` |
| [std::tree::GeneralTree::fromBinary](std-tree.md#api-65) | `static GeneralTree fromBinary(BinarySearchTree tree)` |
| [std::tree::GeneralTree::height](std-tree.md#api-63) | `int64 height()` |
| [std::tree::GeneralTree::len](std-tree.md#api-54) | `int64 len()` |
| [std::tree::GeneralTree::levelOrder](std-tree.md#api-62) | `List<int64> levelOrder()` |
| [std::tree::GeneralTree::preorder](std-tree.md#api-61) | `List<int64> preorder()` |
| [std::tree::GeneralTree::rootNode](std-tree.md#api-59) | `int64 rootNode()` |
| [std::tree::GeneralTree::snapshot](std-tree.md#api-66) | `GeneralTreeSnapshot snapshot()` |
| [std::tree::GeneralTree::toBinary](std-tree.md#api-64) | `BinarySearchTree toBinary()` |
| [std::tree::GeneralTree::valueAt](std-tree.md#api-58) | `int64 valueAt(int64 cursor)` |
| [std::tree::MinHeap::clear](std-tree.md#api-47) | `void clear()` |
| [std::tree::MinHeap::copy](std-tree.md#api-44) | `MinHeap copy()` |
| [std::tree::MinHeap::create](std-tree.md#api-43) | `static MinHeap create()` |
| [std::tree::MinHeap::isEmpty](std-tree.md#api-46) | `bool isEmpty()` |
| [std::tree::MinHeap::len](std-tree.md#api-45) | `int64 len()` |
| [std::tree::MinHeap::peek](std-tree.md#api-49) | `Option<int64> peek()` |
| [std::tree::MinHeap::pop](std-tree.md#api-50) | `Option<int64> pop()` |
| [std::tree::MinHeap::push](std-tree.md#api-48) | `void push(int64 value)` |
| [std::tree::MinHeap::snapshot](std-tree.md#api-51) | `List<int64> snapshot()` |
| [std::tree::RedBlackTree::clear](std-tree.md#api-18) | `void clear()` |
| [std::tree::RedBlackTree::contains](std-tree.md#api-17) | `bool contains(int64 key)` |
| [std::tree::RedBlackTree::copy](std-tree.md#api-15) | `RedBlackTree copy()` |
| [std::tree::RedBlackTree::create](std-tree.md#api-14) | `static RedBlackTree create()` |
| [std::tree::RedBlackTree::height](std-tree.md#api-19) | `int64 height()` |
| [std::tree::RedBlackTree::inorder](std-tree.md#api-20) | `List<int64> inorder()` |
| [std::tree::RedBlackTree::insert](std-tree.md#api-23) | `bool insert(int64 key)` |
| [std::tree::RedBlackTree::len](std-tree.md#api-16) | `int64 len()` |
| [std::tree::RedBlackTree::max](std-tree.md#api-22) | `Option<int64> max()` |
| [std::tree::RedBlackTree::min](std-tree.md#api-21) | `Option<int64> min()` |
| [std::tree::RedBlackTree::remove](std-tree.md#api-24) | `bool remove(int64 key)` |
| [std::tree::RedBlackTree::validate](std-tree.md#api-25) | `bool validate()` |
| [std::utf8::Utf8Error::toString](std-utf8.md#api-0) | `String toString()` |
| [std::utf8::codePointCount](std-utf8.md#api-6) | `Result<int64,Error> codePointCount(String text)` |
| [std::utf8::decodeUtf8](std-utf8.md#api-2) | `Result<List<uint32>,Error> decodeUtf8(String text)` |
| [std::utf8::decodeUtf8Bytes](std-utf8.md#api-1) | `Result<List<uint32>,Error> decodeUtf8Bytes(List<uint8> text)` |
| [std::utf8::encodeUtf8](std-utf8.md#api-5) | `Result<String,Error> encodeUtf8(List<uint32> points)` |
| [std::utf8::encodeUtf8Bytes](std-utf8.md#api-4) | `Result<List<uint8>,Error> encodeUtf8Bytes(List<uint32> points)` |
| [std::utf8::isValidUtf8](std-utf8.md#api-3) | `bool isValidUtf8(String text)` |
| [std::utf8::sliceCodePoints](std-utf8.md#api-7) | `Result<String,Error> sliceCodePoints(String text, int64 start, int64 count)` |
| [std::xml::XmlNode::attribute](std-xml.md#api-1) | `Option<String> attribute(String name)` |
| [std::xml::XmlNode::child](std-xml.md#api-3) | `Option<XmlNode> child(int64 index)` |
| [std::xml::XmlNode::childCount](std-xml.md#api-2) | `int64 childCount()` |
| [std::xml::XmlNode::name](std-xml.md#api-0) | `String name()` |
| [std::xml::XmlNode::text](std-xml.md#api-4) | `Option<String> text(int64 index)` |
| [std::xml::escapeAttribute](std-xml.md#api-7) | `String escapeAttribute(String value)` |
| [std::xml::escapeText](std-xml.md#api-6) | `String escapeText(String value)` |
| [std::xml::parse](std-xml.md#api-5) | `Result<XmlNode,Error> parse(String text)` |
