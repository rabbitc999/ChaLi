import java.lang.management.ManagementFactory;
import java.util.function.LongUnaryOperator;

/** 工作区初始化后，在每个独立 JVM 内完成至少 256 次且至少一秒的热身。 */
final class FeatureTiming {
    private static volatile long blackhole;
    static void run(Runnable setup, LongUnaryOperator kernel, String[] args) {
        setup.run();
        if (args.length > 0 && args[0].equals("--verify")) {
            for (int seed = 0; seed < 5; seed++) System.out.println("value " + seed + " " + kernel.applyAsLong(seed));
            return;
        }
        long start = System.nanoTime();
        int warmup = 0;
        do {
            blackhole = kernel.applyAsLong(warmup % 5);
            warmup++;
        } while (warmup < 256 || System.nanoTime() - start < 1_000_000_000L);
        System.out.println("warmup_calls " + warmup);
        System.out.println("warmup_ns " + (System.nanoTime() - start));
        System.out.println("warmup_complete_uptime_ms " + ManagementFactory.getRuntimeMXBean().getUptime());
        long sink = kernel.applyAsLong(0);
        for (int seed = 0; seed < 5; seed++) {
            long before = System.nanoTime();
            long value = kernel.applyAsLong(seed);
            long elapsed = System.nanoTime() - before;
            sink += value;
            System.out.println("rep " + elapsed);
        }
        blackhole = sink;
        System.out.println("sink " + sink);
    }
}
