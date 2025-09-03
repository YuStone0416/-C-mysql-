#pragma once
#include <string>
#include <queue>
#include "connection.h"
#include <mutex>
#include <atomic>
#include <thread>
#include <memory>
#include <functional>
#include <condition_variable>
/*
ʵ�����ӳص�ģ��

*/
class ConnectionPool
{
public:
	//��ȡ���ӳض���ʵ��
	static ConnectionPool* getConnectionPool();
	//���ⲿ�ṩ�ӿڣ������ӳ��л�ȡһ�����õĿ�������
	std::shared_ptr<Connection> getConnection();//ʹ������ָ�뷵�أ�ʹ���Զ���ɾ����ʵ������ʱ�����ͷ���Դ�Ż����ӳء�
private:
	ConnectionPool();//#1 �����Ĺ��캯��˽�л�
	bool loadConfigFile();//�������ļ��м���������
	//�����ڶ������߳��У�ר�Ÿ�������������
	void produceConnectionTask();
	//�����ڶ������߳��У�ר�Ÿ���ɨ�����Ŀ������ӣ�����maxIdleTimeʱ��Ŀ������ӣ����ж�������ӻ���
	void scannerConnectionTask();
	std::string _ip;//mysql��ip��ַ
	unsigned short _port;//mysql�Ķ˿ں�
	std::string _username;//mysql�ĵ�¼�û���
	std::string _password;//mysql�ĵ�¼����
	std::string _dbname;//���ӵ����ݿ�����
	int _initSize;//���ӳصĳ�ʼ������
	int _maxSize;//���ӳص����������
	int _maxIdleTime;//���ӳ�������ʱ��
	int _connectionTimeout;//���ӳػ�ȡ���ӵĳ�ʱʱ��

	std::queue<Connection*> _connectionQue;//�洢mysql�Ķ���
	std::mutex _queueMutex;//ά�����Ӷ��е��̰߳�ȫ������
	std::atomic_int _connectionCnt;//��¼������������connection���ӵ�������
	std::condition_variable cv;//���������������������������̺߳������̵߳�ͨ��
};