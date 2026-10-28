public final class interface_poly {
    interface Operation { long apply(long value); }
    interface NamedOperation extends Operation { long bias(); }
    static final class Multiply implements NamedOperation {
        long factor, offset;
        public long apply(long value) { return (value * factor + offset) & 2147483647L; }
        public long bias() { return offset; }
    }
    static final class Subtract implements NamedOperation {
        long factor, offset;
        public long apply(long value) { return (value * factor - offset) & 2147483647L; }
        public long bias() { return offset; }
    }
    static final class ExclusiveOr implements NamedOperation {
        long factor, offset;
        public long apply(long value) { return ((value * factor) ^ offset) & 2147483647L; }
        public long bias() { return offset; }
    }
    static Multiply first;
    static Subtract second;
    static ExclusiveOr third;
    static void setup() {
        first = new Multiply(); first.factor = 3; first.offset = 7;
        second = new Subtract(); second.factor = 5; second.offset = 11;
        third = new ExclusiveOr(); third.factor = 7; third.offset = 13;
    }
    static NamedOperation choose(long index) {
        if (index == 1) return second;
        if (index == 2) return third;
        return first;
    }
    static long invoke(NamedOperation operation, long value) { return operation.apply(value); }
    static long work(long seed) {
        long acc = seed;
        for (int i = 0; i < 262144; i++) {
            NamedOperation derived = choose((i + seed) & 3);
            acc = invoke(derived, acc);
        }
        return acc;
    }
    public static void main(String[] args) { FeatureTiming.run(interface_poly::setup, interface_poly::work, args); }
}
