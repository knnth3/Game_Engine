#include "Server.h"
#include "Address.h"
#include "EPacket.h"

namespace Net
{

	using namespace std::placeholders;

	Server::Server(int port)
	{
		InitializeSockets();
		//Init all data storage and sectors
		m_database = std::make_shared<DataBase>();
		m_recievedDB = std::make_shared<TDataBase>();
		m_sendingQueue = std::make_shared<std::queue<EPacket>>();
		m_ConnectionTimers = std::make_unique<IPAuthenticator>();
		m_transciever = std::make_unique<Transciever>(port);

		m_isInit = false;
	}

	bool Server::Initialize()
	{
		bool result = m_transciever->Init(m_recievedDB, m_sendingQueue);
		if (result)
		{
			m_transciever->SetConnectionFunc(std::bind(&Server::ConnectPeer, this, _1, _2));
			m_closeThread = false;
			m_asyncThread = std::async(std::launch::async, &Server::Update, this);
			m_isInit = true;
		}
		return result;
	}

	void Server::Update()
	{
		while (!m_closeThread)
		{
			m_transciever->Update();
			//Remove this when nodes are threaded!!!
			for (size_t index = 0; index < m_ActiveNodes.size(); index++)
			{
				m_ActiveNodes.at(index)->Update();
			}
		}
	}

	Identification Server::GetID(const char* name)
	{
		std::string user(name);
		if(m_connections.find(user) != m_connections.end())
			return m_connections[user]->GetID();
		return 0;
	}

	void Server::Send(Identification ID, const char* data)
	{
		if (IsValid(ID))
		{
			std::string strdata(data);
			edata info(strdata.begin(), strdata.end());
			ProgramData d = std::make_shared<edata>(info);
			m_database->at(ID)->at(outqueue).push(d);
		}
	}

	void Server::Send( Identification ID, std::vector<byte> data)
	{
		if (IsValid(ID))
		{
			ProgramData d = std::make_shared<edata>(data);
			m_database->at(ID)->at(outqueue).push(d);
		}
	}

	bool Server::Recieve(Identification ID, char* data, uint32_t maxSize)
	{
		edata info;
		if (IsValid(ID))
		{
			if (!m_database->at(ID)->at(inqueue).empty())
			{
				info = *(m_database->at(ID)->at(inqueue).front()).get();
				m_database->at(ID)->at(inqueue).pop();
			}
		}
		std::string strdata(info.begin(), info.end());
		uint32_t len = static_cast<uint32_t>(strdata.length());
		if (strdata.empty())
		{
			return false;
		}

		if (maxSize >= len)
		{
			memcpy(data, strdata.c_str(), len);
			return true;
		}
		return false;

	}

	size_t Server::GetNumOfUsers()
	{
		return m_connections.size();
	}

	NET_API bool Server::GetNewUsername(char * name, uint32_t size)
	{
		std::string query = "";
		std::string command = "!get";
		AccessNewUsernames(query, command);
		if (query.size() > 1)
		{
			if (query.size() <= size)
			{
				memcpy(name, query.data(), size);
				name[query.size()] = '\0';
				return true;
			}
		}
		return false;
	}

	void Server::Close()
	{
		if (m_isInit)
		{
			using namespace std::chrono_literals;
			m_closeThread = true;
			std::future_status status;
			do
			{
				status = m_asyncThread.wait_for(0ms);
			} while (status != std::future_status::ready);

			m_transciever->Close();
			ShutdownSockets();
		}
	}

	void Server::CreateNode(std::shared_ptr<Address>& address)
	{
		//Create threads here for optimal work
		Identification id = address->GetID();
		m_database->emplace(id, std::make_shared<std::vector<std::queue<ProgramData>>>());
		m_recievedDB->emplace(id, std::make_shared<std::queue<EPacket>>());
		m_ActiveNodes.emplace_back
		(
			std::make_unique<Node>
			(
				m_database->at(id),
				m_recievedDB->at(id),
				m_sendingQueue,
				address
				)
		);
	}

	ConnectionType Server::ConnectPeer(std::shared_ptr<Address>& address, ProgramData& data)
	{
		if (IsAddressQueried(address))
		{
			return ConnectionType::Success;
		}
		if (!m_ConnectionTimers->IsTimedOut(address->GetAddress()))
		{
			if (!data->empty() && data->at(0) == '!')
			{
				//Identification process
				std::string username;
				std::string value;
				Identification key = 0;
				Identification derivedkey = htons(GenerateKey(CONNECTION_KEY));

				bool keyObtained = false;
				for (auto x = data->begin() + 1; x < data->end(); x++)
				{
					char c = *x;
					if (!(c == '|'))
					{
						if (!keyObtained)
							value.push_back(c);
						else
							username.push_back(c);
					}
					else
					{
						keyObtained = true;
					}
				}

				key = static_cast<Identification>(std::stoi(value));

				printf("Key: %u, Value: %s, Username: %s, derivedKey: %u\n", key, value.c_str(), username.c_str(), derivedkey);

				//create connection if valid
				if (key == derivedkey)
				{
					address->AssignID(ntohs(key));
					address->AssignName(username);
					m_connections[username] = address;
					AddNewUsername(username);
					CreateNode(address);

					std::string msg = "!Hey, you logged in!";
					edata loginData = std::vector<byte>(msg.begin(), msg.end());
					Send(m_connections[username]->GetID(), loginData);

					msg = "L|" + username;
					loginData = std::vector<byte>(msg.begin(), msg.end());
					data = std::make_shared<edata>(loginData);
				}
				else
				{
					printf("Login failed, Invalid Key!\n");
					return ConnectionType::Failed;
				}
			}
		}
		return ConnectionType::NewConnect;
	}

	Identification Server::GenerateKey(Identification seed)
	{
		return ((((seed / 2) + 5724) % 100001) >> 8);
	}

	bool Server::IsValid(Identification ID)
	{
		if (m_database->find(ID) == m_database->end())
		{
			return false;
		}
		return true;
	}

	bool Server::IsAddressQueried(std::shared_ptr<Address>& address)
	{
		for each(auto x in m_connections)
		{
			if (x.second->GetAddress() == address->GetAddress())
			{
				address->AssignID(x.second->GetID());
				return true;
			}
		}
		return false;
	}

	void Server::AddNewUsername(std::string name)
	{
		std::string empty = "";
		AccessNewUsernames(name, empty);
	}

	bool Server::AccessNewUsernames(std::string& nameToChangeTo, std::string& nameToLookFor)
	{
		std::lock_guard<std::mutex> guard(m_Lock);

		if (nameToLookFor.compare("!get") == 0)
		{
			for(size_t it = 0; it < m_newUsernames.size(); it++)
			{
				if (m_newUsernames[it].compare("") != 0)
				{
					nameToChangeTo = m_newUsernames[it];
					printf("Found %s: %s\n", nameToLookFor.c_str(), nameToChangeTo.c_str());
					m_newUsernames[it] = "";
					return true;
				}
			}
		}
		else
		{
			size_t pos = 0;
			for (size_t it = 0; it < m_newUsernames.size(); it++)
			{
				if (m_newUsernames[it] == nameToLookFor)
				{
					pos = it;
					break;
				}
			}
			if (pos >= m_newUsernames.size())
			{
				m_newUsernames.push_back(nameToChangeTo);
				return true;
			}
			else
			{
				m_newUsernames.at(pos) = nameToChangeTo;
				return true;
			}
		}
		return false;
	}



}
