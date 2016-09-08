\ENCODING=ISO-8859-1
\Problem name: spp

Minimize
 obj: 5 x1 + 6 x2 + 3 x3 + 4 x4 + 8 x5 + 6 x6 + 4 x7 + 3 x8
Subject To
 c1: x1 + x2  = 1
 c2: x3 + x4 + x5 + x6  = 1
 c3: x1 + x3 + x5 + x7  = 1
 c4: x4 + x5 + x7 + x8  = 1
 c5: x2 + x6 + x8  = 1
Bounds
 0 <= x1 <= 1
 0 <= x2 <= 1
 0 <= x3 <= 1
 0 <= x4 <= 1
 0 <= x5 <= 1
 0 <= x6 <= 1
 0 <= x7 <= 1
 0 <= x8 <= 1
End
