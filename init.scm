(use-modules (ice-9 threads))
(use-modules (ice-9 match))
(use-modules (srfi srfi-1))  ; to use iota for ranges

(defmacro ~> (head . body)
  (match body
         ('() head)

         (((f . args))        `(~> (,f ,head . ,args)))
         (((f . args) . rest) `(~> (,f ,head . ,args) . ,rest))

         ((f)        `(~> (,f ,head)))
         ((f . rest) `(~> (,f ,head) . ,rest))

         ))


(define root-shape (polygon 4))
(define mutex      (make-mutex))


(define (nth n)  (map (lambda (x) (/ x n))
                      (iota n)))


(define (get-root-shape) (with-mutex mutex root-shape))

(define (draw . shapes)  (with-mutex mutex (set! root-shape shapes)))

(define group list)

; Esoteric ASCII version
(define >_    draw)
(define @     group)
(define >>    change)
