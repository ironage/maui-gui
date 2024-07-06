function lin_idx = getAroundVert(vert, imgHeight, up, down)

x = vert(:,1);
y = vert(:,2);
x = round(x');
y = round(y');

% get a linear index to the vein points in the matrix
vein_idx = y + (x-1)*imgHeight;

% get a lower limit vector (lower in terms of linear indexing)
lower_limit = (y-1).*(y<=up) + up*(y>up);
lower_limit_IM = vein_idx - lower_limit;

% get an upper limit vector
upper_limit = (imgHeight-y).*( (imgHeight-y) < down ) + down*( (imgHeight-y) >= down );
upper_limit_IM = vein_idx + upper_limit;

% the length of each column vector obtained
vec_len = upper_limit_IM - lower_limit_IM + 1;

% The total number of elements obtained
num_vein_elem = sum(vec_len);

% Create another lin_idx concatenating the low_limit and upper_limit
lin_idx = ones(1,num_vein_elem);
temp = cumsum([1,vec_len],2);
lin_idx(temp(1:end-1)) = [lower_limit_IM(1), ...
    lower_limit_IM(2:end) - upper_limit_IM(1:end-1)];
lin_idx = cumsum(lin_idx);
end
% % The obtained columns
% vein_columns = IM(lin_idx); % will be a single vector
%
% % If all the columns are guaranteed to be the same length, i.e., it is
% % guarnteed that you will have enough points over and under the vein
% column_len = up + down + 1;
% len = length(x);
% vein_matrix = reshape(vein_columns, column_len,len);
% figure, imshow(vein_matrix)





