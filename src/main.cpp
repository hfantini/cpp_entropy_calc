#include "main.h"
#define VERSION "1.0.0"

void printUsage()
{
	std::cout << "This program can calculate the entropy of a file or text.\n\n";
	std::cout << " USAGE: entropy [OPTION] PAYLOAD \n\n";
	std::cout << "\tOPTIONS:\n";
	std::cout << "\t-f: Parses the payload as a file\n\n";
	std::cout << "\tPAYLOAD: Text or path to a file\n";
}

void printVersion()
{
	std::cout << "Version: " << VERSION << " | By Henrique Fantini @ 2024";
}

void calculateFrequency(std::vector<char> payload, std::streamsize size, std::shared_ptr<std::vector<int>> freqVec)
{
	for (int j = 0; j < size; j++)
	{
		const unsigned char count = static_cast<unsigned char>(payload[j]);
		(*freqVec)[count]++;
	}
}

double calculateEntropy(std::streamsize size, const std::vector<int>& freqVec)
{
	double entropy = 0.;
	double calc;

	for (int count = 0; count < freqVec.size(); count++)
	{
		calc = static_cast<double>(freqVec[count]) / size;
		if (calc > 0) {
			entropy += calc * fabs(log2(calc));
		}
	}

	return entropy;
}

double calculateEntropyOfString(std::vector<char> payload)
{
	std::shared_ptr<std::vector<int>> freqVec = std::make_shared<std::vector<int>>(256);
	calculateFrequency(payload, payload.size(), freqVec);
	return calculateEntropy(payload.size(), *freqVec);
}

double calculateEntropyOfFile(std::string file)
{
	std::ifstream fileStream(file, std::ios::binary);

	if (!fileStream.is_open())
	{
		throw std::exception("Cannot open file");
	}

	std::shared_ptr<std::vector<int>> freqVec = std::make_shared<std::vector<int>>(256);
	std::vector<char> buffer(1024 * 16, 0);
	std::streamsize totalBytesRead = 0;

	std::fill(freqVec->begin(), freqVec->end(), 0);

	while (!fileStream.eof())
	{
		fileStream.read(buffer.data(), buffer.size());
		totalBytesRead += fileStream.gcount();
		calculateFrequency(buffer, fileStream.gcount(), freqVec);
	}

	fileStream.close();

	return calculateEntropy(totalBytesRead, *freqVec.get());
}

int main(int argc, const char *argv[])
{
	try
	{
		if (argc == 1)
		{
			throw std::exception("Invalid number of arguments, please use -h switch to obtain help.");
		}

		for (int count = 0; count < argc; count++)
		{
			const char *value = argv[count];
			if (std::strcmp(value, "-h") == 0)
			{
				printUsage();
				return 0;
			}
			else if (std::strcmp(value, "-v") == 0)
			{
				printVersion();
				return 0;
			}
		}

		// PAYLOAD

		std::string payload = std::string(argv[argc - 1]);

		if (payload.size() == 0)
		{
			throw std::exception("Invalid payload");
		}

		std::string type = argv[1];
		double entropy = -1;

		if (type == "-f")
		{
			// FILE MODE
			entropy = calculateEntropyOfFile(payload);
		}
		else
		{
			// TEXT MODE
			entropy = calculateEntropyOfString(std::vector(payload.begin(), payload.end()));
		}

		std::cout << std::fixed << std::setprecision(2);
		std::cout << entropy;
	}
	catch (std::exception &e)
	{
		std::cerr << e.what();
		return 1;
	}

	return 0;
}