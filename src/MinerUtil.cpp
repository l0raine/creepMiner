//  cryptoport.io Burst Pool Miner
//
//  Created by Uray Meiviar < uraymeiviar@gmail.com > 2014
//  donation :
//
//  [Burst  ] BURST-8E8K-WQ2F-ZDZ5-FQWHX
//  [Bitcoin] 1UrayjqRjSJjuouhJnkczy5AuMqJGRK4b

#include "MinerUtil.hpp"
#include <sstream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include "Declarations.hpp"
#include "MinerLogger.hpp"
#include <Poco/URI.h>
#include <Poco/Net/HTTPClientSession.h>
#include "Deadline.hpp"
#include <Poco/JSON/Object.h>
#include "MinerConfig.hpp"
#include "MinerData.hpp"
#include "PlotReader.hpp"
#include <Poco/JSON/Parser.h>

bool Burst::isNumberStr(const std::string& str)
{
	return std::all_of(str.begin(), str.end(), ::isdigit);
}

std::string Burst::getFileNameFromPath(const std::string& strPath)
{
	size_t iLastSeparator;
	return strPath.substr((iLastSeparator = strPath.find_last_of("/\\")) !=
						  std::string::npos ? iLastSeparator + 1 : 0, strPath.size() - strPath.find_last_of("."));
}

std::vector<std::string> Burst::splitStr(const std::string& s, char delim)
{
	std::vector<std::string> elems;
	splitStr(s, delim, elems);
	return elems;
}

std::vector<std::string> Burst::splitStr(const std::string& s, const std::string& delim)
{
	std::vector<std::string> tokens;
	std::string::size_type pos, lastPos = 0, length = s.length();

	using size_type  = std::vector<std::string>::size_type;

	while(lastPos < length + 1)
	{
		pos = s.find_first_of(delim, lastPos);

		if(pos == std::string::npos)
			pos = length;

		if(pos != lastPos)
			tokens.push_back(std::string(s.data() + lastPos,
			static_cast<size_type>(pos-lastPos)));

		lastPos = pos + 1;
	}

	return tokens;
}

std::vector<std::string>& Burst::splitStr(const std::string& s, char delim, std::vector<std::string>& elems)
{
	std::stringstream ss(s);
	std::string item;

	while (std::getline(ss, item, delim))
		elems.push_back(item);

	return elems;
}

bool Burst::isValidPlotFile(const std::string& filePath)
{
	auto result = false;
	auto fileName = getFileNameFromPath(filePath);

	if (getAccountIdFromPlotFile(fileName) != "" &&
		getStartNonceFromPlotFile(fileName) != "")
	{
		//struct stat info;
		//int statResult = stat( filePath.c_str(), &info );

		//if( statResult >= 0)
		//{
		//    if( (info.st_mode & S_IFDIR) == 0)
		//    {
		std::ifstream testRead(filePath);

		if (testRead.good())
			result = true;

		testRead.close();
		//    }
		//}
	}

	return result;
}

std::string Burst::getAccountIdFromPlotFile(const std::string& path)
{
	return getInformationFromPlotFile(path, 0);
}

std::string Burst::getNonceCountFromPlotFile(const std::string& path)
{
	return getInformationFromPlotFile(path, 2);
}

std::string Burst::getStaggerSizeFromPlotFile(const std::string& path)
{
	return getInformationFromPlotFile(path, 3);
}

std::string Burst::getStartNonceFromPlotFile(const std::string& path)
{
	size_t filenamePos = path.find_last_of("/\\");
	if (filenamePos == std::string::npos)
	{
		filenamePos = 0;
	}
	std::vector<std::string> fileNamePart = splitStr(path.substr(filenamePos + 1, path.length() - (filenamePos + 1)), '_');
	if (fileNamePart.size() > 3)
	{
		std::string nonceStartPart = fileNamePart[1];
		if (isNumberStr(nonceStartPart))
		{
			return nonceStartPart;
		}
	}
	return "";
}

std::string Burst::deadlineFormat(uint64_t seconds)
{
	auto secs = seconds;
	auto mins = secs / 60;
	auto hours = mins / 60;
	auto day = hours / 24;
	auto months = day / 30;
	auto years = months / 12;
	
	std::stringstream ss;
	
	ss.imbue(std::locale(""));
	ss << std::fixed;
	
	if (years > 0)
		ss << years << "y ";
	if (months > 0)
		ss << months % 12 << "m ";
	if (day > 0)
		ss << day % 30 << "d ";

	ss << std::setw(2) << std::setfill('0');
	ss << hours % 24 << ':';
	ss << std::setw(2) << std::setfill('0');
	ss << mins % 60 << ':';
	ss << std::setw(2) << std::setfill('0');
	ss << secs % 60;

	return ss.str();
}

std::string Burst::gbToString(uint64_t size)
{
	std::stringstream ss;
	ss << std::fixed << std::setprecision(2);
	ss << size / 1024 / 1024 / 1024;
	return ss.str();
}

std::string Burst::versionToString()
{
	return std::to_string(Version::Major) + "." +
			std::to_string(Version::Minor) + "." +
			std::to_string(Version::Build);
}

