# Dynamic-RailWay-Algorithm
This program calculates the minimal price of a railway project by the given inputs.\n
The RailWay problem:
  In this problem we build a railway length of L. We also get the next parameters (price, length, left-type connection, rigth-type connection) for each part.
  The left-type and rigth-type connections defines to us which connection can be connected from left or right.
 The Algorithm:
  By recurtion, we define the function f(l,k) thats returns the minimal price for railway of length 'l' and right-type connetion 'k'.
  f(l,k) = { 0 if 'l' == 0 or min(Pi + f(l - Di, Si) if 'l' > 0) }
  ![image](https://user-images.githubusercontent.com/73054794/117088015-f2ac3a00-ad59-11eb-8d8d-61dbe0373d48.png)
The input for command line interface:
  First line: the wanted total length of the project.
  Second line: type of connections we have (singel charecter), splited by comma.
  Third line till the end:
  - Left type connection 
  - Rigth type connection
  - Length of the part
  - Price of the part
  Splited by commas.
 The output: File name rwp_output.out, with the minimal price.
