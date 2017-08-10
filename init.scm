(use-modules (ice-9 threads))
(use-modules (ice-9 match))

(defmacro ~> (head . body)
  (match body
         ('() head)

         (((f . args))        `(~> (,f ,head . ,args)))
         (((f . args) . rest) `(~> (,f ,head . ,args) . ,rest))

         ((f)        `(~> (,f ,head)))
         ((f . rest) `(~> (,f ,head) . ,rest))

         ))


(define root-shape (polygon 6))
(define mutex      (make-mutex))


(define (get-root-shape) (with-mutex mutex root-shape))

(define (draw shape)     (with-mutex mutex (set! root-shape shape)))

(define >_               draw)
(define (@ . rest)       (apply list rest))
