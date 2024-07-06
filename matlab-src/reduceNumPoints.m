function vert_new = reduceNumPoints(vert)
[t1 t2] = unique(round(vert(:,1)));
 vert_new(:,1) = t1;
 vert_new(:,2) = vert(t2,2);
 end