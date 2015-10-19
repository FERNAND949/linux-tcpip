
#include <iostream>
#include <vector>
#include <string.h>
#include <unistd.h>  // read関数を使う為

#include <sys/socket.h>  // ソケット通信用 serverとclientの両方で必要
#include <error.h>  // エラー解析用
#include <netinet/in.h>  // sockaddr_in用
#include <arpa/inet.h>  // inet_addr用

// 受信とデータの表示
int Read(int& our)
{
	const int max_buf = 10;
	char data_buf[max_buf];

	int mRead = read(our, data_buf, sizeof(data_buf));
	int num;

	if (mRead <= 0) {
		perror("read error");

	} else if (mRead >= max_buf) {
		std::cout << "data over" << mRead << std::endl;

	} else
		std::cout << "read data = " << (uint)data_buf[0] << std::endl;

	num = (uint)data_buf[0];
	return num;
}

int main(int argc, char const* argv[])
{
	// サーバー用変数
	int port = 9999;  // ポート用
	int our_sock;  // 自分のソケット

	// AF_INETアドレスファミリーを使用する時は
	// sockaddr_inの構造体を使用する
	struct sockaddr_in server;

	// ソケットの作成
	// server,client共通
	// 今回はTCP/IPで作成するので以下のように引数を与える
	// 引数1:アドレスファミリ AF_INET:IPv4
	// 引数2:ソケットタイプ SOCK_STREAM:TCP
	// 引数3:プロトコル
	our_sock = socket(AF_INET, SOCK_STREAM, 0);

	if (our_sock < 0) {  // 失敗
		std::cout << "socket failed" << std::endl;
	} else
		std::cout << "socket success" << std::endl;

	// サーベーのデータ
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	// IPアドレスの指定(どんなアドレスでも可)
	server.sin_addr.s_addr = INADDR_ANY;

	// サーバーを切った後にタイムウェイトにしない
	int t = 1;
	setsockopt(our_sock, SOL_SOCKET, SO_REUSEADDR, (const sockaddr*)&t,
		   sizeof(int));

	// サーバー設定
	int Baind = bind(our_sock, (struct sockaddr*)&server, sizeof(server));
	// 失敗
	if (Baind != 0) {
		perror("bind error");
		return 0;
	}

	// サーバーを接続可能にする
	// ２つ目の引数が接続可能なクライアント数 大体５から１０
	int sListen = listen(our_sock, 5);
	// 失敗
	if (sListen != 0) {
		perror("listen error");
		return 0;
	}

	// サーバーを接続待機状態にする
	struct sockaddr_in client;
	socklen_t len = sizeof(client);
	int clientSock = accept(our_sock, (struct sockaddr*)&client, &len);
	if (clientSock < 0) {
		perror("client error");
		return 0;
	}

	// サーバーに接続したクライアントのアドレスとポートの情報
	std::cout << "server status" << std::endl;
	std::cout << "addr = " << inet_ntoa(client.sin_addr) << std::endl;
	std::cout << "port = " << ntohs(client.sin_port) << std::endl;

	int i;

	while (1) {

		// 接続しているクライアントから送られてくる情報を読む
		i = Read(clientSock);
		if (i == 0) break;
	}

	std::cout << "server stop" << std::endl;
	close(our_sock);
	close(clientSock);

	return 0;
}
