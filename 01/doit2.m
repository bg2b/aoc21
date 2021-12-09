a = load('input');
windowed = a(1:end-2) + a(2:end-1) + a(3:end);
sum(windowed(2:end) > windowed(1:end-1))
