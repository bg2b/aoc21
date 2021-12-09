lines = load('input1');
if min(min(lines)) == 0
  lines = lines + 1;
end
numlines = size(lines, 1);
n = max(max(lines));
A = sparse(n, n);
for k = 1:numlines
  x1 = lines(k, 1);
  y1 = lines(k, 2);
  x2 = lines(k, 3);
  y2 = lines(k, 4);
  if y1 == y2
    % horizontal
    range = min(x1, x2):max(x1, x2);
    A(range, y1) = A(range, y1) + 1;
  elseif x1 == x2
    % vertical
    range = min(y1, y2):max(y1, y2);
    A(x1, range) = A(x1, range) + 1;
  end
end
nnz(A > 1)