std::string Burst::getInformationFromPlotFile(const std::string& path, uint8_t index)
{
	auto filenamePos = path.find_last_of("/\\");

	if (filenamePos == std::string::npos)
		filenamePos = 0;

	auto fileNamePart = splitStr(path.substr(filenamePos + 1, path.length() - (filenamePos + 1)), '_');

	if (index >= fileNamePart.size())
		return "";

	return fileNamePart[index];
}

Poco::Timespan Burst::secondsToTimespan(float seconds)
{
	auto secondsInt = static_cast<long>(seconds);
	auto microSeconds = static_cast<long>((seconds - secondsInt) * 100000);
	return Poco::Timespan{secondsInt, microSeconds};
}

std::unique_ptr<Poco::Net::HTTPClientSession> Burst::createSession(const Poco::URI& uri)
{
	return nullptr;
}

Poco::Net::SocketAddress Burst::getHostAddress(const Poco::URI& uri)
{
	Poco::Net::SocketAddress address{uri.getHost() + ':' + std::to_string(uri.getPort())};
	MinerLogger::write(address.toString());
	return address;
}

std::string Burst::serializeDeadline(const Deadline& deadline, std::string delimiter)
{
	return deadline.getAccountName() + delimiter +
		std::to_string(deadline.getBlock()) + delimiter +
		std::to_string(deadline.getDeadline()) + delimiter +
		std::to_string(deadline.getNonce());
}

Poco::JSON::Object Burst::createJsonDeadline(std::shared_ptr<Deadline> deadline)
{
	Poco::JSON::Object json;
	json.set("nonce", deadline->getNonce());
	json.set("deadline", deadlineFormat(deadline->getDeadline()));
	json.set("account", deadline->getAccountName());
	json.set("accountId", deadline->getAccountId());
	json.set("plotfile", deadline->getPlotFile());
	json.set("deadlineNum", deadline->getDeadline());
	return json;
}

Poco::JSON::Object Burst::createJsonDeadline(std::shared_ptr<Deadline> deadline, const std::string& type)
{
	auto json = createJsonDeadline(deadline);
	json.set("type", type);
	json.set("time", getTime());
	return json;
}

Poco::JSON::Object Burst::createJsonNewBlock(const MinerData& data)
{
	Poco::JSON::Object json;
	auto blockPtr = data.getBlockData();

	if (blockPtr == nullptr)
		return json;

	auto block = *blockPtr;
	auto bestOverall = data.getBestDeadlineOverall();

	json.set("type", "new block");
	json.set("block", block.block);
	json.set("scoop", block.scoop);
	json.set("baseTarget", block.baseTarget);
	json.set("gensigStr", block.genSig);
	json.set("time", getTime());
	json.set("blocksMined", data.getBlocksMined());
	if (bestOverall != nullptr)
	{
		json.set("bestOverallNum", bestOverall->getDeadline());
		json.set("bestOverall", deadlineFormat(bestOverall->getDeadline()));
	}
	json.set("deadlinesConfirmed", data.getConfirmedDeadlines());
	json.set("deadlinesAvg", deadlineFormat(data.getAverageDeadline()));
		
	Poco::JSON::Array bestDeadlines;

	for (auto& historicalDeadline : data.getAllHistoricalBlockData())
	{
		if (historicalDeadline->bestDeadline != nullptr)
		{
			Poco::JSON::Array jsonBestDeadline;
			jsonBestDeadline.add(historicalDeadline->block);
			jsonBestDeadline.add(historicalDeadline->bestDeadline->getDeadline());
			bestDeadlines.add(jsonBestDeadline);
		}
	}

	json.set("bestDeadlines", bestDeadlines);

	return json;
}

Poco::JSON::Object Burst::createJsonConfig()
{
	Poco::JSON::Object json;
	json.set("type", "config");
	json.set("poolUrl", MinerConfig::getConfig().getPoolUrl().getCanonical(true));
	json.set("miningInfoUrl", MinerConfig::getConfig().getMiningInfoUrl().getCanonical(true));
	json.set("walletUrl", MinerConfig::getConfig().getWalletUrl().getCanonical(true));
	json.set("totalPlotSize", MinerConfig::getConfig().getTotalPlotsize());
	json.set("timeout", MinerConfig::getConfig().getTimeout());
	return json;
}

Poco::JSON::Object Burst::createJsonProgress(float progress)
{
	Poco::JSON::Object json;
	json.set("type", "progress");
	json.set("value", progress);
	return json;
}

Poco::JSON::Object Burst::createJsonLastWinner(const MinerData& data)
{
	auto block = data.getBlockData();

	if (block == nullptr || block->lastWinner == nullptr)
		return Poco::JSON::Object{};

	return *block->lastWinner;
}

Poco::JSON::Object Burst::createJsonShutdown()
{
	Poco::JSON::Object json;
	json.set("shutdown", true);
	return json;
}

std::string Burst::getTime()
{
	auto now = std::chrono::system_clock::now();
	auto now_c = std::chrono::system_clock::to_time_t(now);
	std::stringstream ss;
	ss << std::put_time(std::localtime(&now_c), "%X");
	return ss.str();
}
