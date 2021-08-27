/*
 * Every function is a member of ElementData and can read-write any
 * data member
 */ 
#include <iostream>
#include <vector>
#include <string>

class ElementData {
public:
  ElementData() : m_w(0.), m_x(0.), m_y(0.), m_z(0.) { };
  ElementData(double w, double x, double y, double z) : m_w(w), m_x(x), m_y(y), m_z(z) { };

  void level1Function();
  void level2Function();

  double m_w;
  double m_x;
  double m_y;
  double m_z;

private:
  double m_dx_dt;

};

void ElementData::level1Function() {
  const double beta = 8./3.;
  const double rho = 28.;

  const double dt = 0.1;
  
  double dy_dt;
  double dz_dt;

  dy_dt = m_x * (rho - m_z) - m_y;
  dz_dt = m_x * m_y - beta * m_z;

  m_x += m_dx_dt * dt;
  m_y += dy_dt * dt;
  m_z += dz_dt * dt;
}  
  
void ElementData::level2Function() {
  const double sigma = 10.;
  
  m_dx_dt = sigma * (m_y - m_x);
}

void scalar( ) {
  ElementData e(2., 1., 1., 1.);
  std::cout << "x = " << e.m_x << ", y = " << e.m_y << ", z = " << e.m_z << std::endl;

  e.level1Function();

  std::cout << "Scalar run" << std::endl;
  std::cout << "x = " << e.m_x << ", y = " << e.m_y << ", z = " << e.m_z << std::endl;
}

void iterated_array(int nxyz, int timesteps) {
  double lorenz_range = 10.;
  double lorenz_start = -lorenz_range/2;
  double lorenz_step = lorenz_range/nxyz;
  
  std::vector<ElementData> modelData = std::vector<ElementData>(nxyz*nxyz*nxyz);

  // Fill the arrays
  for (int i = 0; i < nxyz; i++) {
    for (int j = 0; j < nxyz; j++) {
      for (int k = 0; k < nxyz; k++) {
	modelData[k + nxyz * (j + nxyz * i)] =
	  ElementData(
	   i * 10000. + j * 100. + k,
	   -lorenz_start + lorenz_step * i,
	   -lorenz_start + lorenz_step * j,
	   -lorenz_start + lorenz_step * k
	   );
      }
    }
  }

  // Loop over time and all elements
  for (int t = 0; t < timesteps; t++) {
    for (auto iter = modelData.begin(); iter != modelData.end(); iter++) {
	iter->level1Function();
    }
  }

  // Sum all the elements, so that the compiler cannot omptimize away the main loop as not doing anything
  double sumx = 0.;
  double sumy = 0.;
  double sumz = 0.;
  for (auto iter = modelData.begin(); iter != modelData.end(); iter++) {
    sumx += iter->m_x;
    sumy += iter->m_y;
    sumz += iter->m_z;
  }

  std::cout << "Variable totals: x = " << sumx << ", y = " << sumy << ", z = " << sumz << std::endl;

}

int main(int argc, char* argv[]) {

  scalar();

  int nxyz;
  int ntimesteps;
  if (argc > 1) {
    nxyz = std::stoi(std::string(argv[1]));
  } else {
    nxyz = 100;
  }

  if (argc > 2) {
    ntimesteps = std::stoi(std::string(argv[2]));
  } else {
    ntimesteps = 100;
  }

  iterated_array(nxyz, ntimesteps);
  
  return 0;
}
