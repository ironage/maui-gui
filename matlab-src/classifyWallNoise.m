function newVert = classifyWallNoise(vert)
x = vert(:,1);
y = vert(:,2);

[indx,~] = kmeans(y,3,'distance','cosine','start','sample', 'emptyaction','singleton');
% [indx,~] = k_means(y,3);

while length(unique(indx))<3  
% i.e. while one of the clusters is empty
% Just make some outliers to be considered as new cluster points
y(1:5) = y(1:5)+100;
% y(end) = y(end)-100;
[indx,~] = kmeans(y,3,'distance','cosine','start','sample', 'emptyaction','singleton');
% [indx,~] = k_means(y,3);

end  

dominant_class = mode(indx);
x_new = x(indx == dominant_class);
y_new = y(indx == dominant_class);
newVert = zeros(length(x_new),2);
newVert(:,1) = x_new;
newVert(:,2) = y_new;
newVert = interpolateMetoSpecificWidth(newVert, 1, x);
end