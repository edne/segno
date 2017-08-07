(use-modules (ice-9 threads))

(define root-shape (polygon 6))
(define mutex      (make-mutex))


(define (get-root-shape) (with-mutex mutex root-shape))

(define (draw shape)     (with-mutex mutex (set! root-shape shape)))
