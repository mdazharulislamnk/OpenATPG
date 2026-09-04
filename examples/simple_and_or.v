module test(A, B, C, Y);
  input A, B, C;
  output Y;
  wire W1;
  and g1(W1, A, B);
  or g2(Y, W1, C);
endmodule
