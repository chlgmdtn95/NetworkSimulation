#include <iostream>
#include <random>
#	define N 10

int main() 
{
	int	success, collision,	no_trans, cnt;
	int nodes[N];

	std::cout << "Tx prob.    # Success    # Collision    # No TX" << std::endl;
	for (double p=1; p<=25; p++)
	{		
		success = 0;
		collision = 0;
		no_trans = 0;
		for (int t=0; t<1000000; t++)
		{
			cnt = 0;
			for (int i=0; i<N; i++)
				nodes[i] = 0;

			// for(int i=0; i<N; i++)
			// {
			// 	if ((1.0 * rand() / RAND_MAX) * 100 < p)
			// 	{
			// 		nodes[i] = 1;
			// 		cnt++;
			// 		if (cnt > 1)
			// 		{
			// 			collision++;
			// 			break;
			// 		}
			// 	}
			// 	else
			// 		nodes[i] = 0;
			// }
			for(int i=0; i<N; i++)
			{
				if ((1.0 * rand() / RAND_MAX) * 100 < p)
					nodes[i] = 1;
				else
					nodes[i] = 0;
			}

			cnt = 0;
			for (int i=0; i<N; i++)
			{
				if (nodes[i] == 1)
					cnt++;
			}
			if (cnt == 0)
				no_trans++;
			if (cnt == 1)
				success++;
			if (cnt > 1)
				collision++;
		}
		std::cout << std::fixed;
		std::cout.precision(6);
		double pp, s, c, n;
		pp = p / 100;
		s = (double)success / (double)1000000;
		c = (double)collision / (double)1000000;
		n = (double)no_trans / (double)1000000;
		std::cout << pp << "    " << s << "    " << c << "    " << n << std::endl;
	}
}