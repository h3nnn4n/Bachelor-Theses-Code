\ENCODING=ISO-8859-1
\Problem name: IloCplex

Minimize
 obj: 286 x0 + 300 x1 + 245 x2 + 287 x3 + 77 x4 + 309 x5 + 307 x6 + 427 x8
      + 311 x9 + 200 x10 + 301 x11 + 144 x12 + 410 x13 + 104 x16 + 279 x17
      + 265 x18 + 182 x19 + 218 x20 + 132 x21 + 294 x22 + 48 x24
Subject To
 c0:   x0 + x52 + x63 + x64 + x90  = 1
 c1:   x1 + x54  = 1
 c2:   x2 + x59 + x89  = 1
 c3:   x3 + x65  = 1
 c4:   x4 + x50 + x75  = 1
 c5:   x5 + x28 + x36 + x41  = 1
 c6:   x6 + x29 + x46 + x55  = 1
 c7:   x7 + x38 + x58  = 1
 c8:   x8 + x32 + x42 + x48 + x49  = 1
 c9:   x9 + x33 + x52 + x63  = 1
 c10:  x10 + x35 + x76 + x77  = 1
 c11:  x3 + x66 + x83 + x86  = 1
 c12:  x0 + x59 + x72  = 1
 c13:  x2 + x61 + x62 + x74 + x87  = 1
 c14:  x11 + x41 + x46  = 1
 c15:  x12 + x32 + x36 + x57  = 1
 c16:  x13 + x28 + x29 + x38 + x42 + x58  = 1
 c17:  x1 + x55 + x78  = 1
 c18:  x4 + x50 + x75  = 1
 c19:  x6 + x31  = 1
 c20:  x14 + x81 + x88  = 1
 c21:  x15 + x76 + x77  = 1
 c22:  x8 + x33 + x57 + x60  = 1
 c23:  x12 + x34 + x39 + x40 + x61 + x83  = 1
 c24:  x5 + x29 + x32 + x38 + x50  = 1
 c25:  x16 + x72  = 1
 c26:  x17 + x58 + x66  = 1
 c27:  x9 + x35 + x53 + x75 + x78  = 1
 c28:  x10 + x36 + x51 + x67 + x86  = 1
 c29:  x18 + x37 + x85  = 1
 c30:  x13 + x30 + x38 + x50 + x74 + x87  = 1
 c31:  x11 + x43 + x45 + x81 + x88  = 1
 c32:  x16  = 1
 c33:  x17 + x60 + x67  = 1
 c34:  x18 + x34 + x43 + x74 + x81  = 1
 c35:  x22 + x56 + x70  = 1
 c36:  x19 + x69 + x82  = 1
 c37:  x20 + x34 + x37  = 1
 c38:  x18 + x45 + x68 + x79 + x80  = 1
 c39:  x21 + x43 + x84  = 1
 c40:  x24 + x68 + x73  = 1
 c41:  x20 + x44  = 1
 c42:  x22 + x71  = 1
 c43:  x19 + x70  = 1
 c44:  x23  = 1
 c45:  x21 + x47 + x79 + x80  = 1
 c46:  x24 + x69 + x84  = 1
 c47:  x25  = 1
 c48:  x26  = 1
 c49:  x24 + x73 + x84  = 1
 c_nj: x0 + x1 + x2 + x3 + x4 + x5 + x6 + x8 + x9 + x10 + x11 + x12 + x13 + x16
       + x17 + x18 + x19 + x20 + x21 + x22 + x24 + x7 + x14 + x15 + x23 + x25
       + x26 + x28 + x29 + x30 + x31 + x32 + x33 + x34 + x35 + x36 + x37 + x38
       + x39 + x40 + x41 + x42 + x43 + x44 + x45 + x46 + x47 + x48 + x49 + x50
       + x51 + x52 + x53 + x54 + x55 + x56 + x57 + x58 + x59 + x60 + x61 + x62
       + x63 + x64 + x65 + x66 + x67 + x68 + x69 + x70 + x71 + x72 + x73 + x74
       + x75 + x76 + x77 + x78 + x79 + x80 + x81 + x82 + x83 + x84 + x85 + x86
       + x87 + x88 + x89 + x90  = 27
Bounds
 0 <= x0 <= 1
 0 <= x1 <= 1
 0 <= x2 <= 1
 0 <= x3 <= 1
 0 <= x4 <= 1
 0 <= x5 <= 1
 0 <= x6 <= 1
 0 <= x8 <= 1
 0 <= x9 <= 1
 0 <= x10 <= 1
 0 <= x11 <= 1
 0 <= x12 <= 1
 0 <= x13 <= 1
 0 <= x16 <= 1
 0 <= x17 <= 1
 0 <= x18 <= 1
 0 <= x19 <= 1
 0 <= x20 <= 1
 0 <= x21 <= 1
 0 <= x22 <= 1
 0 <= x24 <= 1
 0 <= x7 <= 1
 0 <= x14 <= 1
 0 <= x15 <= 1
 0 <= x23 <= 1
 0 <= x25 <= 1
 0 <= x26 <= 1
 0 <= x28 <= 1
 0 <= x29 <= 1
 0 <= x30 <= 1
 0 <= x31 <= 1
 0 <= x32 <= 1
 0 <= x33 <= 1
 0 <= x34 <= 1
 0 <= x35 <= 1
 0 <= x36 <= 1
 0 <= x37 <= 1
 0 <= x38 <= 1
 0 <= x39 <= 1
 0 <= x40 <= 1
 0 <= x41 <= 1
 0 <= x42 <= 1
 0 <= x43 <= 1
 0 <= x44 <= 1
 0 <= x45 <= 1
 0 <= x46 <= 1
 0 <= x47 <= 1
 0 <= x48 <= 1
 0 <= x49 <= 1
 0 <= x50 <= 1
 0 <= x51 <= 1
 0 <= x52 <= 1
 0 <= x53 <= 1
 0 <= x54 <= 1
 0 <= x55 <= 1
 0 <= x56 <= 1
 0 <= x57 <= 1
 0 <= x58 <= 1
 0 <= x59 <= 1
 0 <= x60 <= 1
 0 <= x61 <= 1
 0 <= x62 <= 1
 0 <= x63 <= 1
 0 <= x64 <= 1
 0 <= x65 <= 1
 0 <= x66 <= 1
 0 <= x67 <= 1
 0 <= x68 <= 1
 0 <= x69 <= 1
 0 <= x70 <= 1
 0 <= x71 <= 1
 0 <= x72 <= 1
 0 <= x73 <= 1
 0 <= x74 <= 1
 0 <= x75 <= 1
 0 <= x76 <= 1
 0 <= x77 <= 1
 0 <= x78 <= 1
 0 <= x79 <= 1
 0 <= x80 <= 1
 0 <= x81 <= 1
 0 <= x82 <= 1
 0 <= x83 <= 1
 0 <= x84 <= 1
 0 <= x85 <= 1
 0 <= x86 <= 1
 0 <= x87 <= 1
 0 <= x88 <= 1
 0 <= x89 <= 1
 0 <= x90 <= 1
End
