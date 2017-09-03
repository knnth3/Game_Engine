#pragma once
#include <chrono>
#include <atomic>
#include <future>
#include <mutex>
#include "NetModule.h"
#include "Basics.h"
#include "IPAuthenticator.h"
#include "Transciever.h"
#include "Node.h"

//Server program and fundamentals
//Author: Eric Marquez
//+Includes the ability to poll connections for various uses
//+Opens new thread that controlls the node for a specific peer
//+Uses map<index, queue> for quick indexing, insert, and extract
//+ID can be obtained from m_connections with username
//Transciever queue

namespace Net
{
	typedef std::map<Identification, PDQueue> DataBase;

	class Server
	{
	public:
		NET_API Server(int port);
		NET_API bool Initialize();
		NET_API Identification GetID(std::string username);
		NET_API void Send(Identification ID, std::vector<byte> data);
		NET_API std::vector<byte> Recieve(Identification ID);
		NET_API size_t GetNumOfUsers();
		NET_API std::vector<std::string> GetNewUsernames();
		NET_API void GetConnectedUsers(std::vector<std::string>& usernames);
		NET_API void Close();

	private:
		NET_API void Update();
		NET_API void CreateNode(std::shared_ptr<Address>& address);
		//Function used to Connect peer(Completely customizable)
		//although nothing will be sent/recieved if a node is not created.
		NET_API ConnectionType ConnectPeer(std::shared_ptr<Address>& address, ProgramData& packet);
		NET_API Identification GenerateKey(Identification seed);
		NET_API bool IsValid(Identification ID);
		NET_API bool IsAddressQueried(std::shared_ptr<Address>& address);
		NET_API void AddNewUsername(std::string name);
		NET_API bool AccessNewUsernames(std::string& nameToChangeTo, std::string& nameToLookFor);

		std::vector<std::unique_ptr<Node>> m_ActiveNodes;
		std::unique_ptr<Transciever> m_transciever;
		std::unique_ptr<IPAuthenticator> m_ConnectionTimers;
		
		// IDs of all connected mapped to username;
		std::map<std::string, std::shared_ptr<Address>> m_connections;
		std::vector<std::string> m_newUsernames;
		
		//Stores data sent from the program that is to be sent to given peer
		std::shared_ptr<DataBase> m_database;
		
		//Stores packets that have been sent/recieved
		std::shared_ptr<TDataBase> m_recievedDB;
		TQueue m_sendingQueue;
		
		std::atomic<bool> m_closeThread;
		std::future<void> m_asyncThread;
		std::mutex m_Lock;
		
		bool m_isInit;
	};


}