public final class interface_mono {
    interface Operation { long apply(long value); }
    interface NamedOperation extends Operation { long bias(); }
    static final class Multiply implements NamedOperation {
        long factor, offset;
        public long apply(long value) { return (value * factor + offset) & 2147483647L; }
        public long bias() { return offset; }
    }
    static Multiply receiver;
    static void setup() {
        receiver = new Multiply(); receiver.factor = 3; receiver.offset = 7;
    }
    static long invoke(NamedOperation operation, long value) { return operation.apply(value); }
    static long work(long seed) {
        NamedOperation derived = receiver;
        long acc = seed;
        for (int i = 0; i < 262144; i++) acc = invoke(derived, acc);
        return acc;
    }
    public static void main(String[] args) { FeatureTiming.run(interface_mono::setup, interface_mono::work, args); }
}
