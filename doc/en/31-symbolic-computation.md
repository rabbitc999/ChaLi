# Exact symbolic computation and high precision mathematics

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../31-symbolic-computation.md)
<!-- END LANGUAGE NAVIGATION -->

Import `std::cas` to use `Expr`. String input becomes an immutable, normalized AST. Integers and rational numbers use the project's existing big integer backend; CAS has no separate multiplication or division implementation. Decimal literals remain exact rational values. Arithmetic operators combine AST nodes directly, without conversion through binary floating point.

`Expr::parse`, `fromInteger` and `fromDecimal` construct expressions. `ast`, `kind`, `name`, `childCount`, `child` and `children` expose the normalized tree. Retained roots and children survive destruction of the original expression. Subtraction and division may appear as addition, multiplication and powers. Original whitespace and parentheses are not retained.

Transformations include simplification, expansion, factorization, combining rational expressions, differentiation, integration, substitution, limits and Taylor expansion. Derivative and Taylor orders range from 0 to 1000. An unsupported indefinite integral remains an `integrate` node; an unresolved definite integral reports an error. Equality compares normalized structures and does not prove arbitrary mathematical identities. `decimal(digits)` converts an exact numeric node to the shared decimal backend; substitute free variables first.

Generic `std::math` functions accept ordinary numeric values, `BigDecimal` and symbolic expressions. Integer inputs produce `float64`; `float32` preserves its return type. Big integer inputs convert to `BigDecimal`. Big decimal functions retain their precision; symbolic inputs produce function ASTs. Angles use radians.

High precision algorithms operate on decimal arithmetic: AGM for pi, series and repeated squaring for exponentials, square-root reduction and series for logarithms, and high precision period reduction and series for trigonometric functions. Guard digits precede final significant-digit rounding. `pi(digits)` accepts 1..100000 digits. Argument reduction has an additional intermediate precision budget and reports an error when exceeded. Big decimals cannot represent NaN or infinity; invalid real domains report diagnostics. Ordinary floating point retains host NaN and infinity behavior. Rounding uses ties away from zero; truncation goes toward zero; remainder has the dividend's sign.

Products of powers and natural logarithms use a direct integration rule. The exponent `n` must be independent of the integration variable. On intervals where the logarithm is defined, the antiderivative for `n != -1` is `x^(n+1)*log(x)/(n+1) - x^(n+1)/(n+1)^2`; for `n = -1`, it is `log(x)^2/2`. A symbolic parameter uses the generic formula under `n != -1`; a concrete exponent of `-1` takes the separate rule. The input alias `ln` is printed as `log`.

For example, `Expr::parse("x^2*ln(x)").integrate("x")` produces `1/3*x^3*log(x) - 1/9*x^3`, and `Expr::parse("ln(x)/x").integrate("x")` produces `1/2*log(x)^2`. The rule also covers constant coefficients, fractional and negative exponents, and matching linear bases `(a*x+b)^n*log(a*x+b)` with a nonzero constant slope. An exponent depending on `x`, as in `x^x`, does not use the constant-exponent power rule.

CAS calls serialize through a process-wide lock protecting its node references, symbols and pool. Expressions may be copied between threads, but CAS calls execute serially. Shared numeric buffers use the process allocator rather than the producing runtime's arena. `NO_CHALI_STD_CAS` removes the implementation. CAS requires the standard library and big integers; decimal conversion also requires the decimal backend.

Symbol interning, structural equality, structural hashing and exact rational ordering use independent C11 services. Interning preserves existing IDs and stable name addresses, with a C spin lock serializing table operations. Structural comparison borrows read-only C numeric views; temporary cross products retain the current allocation budget. C++ nodes adapt field access and error boundaries. High-level simplification, calculus, polynomial and equation rules retain their existing C++ implementations and share the C integer/decimal backend rather than maintaining separate multiplication or division kernels.

See the [CAS API](../library/std-cas.md), [complete example](../../example/samples/cas.chl), [generic math example](../../example/samples/math_generic.chl), and [Chinese algorithm notes](../31-symbolic-computation.md). `tests/cas_library.py` runs the CAS library tests. `tests/bigfloat_math.py` compares multiple precisions and edge cases with the independent mpmath implementation.
