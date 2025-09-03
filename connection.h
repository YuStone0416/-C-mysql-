#pragma once
#include<mysql.h>
#include <iostream>
#include <string>
#include <ctime>
/*
ʵ�����ݿ����ɾ�Ĳ�
*/
class Connection
{
public:
	//��ʼ�����ݿ�����
	Connection();
	//�ͷ����ݿ�������Դ
	~Connection();
	//�������ݿ�
	bool connect(std::string ip, 
		unsigned short port, 
		std::string user,
		std::string password,
		std::string dbname);
	//���²��� insert,delete,update
	bool update(std::string sql);
	//��ѯ����
	MYSQL_RES* query(std::string sql);

	//ˢ�����ӵ���ʼ�Ŀ���ʱ���
	void refreshAliveTime()
	{
		_alivetime = clock();
	}
	//���ش���ʱ��
	clock_t getAlivetime() const
	{
		return clock() - _alivetime;
	}
private:
	MYSQL* _conn;//��ʾ��MySQL Server��һ������
	clock_t _alivetime;//��¼�������״̬�����ʼ���ʱ��
};