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


(define root-shape (polygon 6))
(define mutex      (make-mutex))


(define (get-root-shape) (with-mutex mutex root-shape))

(define (draw . shapes)  (with-mutex mutex (set! root-shape shapes)))

(define (group . rest)   (apply list rest))

; Esoteric ASCII version
(define >_               draw)
(define (@  . rest)      (apply list rest))
(define (>> . rest)      (apply change rest))
