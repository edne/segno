(define >~ transform)
(define &  group)
(define >+ combine)

(define (>* n . body)
  (define (aux n tr)
    (if (> n 1)
      (>+ tr
          (aux (1- n)
               tr))
      tr))
  (aux n (apply >+ body)))


(define move-x translate-x)
(define move-y translate-y)


(draw (& (>~ (polygon 6)
             (>* 3
                 (scale 1/2)
                 (move-x 1/2)
                 (rotate (nth 6)))
             (scale 3/2))

         (>~ (fill (polygon 6))

             (& (scale 1/3 1/12)

                (>+ (scale 1/9)
                    (move-y 1/9)
                    (move-y 1/3)
                    (rotate (nth 3)))

                (>+ (scale  1/6)
                    (move-y 1/3)
                    (rotate (nth 3))
                    (rotate 1/2)))

             (rotate 1/2))))
