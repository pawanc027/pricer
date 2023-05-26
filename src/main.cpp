#include <pricer.hpp>
using namespace marketdata;

int main(int argc, const char* argv[])
{
	if( argc == 1)
	{
		std::cerr<<"No arguments were passed."<<std::endl;
		return -1;
	}
	else if (argc != 2)
	{
		std::cerr<<"More than two arguments were provided."<<std::endl;
		return -1;
	}

	unsigned long target_size;
	target_size = std::strtoul(argv[1],NULL,0);

	if( target_size == 0 )
	{
		std::cerr<<"Target size was passed as zero , thereby ending the price analysis"<<std::endl;
		return -1;
	}

	pricer mypricer(target_size);
        mypricer.readInputData();

	return 0;
}
