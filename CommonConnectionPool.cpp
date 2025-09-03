
#include "CommonConnectionPool.h"
#include "public.h"
//线程安全的懒汉单例函数接口
ConnectionPool* ConnectionPool::getConnectionPool()
{
	static ConnectionPool pool;
	return &pool;
}
//从配置文件中加载配置项
bool ConnectionPool::loadConfigFile()
{
	FILE* pf = fopen("mysql.ini", "r");
	if (pf == nullptr)
	{
		LOG("mysql.ini file is not exist!");
		return false;
	}
	//feof判断文件是否到末尾，是为真，不是为假
	while (!feof(pf))
	{
		char line[1024] = { 0 };
		fgets(line, 1024, pf);
		std::string str = line;
		int idx = str.find('=', 0);
		if (idx == -1)
		{
			continue;
		}
		int endidx = str.find('\n', idx);
		std::string key = str.substr(0, idx);
		std::string value = str.substr(idx + 1, endidx - idx - 1);

		if (key == "ip")
		{
			_ip = value;
		}
		else if (key == "port")
		{
			_port = atoi(value.c_str());
		}
		else if (key == "username")
		{
			_username = value;
		}
		else if (key == "password")
		{
			_password = value;
		}
		else if (key == "initSize")
		{
			_initSize = atoi(value.c_str());
		}
		else if (key == "maxSize")
		{
			_maxSize = atoi(value.c_str());
		}
		else if (key == "maxIdleTime")
		{
			_maxIdleTime = atoi(value.c_str());
		}
		else if (key == "connectionTimeout")
		{
			_connectionTimeout = atoi(value.c_str());
		}
		else if (key == "dbname")
		{
			_dbname = value;
		}
	}
	return true;
}
//连接池的构造
ConnectionPool::ConnectionPool()
{
	//加载配置项
	if (!loadConfigFile())
	{
		return;
	}
	//创建初始数量的连接
	for (int i = 0;i < _initSize;++i)
	{
		Connection* p = new Connection();
		p->connect(_ip, _port, _username, _password, _dbname);
		p->refreshAliveTime();//刷新进入空闲的起始时间
		_connectionQue.push(p);
		_connectionCnt++;
	}
	//启动一个新的线程，作为连接的生产者
	std::thread produce(std::bind(&ConnectionPool::produceConnectionTask,this));
	produce.detach();
	//启动一个新的定时线程，扫描多余的空闲连接，超过maxIdleTime时间的空闲连接，进行多余的连接回收
	std::thread scanner(std::bind(&ConnectionPool::scannerConnectionTask, this));
	scanner.detach();
}
//运行在独立的线程中，专门负责生产新连接
void ConnectionPool::produceConnectionTask()
{
	for (;;)
	{
		std::unique_lock<std::mutex> lock(_queueMutex);
		while (!_connectionQue.empty())
		{
			cv.wait(lock);//队列不空，此处生产线程进入等待状态
		}
		//连接数量没有到达上限，继续创建新的连接
		if (_connectionCnt < _maxSize)
		{
			Connection* p = new Connection();
			p->connect(_ip, _port, _username, _password, _dbname);
			p->refreshAliveTime();//刷新进入空闲的起始时间
			_connectionQue.push(p);
			_connectionCnt++;
		}
		//通知消费者线程，可以消费连接了
		cv.notify_all();
	}
}
//给外部提供接口，从连接池中获取一个可用的空闲连接
std::shared_ptr<Connection> ConnectionPool::getConnection()
{
	std::unique_lock<std::mutex> lock(_queueMutex);
	while(_connectionQue.empty())
	{
		//如果发现连接池没有连接，让消费者线程在_connectionTimeout时间内一直看是否可以拿到连接，这里不要用sleep
		if (std::cv_status::timeout == cv.wait_for(lock, std::chrono::milliseconds(_connectionTimeout)))
		{
			if (_connectionQue.empty())
			{
				LOG("获取空闲连接超时了。。。获取连接失败！");
				return nullptr;
			}
		}	
	}
	/*
	shared_ptr智能指针析构时，会把connection资源直接delete，相当于调用connection的析构函数，connection就被close掉。
	这里需要自定义shared_ptr的资源释放方式，把connection直接归还到queue当中
	*/
	std::shared_ptr<Connection> sp(_connectionQue.front(),
		[&](Connection* pcon)
		{
			//这里是在服务器应用线程中调用，所以一定要考虑队列的线程安全操作
			std::unique_lock<std::mutex> lock(_queueMutex);
			pcon->refreshAliveTime();//刷新进入空闲的起始时间
			_connectionQue.push(pcon);
		});
	_connectionQue.pop();
	if (_connectionQue.empty())
	{
		//谁消费了队列中的最后一个connection,谁负责通知一下生产者生产连接
		cv.notify_all();
	}
	return sp;
}

void ConnectionPool::scannerConnectionTask()
{
	for (;;)
	{
		//通过sleep模拟定时效果
		std::this_thread::sleep_for(std::chrono::seconds(_maxIdleTime));
		//扫描整个队列，释放多余的连接
		std::unique_lock<std::mutex> lock(_queueMutex);
		while (_connectionCnt > _initSize)
		{
			Connection* p = _connectionQue.front();
			if (p->getAlivetime() >= _maxIdleTime*1000)
			{
				_connectionQue.pop();
				_connectionCnt--;
				delete p;
			}
			else
			{
				break;//前面的空闲时间是大于后面的空闲时间
			}
		}
	}
}