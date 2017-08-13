(use-modules (ice-9 threads))
(use-modules (ice-9 match))
(use-modules (srfi srfi-1))  ; to use iota for ranges
(use-modules (ice-9 poe))

(defmacro ~> (head . body)
  (match body
         ('() head)

         (((f . args))        `(~> (,f ,head . ,args)))
         (((f . args) . rest) `(~> (,f ,head . ,args) . ,rest))

         ((f)        `(~> (,f ,head)))
         ((f . rest) `(~> (,f ,head) . ,rest))

         ))


(define root-shape '(polygon 4))
(define mutex      (make-mutex))


(define (nth n)  (map (lambda (x) (/ x n))
                      (iota n)))


(define (get-root-shape) (with-mutex mutex (eval root-shape
                                                 (interaction-environment))))

(define-macro (draw body) (with-mutex mutex (set! root-shape body)) #t)

; Save the outputs of polygon in a table of roughly 24 elements
(define polygon (perfect-funcq 24 polygon))

(define group list)
