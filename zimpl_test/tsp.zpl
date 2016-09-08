set V      := { 1 to 8 };
set E      := { read "csp10_2.txt" as "<1n 2n>" comment "#" };
param c[E] := read "csp10_2.txt" as "<1n 2n> 3n" comment "#";

var x[E] binary;

defset dminus(v) := {<i,v> in E};
defset dplus(v)  := {<v,j> in E};

minimize cost: sum<i,j> in E: c[i,j] * x[i,j];

subto fc:
    forall <v> in V - {1,8}:
    sum<i,v> in dminus(v): x[i,v] == sum<v,i> in dplus(v): x[v,i];

subto uf:
    sum<s,i> in dplus(1): x[s,i] == 1;
