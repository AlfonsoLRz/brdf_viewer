#define SQUARE(x) (x * x)
#define ATAN2(x, y) (mix(PI / 2.0f - atan(x, y), atan(y, x), float(abs(x) > abs(y))))
#define VECTOR_H(v, l) (normalize(v + l))