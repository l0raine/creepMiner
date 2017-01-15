//  cryptoport.io Burst Pool Miner
//
//  Created by Uray Meiviar < uraymeiviar@gmail.com > 2014
//  donation :
//
//  [Burst  ] BURST-8E8K-WQ2F-ZDZ5-FQWHX
//  [Bitcoin] 1UrayjqRjSJjuouhJnkczy5AuMqJGRK4b

#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <Poco/Timespan.h>
#include <memory>
#include <Poco/Net/SocketAddress.h>
#include <array>

namespace Poco
{
	class URI;
	namespace Net { class HTTPClientSession; }
	namespace JSON { class Object; }
}

namespace Burst
{
	class PlotReadProgress;
	struct BlockData;
	class Deadline;
	class MinerData;

	template <typename T, size_t SZ>
	std::string byteArrayToStr(const std::array<T, SZ>& arr)
	{
		std::stringstream stream;
		for (size_t i = 0; i < SZ; i++)
		{
			stream << std::setfill('0') << std::setw(sizeof(T) * 2) << std::hex << static_cast<size_t>(arr[i]);
		}
		return stream.str();
	}

	bool isNumberStr(const std::string& str);
	std::string getFileNameFromPath(const std::string& strPath);
	std::vector<std::string>& splitStr(const std::string& s, char delim, std::vector<std::string>& elems);
	std::vector<std::string> splitStr(const std::string& s, char delim);
	std::vector<std::string> splitStr(const std::string& s, const std::string& delim);
	bool isValidPlotFile(const std::string& filePath);
	std::string getAccountIdFromPlotFile(const std::string& path);
	std::string getStartNonceFromPlotFile(const std::string& path);
	std::string getNonceCountFromPlotFile(const std::string& path);
	std::string getStaggerSizeFromPlotFile(const std::string& path);
	std::string deadlineFormat(uint64_t seconds);
	std::string gbToString(uint64_t size);
	std::string versionToString();
	std::string getInformationFromPlotFile(const std::string& path, uint8_t index);

	template <typename T, typename U>
	void transferSession(T& from, U& to)
	{
		auto socket = from.transferSession();

		if (socket != nullptr)
			to = std::move(socket);
	}

	Poco::Timespan secondsToTimespan(float seconds);
	std::unique_ptr<Poco::Net::HTTPClientSession> createSession(const Poco::URI& uri);
	Poco::Net::SocketAddress getHostAddress(const Poco::URI& uri);
	std::string serializeDeadline(const Deadline& deadline, std::string delimiter = ":");

	Poco::JSON::Object createJsonDeadline(std::shared_ptr<Deadline> deadline);
	Poco::JSON::Object createJsonDeadline(std::shared_ptr<Deadline> deadline, const std::string& type);
	Poco::JSON::Object createJsonNewBlock(const MinerData& data);
	Poco::JSON::Object createJsonConfig();
	Poco::JSON::Object createJsonProgress(float progress);
	Poco::JSON::Object createJsonLastWinner(const MinerData& data);
	Poco::JSON::Object createJsonShutdown();

	std::string getTime();
}
