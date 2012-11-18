#include "kdtree.h"
#include <iostream>

using namespace KDTREE;
int main()
{
	KDtree kd (10, 10); // 30 vectors, dim=10

	Point p(10); p[0]=p[1]=p[2]=p[3]=p[4]=p[5]=p[6]=p[7]=p[8]=2; p[9]=1;

	std::cout << kd.data();

	kd.build();
	kd.depthFirstVisit(kd.getRoot());

	NNreturn nn;
	nn=kd.returnNearest(p);

	std::cout << "node=" << nn.get<0>() << " d=" << nn.get<1>() << " h=" << nn.get<2>() << "\n";

	std::string a;
	std::cin >> a;

}
