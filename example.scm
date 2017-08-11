(define hexa (polygon 6))

(>_ (@ (~> hexa
           (scale 1/2))
       (~> hexa
           (scale 1/2)
           (rotate 1/4)
           (translate 2/3)
           (rotate (iota 6 1/4 1/3))
           (scale 3/9))))
