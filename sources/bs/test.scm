(bs-builtin-block
    (bs-builtin-define a 123)
    (bs-builtin-define b 420)
    (bs-builtin-define (f a b c) a)

    (f 1 2 3)

    ((bs-builtin-lambda (a) a) 12)
)
