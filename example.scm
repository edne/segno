(define >~ transform)
(define &  group)
(define >+ combine)
;(define >. attribute)

(define move-x translate-x)
(define move-y translate-y)


(draw (>~ (polygon 6)

           (& (scale 1/12
                     2/3)

              (>+ (scale 1/9)
                  (move-y 1/9)
                  (move-y 1/3)
                  (rotate (nth 3)))

              (>+ (scale  1/6)
                  (move-y 1/3)
                  (rotate (nth 3))
                  (rotate 1/2)))

           (rotate 1/2)))
