#include <armadillo>
#include <iostream>

using namespace std;
using namespace arma;

#include <armadillo>
#include <cmath>
#include <vector>

int main(int ac, char *av[]) {

  cout << "<Hello Armadillo!>\n";
  arma::mat A(4, 5, fill::randu);
  arma::mat B(4, 5, fill::randu);

  cout << A * B.t() << endl;

  return 0;
}